/**
 * Copy values from an array to another.
 */
#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h>
#include <time.h>


void print_tabf(float * tab, int size){
	printf("Mon tableau : \n");
	for (size_t i = 0; i < size; i++)
	{
		printf("[0] : %f\n", tab[i]);
	}
	
}

void print_tabd(double * tab, int size){
	printf("Mon tableau : \n");
	for (size_t i = 0; i < size; i++)
	{
		printf("[0] : %f\n", tab[i]);
	}
	
}


int maxvalue(int random_size)
{

	srand(time(NULL));
	float *array0 = _mm_malloc(random_size * sizeof(float), 16);

	for (size_t i = 0; i < random_size; i++)
	{
		float value = (float)(rand() % 20);
		printf("elem %ld : %f \n", i, value);
		array0[i] = value;
	}

	__m128 r0;
	__m128 r_max = _mm_set1_ps(__FLT_MIN__);

	int block_count = random_size / 4;
	int reste = random_size % 4;

	for (size_t i = 0; i < block_count; i++)
	{
		r0 = _mm_load_ps(array0 + i * 4);
		r_max = _mm_max_ps(r_max, r0);
	}


	float max = array0[block_count * 4];
	float curr_val;

	for (size_t i = 1; i < reste; i++)
	{
		curr_val = array0[block_count*4+i];
		max = (max < curr_val) ? curr_val : max;
	}

	float max_value[4] __attribute__((aligned(16)));
	_mm_store_ps(max_value, r_max);

	for (size_t i = 0; i < 4; i++)
	{
		curr_val = max_value[i];
		{
			if (max < curr_val)
			{
				max = curr_val;
			}
		}
	}

	printf("\nLe float max : %f\n", max);
	return max;
}

int copy(int random_size)
{
	// Static arrays are stored into the stack thus we need to add an alignment attribute to tell the compiler to correctly align both arrays.

	// float array0[ 4 ] __attribute__((aligned(16))) = { 0.0f, 1.0f, 2.0f, 3.0f };
	// srand(time(NULL));
	// int random_size = rand()%16;
	float *array0 = _mm_malloc(random_size * sizeof(float), 16);
	float *array1 = _mm_malloc(random_size * sizeof(float), 16);
	for (size_t i = 0; i < random_size; i++)
	{
		array0[i] = i + 0.0f;
	}

	int block_count = random_size / 4;
	int reste = random_size % 4;
	__m128 r0;
	for (size_t i = 0; i < block_count; i++)
	{
		// Load 4 values from the first array into a SSE register.
		r0 = _mm_load_ps(array0 + i * 4);
		// Store the content of the register into the second array.
		_mm_store_ps(array1 + i * 4, r0);
	}
	for (size_t i = 0; i < reste; i++)
	{
		array1[block_count * 4 + i] = array0[block_count * 4 + i];
	}

	return array1[0];
}

float reduce_from_tab(int random_size)
{
	srand(time(NULL));
	float *array0 = _mm_malloc(random_size * sizeof(float), 16);

	for (size_t i = 0; i < random_size; i++)
	{
		float value = (float)(rand() % 20);
		printf("elem %ld : %f \n", i, value);
		array0[i] = value;
	}
	
	int block_count = random_size / 4;
	int reste = random_size % 4;

	__m128 r0;
	__m128 r_total = _mm_set1_ps(0);



	for (size_t i = 0; i < block_count; i++)
	{
		r0 = _mm_load_ps(array0 + i * 4);
		r_total = _mm_add_ps(r0, r_total);
	}
	
	float res = 0.0f;
	for (size_t i = 0; i < reste; i++)
	{
		res += array0[block_count*4+i];	
	}


	float total[4] __attribute__((aligned(16)));
	_mm_store_ps(total, r_total);

	for (size_t i = 0; i < 4; i++)
	{
		res += total[i];
	}

	return res;
}

double produit_scalaire(int random_size)
{
	double *array0 = _mm_malloc(random_size * sizeof(double), 16);
	double *array1 = _mm_malloc(random_size * sizeof(double), 16);
	for (size_t i = 0; i < random_size; i++)
	{
		array0[i] = (double)(rand() % 20);
		array1[i] = (double)(rand() % 20);
	}

	int block_count = random_size / 2;
	int reste = random_size % 2;


	__m128d r0, r1, r_res;
	r_res = _mm_set1_pd(0.0);

	for (size_t i = 0; i < block_count; i++)
	{
		r0 = _mm_load_pd(array0 + i * 2);
		r1 = _mm_load_pd(array1 + i * 2);
		r_res = _mm_add_pd(r_res, _mm_mul_pd(r0, r1));
	}

	double res = 0.0;
	for (size_t i = 0; i < reste; i++)
	{
		res += array0[block_count*2+i] * array1[block_count*2+i];
	}
	
	double total[2] __attribute__((aligned(16)));
	_mm_store_pd(total, r_res);

	for (size_t i = 0; i < 4; i++)
	{
		res += total[i];
	}
	print_tabd(array0, random_size);
	print_tabd(array1, random_size);

	return res;
}


int main(void)
{
	// copy(1000000000);
	// maxvalue(10);
	// printf("\nMAX : %f", reduce_from_tab(6));
	printf("\nProduict scalaire : %f\n", produit_scalaire(2));
	return 0;
}
