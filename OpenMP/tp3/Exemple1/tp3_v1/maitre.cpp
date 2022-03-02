#include <mpi.h>
#include <iostream>
using namespace std;
int main(int argc, char**argv){
  int nmasters, pid;
  MPI_Comm intercom;
  MPI_Init (&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&nmasters);
  MPI_Comm_rank(MPI_COMM_WORLD,&pid);

  // le code des maîtres

  int n = atoi(argv[1]);
  int nslaves = atoi(argv[2]);
  int root = atoi(argv[3]);

  cout << "je suis le maître " << pid << " parmi " << nmasters << " maîtres" << endl;
  int* tab = new int[n];

  srand(time(NULL));
  for (int i=0; i<n; i++)
      tab[i] = rand()%100;

  MPI_Comm_spawn("esclave", // exécutable
                     argv, // arguments à passer sur la ligne de commande
                     nslaves, // nombre de processus esclave
                     MPI_INFO_NULL, //permet de préciser où et comment lancer les processus
                     root, // rang du processus maître qui effectue réellement le spawn
                     MPI_COMM_WORLD, // Monde dans lequel est effectué le spwan
                     &intercom, // l'intercommunicateur permettant de communiquer avec les esclaves
                     MPI_ERRCODES_IGNORE // tableau contenant les erreurs
    );


  int reste = n % nslaves; 
  int* sendcount = new int[nslaves];
  int* displ = new int[nslaves];
  cout << "sendcount ";
  for(size_t i = 0; i < nslaves; i++){
    sendcount[i] = (i < reste) ? n / nslaves + 1 : n / nslaves;
    cout<< i << ": "<< sendcount[i] << " ";
  }
  cout << "displ ";
  for (size_t i = 0; i < nslaves; i++){
    displ[i] = i==0 ? 0 : displ[i-1] + sendcount[i-1];
    cout<< i << ": "<< displ[i] << " ";

  }

cout << "tab chez le maitre ;"; 
for (size_t i = 0; i < n; i++)
{
  cout << tab[i] << " ";
}
cout << endl;
  
for (int i = 0; i < nslaves; i++){
    MPI_Ssend(tab+displ[i], sendcount[i], MPI_INT, i, 10, intercom);
}

int* all_min = new int[nslaves];
for (size_t i = 0; i < nslaves; i++)
{
  MPI_Recv(all_min+i, 1, MPI_INT, i, 20+i, intercom, MPI_STATUS_IGNORE);
}
cout << "rec des min : "<< endl;
int min = all_min[0];
for (size_t i = 0; i < nslaves; i++)
{
  if(all_min[i] < min) {
    min = all_min[i];
  }
  cout << all_min[i] << endl;
}

  cout << "####RES : "<< min<< endl;
  MPI_Comm_free(&intercom);
  MPI_Finalize();
  return 0;
}
