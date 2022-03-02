#include <mpi.h>
#include <iostream>
using namespace std;

int main(int argc, char**argv){
  int nslaves, pid, nmasters,flag;
  MPI_Comm intercom;

  MPI_Init (&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&nslaves);
  MPI_Comm_rank(MPI_COMM_WORLD,&pid);
  MPI_Comm_get_parent(&intercom); // obtention de l'intercommunicateur vers le/les parents
  MPI_Comm_remote_size(intercom,&nmasters); // permet de connaître le nombre de parents
  // code de l'esclave

  cout << "je suis l'esclave " << pid << " parmi " << nslaves << " esclaves et avec " << nmasters << " parents" << endl;

  // Attention pour les esclaves on a esclave ./maitre 16 4 0 donc n est l'argument n°2
  int n = atoi(argv[2]);
  int root = atoi(argv[4]);
  int reste = n % nslaves;
  int taille_tab = pid < reste ? n/nslaves + 1 : n/nslaves;
  int* tab = new int[taille_tab];

  MPI_Status status;
  MPI_Recv(tab, taille_tab, MPI_INT,root,10,intercom,&status);

  int min = tab[0];
  for (int i=0; i<taille_tab; i++){
    if (tab[i] <= min){
      min = tab[i];
    }
  }
  cout << "min de l'esclave "<< pid << " : "<< min << endl;  
  MPI_Send(&min, 1, MPI_INT, root, 20+pid, intercom);

  MPI_Finalize();
  return 0;
}
