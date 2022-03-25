/**
 * Copy values from an array to another.
 */
#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h> 
#include <time.h>

int maxvalue(int random_size){

  srand(time(NULL));
  float * array0  = _mm_malloc(random_size*sizeof(float), 16);
  float * array1 = _mm_malloc(random_size*sizeof(float), 16);
  for (size_t i = 0; i < random_size; i++)
  {
    array0[i] = (float)(rand() % 20); 
    array1[i] = (float)(rand() % 20); 
    
  }
  __m128 r0, r1;


  int block_count = random_size/4; 
  int reste = random_size % 4; 

  for (size_t i = 0; i < block_count; i++)
  {
    r0 = _mm_load_ps( array0 + i*4);
    r1 = _mm_load_ps( array1 + i*4);  
  }

  
  

  __m128 r1 = _mm_max_ps(r0, r0);
  float max_value[4] __attribute__((aligned(16)));  
  _mm_store_ps( max_value, r1 );
  printf("Print de l'array max_value\n");
  for(int i = 0 ; i < random_size ; ++i){
    printf("%f ", max_value[i]);
  }
  printf("\n");
}

int copy(int random_size){
   // Static arrays are stored into the stack thus we need to add an alignment attribute to tell the compiler to correctly align both arrays.

  // float array0[ 4 ] __attribute__((aligned(16))) = { 0.0f, 1.0f, 2.0f, 3.0f };
  // srand(time(NULL));
  // int random_size = rand()%16;
  float * array0  = _mm_malloc(random_size*sizeof(float), 16);
  float * array1 = _mm_malloc(random_size*sizeof(float), 16);
  for (size_t i = 0; i < random_size; i++)
  {
    array0[i] = i + 0.0f; 
  }
  

  int block_count = random_size/4; 
  int reste = random_size % 4; 
  __m128 r0;
  for (size_t i = 0; i < block_count; i++)
  {
      // Load 4 values from the first array into a SSE register.
      r0 = _mm_load_ps( array0 + i*4);
      // Store the content of the register into the second array.
      _mm_store_ps( array1  + i*4, r0 );
  }
  for (size_t i = 0; i < reste; i++)
  {
    array1[block_count*4+i] = array0[block_count*4+i];
  }
  
  return array1[0];

  // printf("Print de l'array1 après copie \n");
  // for(int i = 0 ; i < random_size ; ++i){
  //   printf("%f ", array1[i]);
  // }
  // printf("\n");


  // __m128 r1 = _mm_max_ps(r0, r0);
  // float max_value[4] __attribute__((aligned(16)));  
  // _mm_store_ps( max_value, r1 );
  // printf("Print de l'array max_value\n");
  // for(int i = 0 ; i < random_size ; ++i){
  //   printf("%f ", max_value[i]);
  // }
  // printf("\n");
  
}

int main(void)
{
  copy(1000000000);
  maxvalue(10);
  return 0;
}
