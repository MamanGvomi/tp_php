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
    MPI_Bcast(matrice, n*n, MPI_INT, root, MPI_COMM_WORLD); // envoie de la matrice aux autres thread

    //Variables pour les scatter des vecteurs
    int* sendcounts = new int[nprocs];
    int* displ = new int[nprocs];
    int nb_vecteurs_par_threads = m / nprocs;
    int restes = m % nprocs;
    int* tab_thread_vecteur = new int[nprocs];
    for (size_t i = 0; i < nprocs; i++)
    {
        tab_thread_vecteur[i] = (i < restes) ? nb_vecteurs_par_threads + 1 : nb_vecteurs_par_threads;
    }
    int taille_rec = tab_thread_vecteur[pid] * n;
    int* recvecteur = new int[taille_rec];

    if (pid == root){
        debut = chrono::system_clock::now();
        for(int i = 0; i < nprocs; i++){
            sendcounts[i] = tab_thread_vecteur[i] * n;
        }
        int init_displ = 0;
        for(int i = 0; i < nprocs; i ++){
            displ[i] = init_displ;
            init_displ += sendcounts[i];
        }
    }
    f << "Le sendcounts : " << endl;
    for (size_t i = 0; i < nprocs; i++)
    {
        f << "pid " << i << " : " << sendcounts[i] << endl;
    }
    f << "Le displ : " << endl;
    for (size_t i = 0; i < nprocs; i++)
    {
        f << "pid " << i << " : " << displ[i] << endl;
    }
    
    MPI_Scatterv(vecteurs, sendcounts, displ, MPI_INT, recvecteur, taille_rec, MPI_INT, root, MPI_COMM_WORLD);

    // A compléter à partir d'ici.
    int* vecRes = new int[taille_rec]; // Local pour get les res dedans
    for (size_t i = 0; i < tab_thread_vecteur[pid]; i++)
    {
        matrice_vecteur(n, matrice, recvecteur + (i * n), vecRes + (i * n)); // chaque thread fais le calcule

    }
    int* res = new int[m*n];  
    MPI_Gatherv(vecRes, taille_rec, MPI_INT, res, sendcounts, displ, MPI_INT , root, MPI_COMM_WORLD);
    // Dans le temps écoulé on ne s'occupe que de la partie communications et calculs
    // (on oublie la génération des données et l'écriture des résultats sur le fichier de sortie)
    if (pid == root) {
        fin = chrono::system_clock::now();
        chrono::duration<double> elapsed_seconds = fin - debut;
        cout << "temps en secondes : " << elapsed_seconds.count() << endl;
    }
    
    // A compléter également avec la récupération des résulta
    
    if (pid == root) {
      f << "Les vecteurs résultat sur le root " << endl;
      for (int i = 0; i < m; i++) {
	for (int j = 0; j < n; j++)
	  f << res[i * n + j] << " ";
	f << endl;
      }
      f.close();
    }
    
    MPI_Finalize();
    return 0;
}
