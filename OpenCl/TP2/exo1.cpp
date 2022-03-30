//
// Created by mamangvomi on 28/03/2022.
//
#define __CL_ENABLE_EXCEPTIONS

#include "CL/cl.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <ctime>

#define NOM_CL "calcul_pi.cl"
#define FILE_MNT "mat_test.txt"
#define CPU
//#define GPU

void compute_flow_accumulation(int *pInt, int *pInt1, int nx, int ny);

int compute_from_index(int index, int i, int j, int nx, int ny, int *pInt, int *res);

bool has_zero(int *pInt, int nx, int ny);

void read_file(char *name, int *nx, int *ny, int *gauche, int *droit, int *cell, int *nodata, float **tab) {
    FILE *fp = fopen(name, "r");
    if (fp == NULL) {
        exit(0);
    }
    fscanf(fp, "%d", nx);
    fscanf(fp, "%d", ny);
    fscanf(fp, "%d", gauche);
    fscanf(fp, "%d", droit);
    fscanf(fp, "%d", cell);
    fscanf(fp, "%d", nodata);
    *tab = new float[(*nx) * (*ny)];
    float hauteur;
    for (int i = 0; i < *nx; i++) {
        for (int j = 0; j < *ny; j++) {
            fscanf(fp, "%f", &hauteur);
            (*tab)[i * (*nx) + j] = hauteur;
        }
    }
    fclose(fp);
}

void affiche_device(cl::Device device) {
    std::cout << "\t\tDevice Name: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    std::cout << "\t\tDevice Type: " << device.getInfo<CL_DEVICE_TYPE>();
    std::cout << " (GPU: " << CL_DEVICE_TYPE_GPU << ", CPU: " << CL_DEVICE_TYPE_CPU << ")" << std::endl;
    std::cout << "\t\tDevice Vendor: " << device.getInfo<CL_DEVICE_VENDOR>() << std::endl;
    std::cout << "\t\tDevice Max Compute Units: " << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
    std::cout << "\t\tDevice Global Memory: " << device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() << std::endl;
    std::cout << "\t\tDevice Max Clock Frequency: " << device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << std::endl;
    std::cout << "\t\tDevice Max Allocateable Memory: " << device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() << std::endl;
    std::cout << "\t\tDevice Local Memory: " << device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() << std::endl;
    std::cout << "\t\tDevice Available: " << device.getInfo<CL_DEVICE_AVAILABLE>() << std::endl;
    std::cout << "\t\tMax Work-group Total Size: " << device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << std::endl;
    std::vector<size_t> d = device.getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
    std::cout << "\t\tMax Work-group Dims: (";
    for (std::vector<size_t>::iterator st = d.begin(); st != d.end(); st++)
        std::cout << *st << " ";
    std::cout << "\x08)" << std::endl;
}

cl::Program creationProgramme(std::string nomFicSource, cl::Context contexte) {
    // lecture du programme source
    std::ifstream sourceFile(nomFicSource);
    std::string sourceCode(std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
    // la premier argument indique le nombre de programmes sources utilisés, le deuxième est une paire (texte, taille du programme)
    cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));
    // creation du programme dans le contexte
    return cl::Program(contexte, source);
}

void affiche_mat_float(float *mat, int nx, int ny, char *nom) {
    std::cout << "Matrice : " << nom << std::endl;
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++)
            std::cout << mat[i * nx + j] << " ";
        std::cout << std::endl;
    }
}

void affiche_mat_int(int *mat, int nx, int ny, char *nom) {
    std::cout << "Matrice : " << nom << std::endl;
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++)
            std::cout << mat[i * nx + j] << " ";
        std::cout << std::endl;
    }
}

int get_direction_code(int index) {
    switch (index) {
        case 0:
            return 2;
        case 1:
            return 3;
        case 2:
            return 1;
        case 3:
            return 4;
        case 4:
            return 8;
        case 5:
            return 6;
        case 6:
            return 5;
        case 7:
            return 7;
        default:
            return 0;
    }
}

char *get_arrow(int index) {
    switch (index) {
        case 2:
            return "↑";
        case 3:
            return "↗";
        case 1:
            return "↖";
        case 4:
            return "→";
        case 8:
            return "←";
        case 6:
            return "↓";
        case 5:
            return "↘";
        case 7:
            return "↙";
        case 0:
            return " ";
    }
}

void direction_cpu(float *mat, int *res, int nx, int ny) {
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            float *voisins = new float[8]{-1, -1, -1, -1, -1, -1, -1, -1};  // [n, ne, no, e, o, s, se, so]
            int index = i * nx + j;
            float min = mat[index];
            int index_min = -1;
            //NORD
            if (i > 0) {
                voisins[0] = mat[(i - 1) * nx + j]; // n
                if (j > 0) {
                    voisins[2] = mat[(i - 1) * nx + (j - 1)]; // no
                }
                if (j < ny - 1) {
                    voisins[1] = mat[(i - 1) * nx + (j + 1)]; // ne
                }
            }
            //SUD
            if (i < nx - 1) {
                voisins[5] = mat[(i + 1) * nx + j]; // s
                if (j > 0) {
                    voisins[7] = mat[(i + 1) * nx + (j - 1)]; // so
                }
                if (j < ny - 1) {
                    voisins[6] = mat[(i + 1) * nx + (j + 1)]; // se
                }
            }
            //Ouest
            if (j > 0) {
                voisins[4] = mat[index - 1]; // o
            }
            //Est
            if (j < ny - 1) {
                voisins[3] = mat[index + 1]; // e
            }
            for (int k = 0; k < 8; ++k) {
                // Pour chaque voisins
                if (voisins[k] != -1 && min > voisins[k]) {
                    min = voisins[k];
                    index_min = k;
                }
            }
            res[index] = get_direction_code(index_min);
        }
    }
}

void print_in_file(int *res, int nx, int ny, char *nom, bool is_arrow) {
    std::ofstream myfile;
    myfile.open("max_direction.txt", std::ios::app);
    myfile << "Matrice : " << nom << std::endl;
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++)
            (is_arrow) ? myfile << get_arrow(res[i * nx + j]) << " " : myfile << res[i * nx + j] << " ";
        myfile << "\n";
    }
    myfile.close();
}

int main(int argc, char **argv) {

#ifdef CPU
    int nx, ny, gauche, droit, cell, nodata;
    float *mat_hauteur;
    read_file(FILE_MNT, &nx, &ny, &gauche, &droit, &cell, &nodata, &mat_hauteur);
    int *mat_direction = new int[nx * ny];
    direction_cpu(mat_hauteur, mat_direction, nx, ny);
    print_in_file(mat_direction, nx, ny, "de direction", true);
    int *mat_flot_accum = new int[nx * ny];
    for (int i = 0; i < nx * ny; ++i) {
        mat_flot_accum[i] = 0;
    }
    compute_flow_accumulation(mat_flot_accum, mat_direction, nx, ny);
    print_in_file(mat_flot_accum, nx, ny, "de flot d'accumulation", false);
#endif
#ifdef GPU
    try {
        // debut de la zone d'utilisation de l'API pour OpenCL

        // les plateformes
        std::vector<cl::Platform> plateformes;
        cl::Platform::get(&plateformes); // recherche des plateformes normalement 1 sur un PC

        //les devices
        std::vector<cl::Device> devices;
        plateformes[0].getDevices(CL_DEVICE_TYPE_ALL, &devices); // recherche des devices (normalement 1)

        // affichage des informations des devices
        for (int i = 0; i < devices.size(); i++) {
            std::cout << "\tDevice " << i << ": " << std::endl;
            affiche_device(devices[i]);
        }

        // création d'un contexte pour les devices
        cl::Context contexte(devices);
        // création du programme dans le contexte (voir code fonction)
        // cl::Program programme=creationProgramme("mult_mat.cl",contexte);
//        cl::Program programme = creationProgramme(NOM_CL, contexte);
//        // compilation du programme
//        try {
//            programme.build(devices);
//        } catch (...) {
//            // Récupération des messages d'erreur au cas où...
//            cl_int buildErr = CL_SUCCESS;
//            auto buildInfo = programme.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0], &buildErr);
//            std::cerr << buildInfo << std::endl << std::endl;
//            exit(0);
//        }
        // création de la file de commandes (ordres de l'hote pour le GPU)
//        cl::CommandQueue queue = cl::CommandQueue(contexte, devices[0]);
//        test_GPU(programme,queue,contexte);
    } catch (cl::Error err) { // Affichage des erreurs en cas de pb OpenCL
        std::cout << "Exception\n";
        std::cerr << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
        return EXIT_FAILURE;
    }
#endif
}

void compute_flow_accumulation(int *res, int *direction, int nx, int ny) {
    int i = 0;
    while (has_zero(res, nx, ny)) {
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                int index = i * nx + j;
                int val = 0;
                if (i > 0) {
                    if (direction[(i - 1) * nx + j] == 6) {
                        if (res[(i - 1) * nx + j] != 0) {
                            val += res[(i - 1) * nx + j]; // n
                        } else {
                            continue;
                        }
                    }
                    if (j > 0) {
                        if (direction[(i - 1) * nx + (j - 1)] == 5)
                            if (res[(i - 1) * nx + (j - 1)] != 0) {
                                val += res[(i - 1) * nx + (j - 1)];// no

                            } else {
                                continue;
                            }
                    }
                    if (j < ny - 1) {
                        if (direction[(i - 1) * nx + (j + 1)] == 7)
                            if (res[(i - 1) * nx + (j + 1)] != 0) {
                                val += res[(i - 1) * nx + (j + 1)];// ne

                            } else {
                                continue;
                            }
                    }
                }
                //SUD
                if (i < nx - 1) {
                    if (direction[(i + 1) * nx + j] == 2)
                        if (res[(i + 1) * nx + j] != 0) {
                            val += res[(i + 1) * nx + j];// s

                        } else {
                            continue;
                        }
                    if (j > 0) {
                        if (direction[(i + 1) * nx + (j - 1)] == 3)
                            if (res[(i + 1) * nx + (j - 1)] != 0) {
                                val += res[(i + 1) * nx + (j - 1)];// so

                            } else {
                                continue;
                            }
                    }
                    if (j < ny - 1) {
                        if (direction[(i + 1) * nx + (j + 1)] == 1)
                            if (res[(i + 1) * nx + (j + 1)] != 0) {
                                val += res[(i + 1) * nx + (j + 1)];// se

                            } else {
                                continue;
                            }
                    }
                }
                //Ouest
                if (j > 0) {
                    if (direction[index - 1] == 4)
                        if (res[index - 1] != 0) {
                            val += res[index - 1];// o
                        } else {
                            continue;
                        }
                }
                //Est
                if (j < ny - 1) {
                    if (direction[index + 1] == 8)
                        if (res[index + 1] != 0) {
                            val += res[index + 1];// e

                        } else {
                            continue;
                        }
                }
                val++;
                res[index] = val;
            }
        }
        i++;
    }
}

bool has_zero(int *pInt, int nx, int ny) {
    for (int i = 0; i < nx * ny; ++i) {
        if (pInt[i] == 0) {
            return true;
        }
    }
    return false;
}



