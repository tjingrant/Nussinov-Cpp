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

#define NICE_PRINTING true
#define  BLOCK_LEN 4
#define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
        } \
    } while (false)


#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

	ASSERT(argc == 3, "invalid parameter");
	int size = atoi(argv[2]);
	int* matrix[size][size];
	const int BLOCK_NUMBER = ((size % BLOCK_LEN) == 0) ? size / BLOCK_LEN : size / BLOCK_LEN + 1;

#ifdef NICE_PRINTING
	for(int i = 0; i < size; i++)
		for(int j = 0; j < size; j++) {
			matrix[i][j] = 0;
		}
#else

#endif

	cout << "initializing" << endl;

	double start = omp_get_wtime();
	for(int i = 0; i < size - 1; i++)
		matrix[i+1][i] = 0;
	double end = omp_get_wtime();

	cout << "initialized. initialization took " << (end-start) << " second(s)" << endl;

	start = omp_get_wtime();
	for(int iB = BLOCK_NUMBER - 1; iB >= 0; iB--) {
		for(int jB = iB; jB < BLOCK_NUMBER; jB++) {
			for(int b = 0; b < BLOCK_LEN; b++) {

			}
			for(int b = 0; b < BLOCK_LEN; b++) {

			}
		}
	}
	end = omp_get_wtime();
	cout << (end - start) << "s" << endl;
	for(int i = 0; i < size; i++) {
			for(int j = 0; j < size; j++) {
				cout << matrix[i][j] << "\t";
			}
			cout << endl;
	}

	return 0;
}


