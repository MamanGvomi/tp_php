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
    
    
    int * nb_par_t = new int[nprocs];
    for (size_t i = 0; i < nprocs; i++)
    {    
        int reste = m % nprocs;
        nb_par_t[i] = (pid < reste) ? m / nprocs + 1 : m / nprocs;
   
    }
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

    // A compléter également avec la récupération des résulta
    int* vecteurs_th = new int[n * nb_par_t[pid]];
    if(pid == root){
        for (size_t i = 0; i < n * nb_par_t[pid]; i++)
        {
            vecteurs_th[i] = vecteurs[i];
        }
    } 
    else{
        int offset = 0;
        for (size_t i = 0; i < pid; i++)
        {
            offset += nb_par_t[i] * n;
        }
        
        MPI_Get(matrice, n*n, MPI_INT, root, 0, n*n,  MPI_INT, window_matrice); // On get la matrice
        MPI_Get(vecteurs_th, n*nb_par_t[pid], MPI_INT, root, offset, n * nb_par_t[pid],  MPI_INT, window_vecteurs); // On get ses vecteurs
    }
    MPI_Win_fence( 0 , window_vecteurs);
    cout << "les vecteurs de pid " << pid << endl;
        for (size_t i = 0; i < nb_par_t[pid]; i++)
        {
            cout << "vecteur : "; 
            for (size_t j = 0; j < n ; j++)
            {
                cout << vecteurs_th[j + i*n] << " ";
            }
           cout << endl;
        }
    
    matrice_vecteur(n, matrice, vecteurs_th);

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
