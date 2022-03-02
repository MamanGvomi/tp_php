__kernel
void mult_mat(const int taille, __global int * M1, __global int *M2, __global int *R){
   int i=get_global_id(0) * taille;
   int val = 0;
 for (size_t j = 0; j < taille; j++)
    {
        val = 0;
      for (size_t k = 0; k < taille; k++)
      {
        val += M1[i + k] * M2[j + k * taille];
      }
      R[i + j] = val;
    }
}
