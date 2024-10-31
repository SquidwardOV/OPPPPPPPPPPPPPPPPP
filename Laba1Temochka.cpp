///ПОСЛЕДОВАТЕЛЬНАЯ ВЕРСИЯ

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>

void createAndFillMatrix(std::vector<std::vector<int>>& matrix, 
int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = rand() % 100;  // Заполнение случайным числом от 0 до 99
        }
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));  // Инициализация генератора случайных чисел
    int N;

    std::cout << "Enter the matrix size N: ";
    std::cin >> N;

    // Создание матрицы N x N
    std::vector<std::vector<int>> matrix(N, std::vector<int>(N));

    // Измерение времени выполнения
    auto start = std::chrono::high_resolution_clock::now();  // Начало измерения времени

    createAndFillMatrix(matrix, N);  // Заполнение матрицы

    auto end = std::chrono::high_resolution_clock::now();  // Конец измерения времени
    std::chrono::duration<double> elapsed = end - start;  // Расчёт времени выполнения

    std::cout << "Execution time: " << elapsed.count() << " секунд." << std::endl;

    // Вывод матрицы
    std::cout << "Generated matrix:" << std::endl;
    for (const auto& row : matrix) {
        for (const auto& elem : row) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}




///ПАРАЛЛЕЛЬНАЯ ВЕРСИЯ


#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <omp.h>
#include <algorithm>
#include <numeric>

// Функция для параллельного создания и заполнения матрицы случайными числами
void fillMatrix(std::vector<std::vector<int>>& matrix, 
int M, int N, int num_threads) {
#pragma omp parallel num_threads(num_threads)
    {
        int thread_id = omp_get_thread_num();
        int total_threads = omp_get_num_threads();

        // Распределение работы между потоками
        for (int i = thread_id; i < M; i += total_threads) {
            for (int j = 0; j < N; ++j) {
                matrix[i][j] = rand() % 100;
            }
        }
    }
}

// Функция для измерения времени выполнения
double measureExecutionTime(int M, int N, 
int num_trials, int num_threads) {
    std::vector<std::vector<int>> matrix(M, std::vector<int>(N));

    std::vector<double> times(num_trials);

    // Измерение времени выполнения
    for (int trial = 0; trial < num_trials; ++trial) {
        auto start = std::chrono::high_resolution_clock::now();

        fillMatrix(matrix, M, N, num_threads);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        times[trial] = duration.count();
    }

    // Вычисление минимального, максимального и среднего времени
    double min_time = *std::min_element(times.begin(),
    times.end());
    double max_time = *std::max_element(times.begin(),
    times.end());
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

    std::vector<std::pair<int, int>> matrix_sizes = { {100, 100},
    {500, 500}, {1000, 1000}, {2000, 2000} };
    int num_trials = 5;

    for (const auto& size : matrix_sizes) {
        int M = size.first;
        int N = size.second;
        measureExecutionTime(M, N, num_trials, num_threads);
    }

    return 0;
}
