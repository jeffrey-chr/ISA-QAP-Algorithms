#include <algorithm>
#include <vector>
#include <random>

typedef int_fast64_t reallng;
typedef reallng* ptr_reallng;

void scrambleMatrix(reallng** input, int n, int buffer) {
	
	std::vector<int> pvec;
	
	for (int i = 0; i < n; i++) pvec.push_back(i);
	
	std::random_device rd;
	std::mt19937 g(rd());

	std::shuffle(pvec.begin(), pvec.end(), g);
	
	reallng* output = new reallng[n*n];

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			output[i + j * n] = input[pvec[i]][pvec[j]];
		}
	}
	
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			input[i][j] = output[i + j * n];
		}
	}
}