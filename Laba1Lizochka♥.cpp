//ПОСЛЕДОВАТЕЛЬНАЯ ВЕРСИЯ

#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <numeric>
#include <algorithm>

// Функция для транспонирования матрицы (последовательная версия)
void transposeMatrix(const std::vector<std::vector<int>>& mat,
std::vector<std::vector<int>>& transposed, int M, int N) {
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            transposed[j][i] = mat[i][j];
        }
    }
}

// Функция для измерения времени выполнения транспонирования
double measureExecutionTime(int M, int N, int num_trials) {
    std::vector<std::vector<int>> mat(M, std::vector<int>(N));
    std::vector<std::vector<int>> transposed(N, 
    std::vector<int>(M));

    // Инициализация матрицы случайными значениями
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            mat[i][j] = rand() % 100;
        }
    }

    std::vector<double> times(num_trials);

    // Измерение времени выполнения
    for (int trial = 0; trial < num_trials; ++trial) {
        auto start = std::chrono::high_resolution_clock::now();
        
        transposeMatrix(mat, transposed, M, N);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        times[trial] = duration.count();
    }

    // Вычисление минимального, максимального и среднего времени
    double min_time = *std::min_element(times.begin(), times.end());
    double max_time = *std::max_element(times.begin(), times.end());
    double avg_time = std::accumulate(times.begin(), times.end(),
    0.0) / num_trials;

    std::cout << "Matrix size: " << M << "x" << N << "\n";
    std::cout << "Min time: " << min_time << " seconds\n";
    std::cout << "Max time: " << max_time << " seconds\n";
    std::cout << "Avg time: " << avg_time << " seconds\n\n";

    return avg_time;
}

int main() {
    std::vector<std::pair<int, int>> matrix_sizes = {{100, 100},
    {500, 500}, {1000, 1000}, {2000, 2000}};
    int num_trials = 5;

    for (const auto& size : matrix_sizes) {
        int M = size.first;
        int N = size.second;
        measureExecutionTime(M, N, num_trials);
    }

    return 0;
}



///ПАРАЛЛЕЛЬНАЯ ВЕРСИЯ

#include <iostream>
#include <vector>
#include <omp.h>
#include <algorithm>
#include <cstdlib>
#include <chrono>
#include <numeric>

void transposeMatrix(const std::vector<std::vector<int>>& mat,
std::vector<std::vector<int>>& transposed, int M, int N, int num_threads) {
    #pragma omp parallel num_threads(num_threads)
    {
        int thread_id = omp_get_thread_num();
        int total_threads = omp_get_num_threads();

        // Каждый поток обрабатывает свой диапазон строк матрицы
        for (int i = thread_id; i < M; i += total_threads) {
            for (int j = 0; j < N; ++j) {
                transposed[j][i] = mat[i][j];
            }
        }
    }
}

double measureExecutionTime(int M, int N, int num_trials, 
int num_threads) {
    std::vector<std::vector<int>> mat(M, std::vector<int>(N));
    std::vector<std::vector<int>> transposed(N, 
    std::vector<int>(M));

    // Инициализация матрицы случайными значениями
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            mat[i][j] = rand() % 100;
        }
    }

    std::vector<double> times(num_trials);

    // Измерение времени выполнения
    for (int trial = 0; trial < num_trials; ++trial) {
        auto start = std::chrono::high_resolution_clock::now();
        
        transposeMatrix(mat, transposed, M, N, num_threads);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        times[trial] = duration.count();
    }

    // Вычисление минимального, максимального и среднего времени
    double min_time = *std::min_element(times.begin(), times.end());
    double max_time = *std::max_element(times.begin(), times.end());
    double avg_time = std::accumulate(times.begin(), times.end(),
    0.0) / num_trials;

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

    std::vector<std::pair<int, int>> matrix_sizes = {{100, 100},
    {500, 500}, {1000, 1000}, {2000, 2000}};
    int num_trials = 5;

    for (const auto& size : matrix_sizes) {
        int M = size.first;
        int N = size.second;
        measureExecutionTime(M, N, num_trials, num_threads);
    }

    return 0;
}
