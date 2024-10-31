#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <omp.h>
#include <algorithm>
#include <numeric>

void fillMatrix(std::vector<std::vector<int>>& matrix,
    int M, int N, int num_threads) {
#pragma omp parallel for collapse(2) schedule(static) num_threads(num_threads)
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = rand() % 100;
        }
    }
}

double measureExecutionTime(int M, int N,
    int num_trials, int num_threads) {
    std::vector<std::vector<int>> matrix(M, std::vector<int>(N));
    std::vector<double> times(num_trials);

    for (int trial = 0; trial < num_trials; ++trial) {
        auto start = std::chrono::high_resolution_clock::now();
        fillMatrix(matrix, M, N, num_threads);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        times[trial] = duration.count();
    }

    double min_time = *std::min_element(times.begin(), times.end());
    double max_time = *std::max_element(times.begin(), times.end());
    double avg_time = std::accumulate(times.begin(), times.end(), 0.0) / num_trials;

    std::cout << "Matrix size: " << M << "x" << N << "\n";
    std::cout << "Number of threads: " << num_threads << "\n";
    std::cout << "Min time: " << min_time << " seconds\n";
    std::cout << "Max time: " << max_time << " seconds\n";
    std::cout << "Avg time: " << avg_time << " seconds\n\n";

    return avg_time;
}

int main() {
    int num_threads;
    std::cout << "Enter the number of threads: ";
    std::cin >> num_threads;

    std::vector<std::pair<int, int>> matrix_sizes = { {100, 100}, {500, 500}, {1000, 1000}, {2000, 2000} };
    int num_trials = 5;

    for (const auto& size : matrix_sizes) {
        int M = size.first;
        int N = size.second;
        measureExecutionTime(M, N, num_trials, num_threads);
    }

    return 0;
}
