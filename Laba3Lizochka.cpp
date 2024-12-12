#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <random>
#include <limits>
#include <algorithm>

// Более точная константа π
const double M_PI = 3.14159265358979323846;

// Структура для хранения 3D точки
struct Point3D {
    double x, y, z;
};

// Функция для генерации случайных точек
std::vector<Point3D> generate_random_points(size_t num_points, double min_coord, double max_coord) {
    std::vector<Point3D> points(num_points);

    // Инициализация генератора случайных чисел
    std::random_device rd;                // Источник энтропии
    std::mt19937 gen(rd());               // Генератор Мерсенна
    std::uniform_real_distribution<> dis(min_coord, max_coord);

    for (size_t i = 0; i < num_points; ++i) {
        points[i].x = dis(gen);
        points[i].y = dis(gen);
        points[i].z = dis(gen);
    }

    return points;
}

// Функция для поворота точек вокруг осей X, Y, Z
void rotate(std::vector<Point3D>& points, double angleX, double angleY, double angleZ) {
    size_t n = points.size();

    // Преобразование углов в радианы
    double radiansX = angleX * M_PI / 180.0;
    double radiansY = angleY * M_PI / 180.0;
    double radiansZ = angleZ * M_PI / 180.0;

    // Предварительный расчет косинусов и синусов
    double cosX = std::cos(radiansX), sinX = std::sin(radiansX);
    double cosY = std::cos(radiansY), sinY = std::sin(radiansY);
    double cosZ = std::cos(radiansZ), sinZ = std::sin(radiansZ);

#pragma omp parallel
    {
        // Поворот вокруг оси X
#pragma omp for
        for (size_t i = 0; i < n; ++i) {
            double y_new = points[i].y * cosX - points[i].z * sinX;
            double z_new = points[i].y * sinX + points[i].z * cosX;
            points[i].y = y_new;
            points[i].z = z_new;
        }

#pragma omp barrier // Синхронизация потоков

        // Поворот вокруг оси Y
#pragma omp for
        for (size_t i = 0; i < n; ++i) {
            double x_new = points[i].x * cosY + points[i].z * sinY;
            double z_new = -points[i].x * sinY + points[i].z * cosY;
            points[i].x = x_new;
            points[i].z = z_new;
        }

#pragma omp barrier // Синхронизация потоков

        // Поворот вокруг оси Z
#pragma omp for
        for (size_t i = 0; i < n; ++i) {
            double x_new = points[i].x * cosZ - points[i].y * sinZ;
            double y_new = points[i].x * sinZ + points[i].y * cosZ;
            points[i].x = x_new;
            points[i].y = y_new;
        }

#pragma omp barrier // Синхронизация потоков
    }
}

int main() {
    // Установка локали для корректного отображения сообщений на русском
    setlocale(LC_ALL, "Russian");

    // Ввод количества потоков от пользователя
    int num_threads;
    std::cout << "Введите количество потоков: ";
    while (!(std::cin >> num_threads) || num_threads <= 0) {
        std::cout << "Некорректный ввод. Пожалуйста, введите положительное целое число: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Установка количества потоков
    omp_set_num_threads(num_threads);
    std::cout << "Используется потоков: " << num_threads << std::endl;

    // Ввод количества точек
    size_t num_points;
    std::cout << "Введите количество точек: ";
    while (!(std::cin >> num_points) || num_points == 0) {
        std::cout << "Некорректный ввод. Пожалуйста, введите положительное целое число: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Задание диапазона координат (можно изменить при необходимости)
    double min_coord = -100.0;
    double max_coord = 100.0;

    // Генерация случайных точек
    std::vector<Point3D> original_points = generate_random_points(num_points, min_coord, max_coord);
    std::cout << "Сгенерировано " << num_points << " случайных точек в диапазоне ["
        << min_coord << ", " << max_coord << "].\n";

    // Задание углов поворота в градусах (фиксированные значения)
    double angleX = 30.0; // Поворот вокруг оси X
    double angleY = 45.0; // Поворот вокруг оси Y
    double angleZ = 60.0; // Поворот вокруг оси Z

    // Задание количества итераций для измерения времени
    int iterations;
    std::cout << "Введите количество итераций: " << std::endl;
    std::cin >> iterations;
    
    std::vector<double> times;
    times.reserve(iterations);

    std::cout << "Выполняется " << iterations << " итераций поворота...\n";

    for (int i = 0; i < iterations; ++i) {
        // Создание копии оригинальных точек для каждой итерации
        std::vector<Point3D> points = original_points;

        // Засекаем время начала
        double start_time = omp_get_wtime();

        // Выполнение поворота
        rotate(points, angleX, angleY, angleZ);

        // Засекаем время окончания
        double end_time = omp_get_wtime();
        double elapsed_time = end_time - start_time;

        times.push_back(elapsed_time);

        std::cout << "Итерация " << i + 1 << ": " << elapsed_time << " секунд\n";
    }

    // Вычисление минимального, максимального и среднего времени
    double min_time = *std::min_element(times.begin(), times.end());
    double max_time = *std::max_element(times.begin(), times.end());
    double sum_time = 0.0;
    for (const auto& t : times) {
        sum_time += t;
    }
    double avg_time = sum_time / iterations;

    // Вывод результатов
    std::cout << "\nСтатистика времени выполнения поворота:\n";
    std::cout << "Минимальное время: " << min_time << " секунд\n";
    std::cout << "Максимальное время: " << max_time << " секунд\n";
    std::cout << "Среднее время: " << avg_time << " секунд\n";

    return 0;
}
