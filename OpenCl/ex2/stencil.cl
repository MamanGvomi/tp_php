__kernel
void stencil(const int taille, __global float * M1, __global float *M2, __global float *R){
    int i=get_global_id(0);
    R[i]=M1[i]+M2[i];
}
