#include <iostream>
#include <fstream>
#include <chrono>

#include <mpi.h>
#include <omp.h>

#include "fonctions.h"

using namespace std;

int main(int argc, char **argv) {

    // Pour initialiser l'environnement MPI avec la possibilité d'utiliser des threads (OpenMP)
    int provided; // renvoi le mode d'initialisation effectué
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided); // MPI_THREAD_MULITPLE chaque processus MPI peut faire appel à plusieurs threads.

    // Pour connaître son pid et le nombre de processus de l'exécution paralléle (sans les threads)
    int pid, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int n = atoi(argv[1]); // taille de la matrice carrée
    int m = atoi(argv[2]); // nombre de vecteurs en entrée
    int root = atoi(argv[3]); // processeur root : référence pour les données
    string name = argv[4]; // le nom du fichier pour que le processus root copie les données initiales et les résultats
    int x = atoi(argv[5]); // Nb vec par batch
    
    // Petit test pour vérifier qu'on peut avoir plusieurs threads par processus.
#pragma omp parallel num_threads(4)
    {
      int id = omp_get_thread_num();
#pragma omp critical
      cout << "je suis le thread " << id << " pour pid=" << pid << endl;
    }


    // Pour mesurer le temps (géré par le processus root)
    chrono::time_point<chrono::system_clock> debut, fin;

    
    int *matrice = new int[n * n]; // la matrice
    int *vecteurs; // l'ensemble des vecteurs connu uniquement par root et distribué à tous.


    fstream f;
    if (pid == root) {
        f.open(name, std::fstream::out);
        srand(time(NULL));
        for (int i = 0; i < n * n; i++)
            matrice[i] = rand() % 20;

        f << "Matrice" << endl;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++)
                        f << matrice[i * n + j] << " ";
            f << endl;
        }
        f << endl;

        vecteurs = new int[m * n];
        for (int i = 0; i < m; i++) {
            int nb_zero = rand() % (n / 2);
            generation_vecteur(n, vecteurs + i * n, nb_zero);
        }
        f << "Les vecteurs" << endl;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++)
                f << vecteurs[i * n + j] << " ";
            f << endl;
        }
    }
    MPI_Win window_matrice;
    MPI_Win window_vecteurs;
    if (pid == root)
    {
        MPI_Win_create(matrice , n*n*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD , &window_matrice);
        MPI_Win_create(vecteurs , n*m*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD , &window_vecteurs);
    }else{
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD , &window_matrice);
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD , &window_vecteurs);
    }
    if (pid == root) {
        fin = chrono::system_clock::now();
        chrono::duration<double> elapsed_seconds = fin - debut;
        cout << "temps en secondes : " << elapsed_seconds.count() << endl;
    }

    MPI_Win_fence(0, window_matrice);
    MPI_Win_fence(0, window_vecteurs);
 
    int nb_batch =  m / x;
    int batch_par_thread = nb_batch / nprocs;
    int reste_batch = nb_batch % nprocs;
    int* my_number_batch = pid < reste_batch ? batch_par_thread + 1 : batch_par_thread;

    for (size_t i = 0; i < my_number_batch[pid]; i++)
    {
        
    }
    



    MPI_Win_fence(0, window_vecteurs);

    int* res = new int[n * m];
    if (pid == root) {
      f << "Les vecteurs résultat sur le root " << endl;
      for (int i = 0; i < m; i++) {
	for (int j = 0; j < n; j++)
	  f << res[i * n + j] << " ";
	f << endl;
      }
      f.close();
    }
    MPI_Win_free(&window_vecteurs);
    MPI_Win_free(&window_matrice);

    
    MPI_Finalize();
    return 0;
}
