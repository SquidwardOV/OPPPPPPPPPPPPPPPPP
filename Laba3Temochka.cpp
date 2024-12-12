#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <limits>
#include <cstdlib>

struct Point3D {
    double x, y, z;
};

double calculate_distance(const Point3D& point, const Point3D& camera) {
    return std::sqrt((point.x - camera.x) * (point.x - camera.x) +
        (point.y - camera.y) * (point.y - camera.y) +
        (point.z - camera.z) * (point.z - camera.z));
}

int main() {
    const int num_points = 1000000; 
    Point3D camera = { 0.0, 0.0, 0.0 };
    std::vector<Point3D> points(num_points);
    std::vector<double> distances(num_points);

    // Выбор количества потоков
    int num_threads;
    std::cout << "Enter the number of threads to use: ";
    std::cin >> num_threads;
    omp_set_num_threads(num_threads);

    // Инициализация случайных точек
#pragma omp parallel for
    for (int i = 0; i < num_points; ++i) {
        points[i] = { static_cast<double>(rand()) / RAND_MAX * 100.0,
                     static_cast<double>(rand()) / RAND_MAX * 100.0,
                     static_cast<double>(rand()) / RAND_MAX * 100.0 };
    }

    double min_time = std::numeric_limits<double>::max();
    double max_time = 0.0;
    double total_time = 0.0;
    const int num_runs = 10;

    for (int run = 0; run < num_runs; ++run) {
        double start_time = omp_get_wtime();

        // Параллельный расчет расстояний
#pragma omp parallel
        {
#pragma omp for
            for (int i = 0; i < num_points; ++i) {
                double distance = calculate_distance(points[i], camera);

                
#pragma omp critical
                distances[i] = distance;
            }
        }

        double end_time = omp_get_wtime();
        double elapsed_time = end_time - start_time;

        min_time = std::min(min_time, elapsed_time);
        max_time = std::max(max_time, elapsed_time);
        total_time += elapsed_time;

        std::cout << "Run " << run + 1 << " completed in " << elapsed_time << " seconds.\n";
    }

    double avg_time = total_time / num_runs;

    std::cout << "\nAnalysis:\n";
    std::cout << "Number of threads used: " << num_threads << "\n";
    std::cout << "Minimum execution time: " << min_time << " seconds\n";
    std::cout << "Maximum execution time: " << max_time << " seconds\n";
    std::cout << "Average execution time: " << avg_time << " seconds\n";

    return 0;
}
