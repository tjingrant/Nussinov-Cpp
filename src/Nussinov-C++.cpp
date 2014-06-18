//============================================================================
// Name        : Nussinov-C++.cpp
// Author      : Tian Jin
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <cstdlib>
#include <omp.h>
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

//A-U;G-C

#define NICE_PRINTING
#define IMPERFECT_TILE
#define BLOCK_LEN 40
#define PRINT_STEPS
#undef PRINT_STEPS
#define NEW_GDB
#define SEED 42

#define PRINT_MATRIX \
	for(int i = 0; i < size; i++) { \
		for(int j = 0; j < size; j++) { \
			printf("%3d ", matrix[i][j]); \
		} \
		printf("\n"); \
	}

#define CLEAR_TEMP_MATRIX \
	for(int i = 0; i < BLOCK_LEN; i++) { \
		for(int j = 0; j < BLOCK_LEN; j++) { \
			temp_matrix[i][j] = 0; \
		} \
	}

#define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
        } \
    } while (false)

inline int max(int a, int b) {
	return a > b ? a : b;
}

using namespace std;

int* input;

inline int basePair(int a, int b) {
#ifdef PRINT_STEPS
	cout << "\tDeducing BasePair at (" << a << "," << b << ") = " << (((((input[a] + input[b]) == 3) && (true))) ? 1 : 0) << " " << input[a] << " + " << input[b] <<  endl;
#endif
	return ((((input[a] + input[b]) == 3) && (a < b - 1))) ? 1 : 0; //a < b - 1 warning! actual program not consistent with log
}

int main(int argc, char* argv[]) {

	//ASSERT(argc == 3, "invalid parameter");
	char *base_print[4] = { "A", "C", "G", "U" };
	int size = atoi(argv[1]);
	int strand[size];
	input = strand;
	int** matrix = (int**) malloc(size*sizeof(int*));

	for (int i = 0; i < size; i++)
	{
	   matrix[i] = (int*) malloc(size*sizeof(int));
	}
	const int BLOCK_NUMBER = size / BLOCK_LEN + 1;
	const int BLOCK_NUMBER_ALONG_I = (size % BLOCK_LEN == 0) ? size / BLOCK_LEN : size / BLOCK_LEN + 1;

	//srand (time(NULL));
	srand(SEED);
	for(int i = 0; i < size; i++) {
		  input[i] = rand() % 4;
#ifdef PRINT_STEPS
		  cout << base_print[(int)input[i]] << "\t";
#endif
	}
	cout << endl;
#ifdef NICE_PRINTING
	for(int i = 0; i < size; i++)
		for(int j = 0; j < size; j++) {
			matrix[i][j] = 0;
		}
#endif

	for(int i = 0; i < size - 1; i++)
		matrix[i+1][i] = 0;

	int temp_matrix[BLOCK_LEN][BLOCK_LEN];
	int temp_max = 0;
#ifdef NEW_GDB
	double start = omp_get_wtime();
#endif
	for(int iB = (BLOCK_NUMBER_ALONG_I - 1) * BLOCK_LEN; iB >= 0; iB -= BLOCK_LEN) {
		for(int jB = iB - 1; jB < (BLOCK_NUMBER) * BLOCK_LEN - 1; jB += BLOCK_LEN) {
#ifdef PRINT_STEPS
			cout << "Block iB,jB=" << iB << "," << max(iB, jB) << endl;
#endif
			CLEAR_TEMP_MATRIX;
			//Starting from here, it is going into tile levels
			for(int kB = iB + BLOCK_LEN - 1; kB + BLOCK_LEN - 1 < jB; kB += BLOCK_LEN) {
#ifdef PRINT_STEPS
				cout << "\tkBlock (" << iB << "," << kB << ") , (" << kB + 1 << "," << jB << ")" <<endl;
#endif
				for(int i = BLOCK_LEN - 1; i >= 0; i--) {
					for(int j = 0; j < BLOCK_LEN; j++) {
						if ((max(iB, jB) + j) < size) {
#ifdef PRINT_STEPS
							cout << "\t\tTargeting (" << iB + i << "," << max(iB, jB) + j << ")" << endl;
#endif
							temp_max = 0;
							for(int k = 0; k < BLOCK_LEN; k++) {
#ifdef PRINT_STEPS
								cout << "\t\t\tCurrentCell: (" << iB + i << "," << kB + k << "), (" << kB + k + 1 << "," << jB + j << ") = " << matrix[iB + i][kB + k] + matrix[kB + k + 1][jB + j] << endl;
#endif
								temp_max = max(temp_max, matrix[iB + i][kB + k] + matrix[kB + k + 1][jB + j]);
							}
							temp_matrix[i][j] = max(temp_matrix[i][j], temp_max);
						}
					}
				}
			}
			//    For the cells inside the tile I'm taking care of:
			//    It moves from left to right, down to up
			//    As it computes cells within each tiles, trivial loop peels are
			//automatically taken care of
			for(int i = BLOCK_LEN - 1; i >= 0; i--) {
				for(int j = 0; j < BLOCK_LEN; j++) {
					if ((iB + i < jB + j) && (iB + i < size) && (jB + j < size)) { //upper triangle && within the square
						int leftdown_i = iB + BLOCK_LEN - 1;
						int leftdown_j = jB;
#ifdef PRINT_STEPS
						cout << "\tTrivial:(" << iB + i << "," << jB + j << ")" << " /w LD Point:(" << leftdown_i << "," << leftdown_j << ")" << endl;
#endif
						if (iB + i < leftdown_i) {
							for (int k = leftdown_i - 1; k >= iB + i; k--) { //bug fixes
								if ((k + 1) < size){
									temp_matrix[i][j] = max(temp_matrix[i][j], (matrix[iB + i][k] + matrix[k + 1][jB + j]));
#ifdef PRINT_STEPS
									cout << "\t\tAdding:(" << iB + i << "," << k << ") , (" << k + 1 << "," << jB + j << ") along i = " << (matrix[iB + i][k] + matrix[k + 1][jB + j]) << endl;
#endif
								}
							}
						}
						if (jB + j > leftdown_j) {
							for (int k = leftdown_j; k < jB + j; k++) {
								temp_matrix[i][j] = max(temp_matrix[i][j], (matrix[iB + i][k] + matrix[k + 1][jB + j]));
#ifdef PRINT_STEPS
								cout << "\t\tAdding:(" << iB + i << "," << k << ") , (" << k + 1 << "," << jB + j << ") along j = " << (matrix[iB + i][k] + matrix[k + 1][jB + j]) << endl;
#endif
							}
						}
						temp_matrix[i][j] = max(temp_matrix[i][j], matrix[iB + i + 1][jB + j - 1] + basePair(iB + i, jB + j));
						matrix[iB + i][jB + j] = temp_matrix[i][j];

					} else if ((iB + i < size) && (jB + j < size) && ((iB + i) >= 0) && ((jB + j) >= 0)) {
						matrix[iB + i][jB + j] = 0;
#ifdef PRINT_STEPS
						cout << "\tSetting hard 0 @ (" << iB + i << "," << jB + j << ")" << endl;
#endif
					}
				}
			}
		}
	}
	cout << "result :" << matrix[0][size - 1] << endl;
#ifdef NEW_GDB
	double end = omp_get_wtime();
	cout << "Execution took " << (end - start) << "s" << endl;
#endif
	//PRINT_MATRIX;
#ifdef PRINT_STEPS

#endif
	return 0;
}


