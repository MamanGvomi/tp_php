__kernel
void mult_mat(const int taille, __global int * M1, __global int *M2, __global int *R){
    int i=get_global_id(0);
    int j=get_global_id(1);
    int index = i * taille + j;
    int val = 0;
    for(int k = 0; k < taille; k ++){
        val += M1[i * taille + k] * M2[k * taille + j];
    }
    R[index] = val;

}
