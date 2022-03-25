/**
 * Copy values from an array to another.
 */
#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h> 
#include <time.h>

int copy(int random_size){
  // Static arrays are stored into the stack thus we need to add an alignment attribute to tell the compiler to correctly align both arrays.
  // float array0[ 4 ] __attribute__((aligned(16))) = { 0.0f, 1.0f, 2.0f, 3.0f };
  // srand(time(NULL));
  // int random_size = rand()%16;
  float * array0  = malloc(random_size*sizeof(float));
  float * array1 = malloc(random_size*sizeof(float));
  for (size_t i = 0; i < random_size; i++)
  {
    array0[i] = i + 0.0f; 
  }
  
  for (size_t i = 0; i < random_size; i++)
  {
      array1[i] = array0[i];
  }
  
  
  return array1[0];

}
float maxvalue(int random_size){

    // init du tableau
    srand(time(NULL));
    float * tab = malloc(random_size*sizeof(float));
    for (size_t i = 0; i < random_size; i++)
    {
        tab[i] = (float) (rand()%50);
        printf(" %f", tab[i]);
    }

    // Calcule du max
    float max = tab[0];
    for (size_t i = 1; i < random_size-1; i++)
    {
        if (max < tab[i])
        {
            max = tab[i];
        }
        
    }
    return max;
    
}
int main(void)
{
//   copy(1000000000);
  printf("\n%f", maxvalue(10));
  return 0;
}
