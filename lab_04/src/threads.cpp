#include <iostream>
#include <chrono>
#include <thread>

double formula1(double x) {
    return x * x - x * x + x * 4 - x * 5 + x + x;
}

double formula2(double x) {
    return x + x;
}

void computeStep1(long long n, double& result, double& seconds) {
    auto start = std::chrono::high_resolution_clock::now();
    double sum = 0;
    for (long long i = 0; i < n; i++) {
        double x = static_cast<double>(i + 1);
        sum += formula1(x);
    }
    auto end = std::chrono::high_resolution_clock::now();
    result = sum;
    seconds = std::chrono::duration<double>(end - start).count();
}

void computeStep2(long long n, double& result, double& seconds) {
    auto start = std::chrono::high_resolution_clock::now();
    double sum = 0;
    for (long long i = 0; i < n; i++) {
        double x = static_cast<double>(i + 1);
        sum += formula2(x);
    }
    auto end = std::chrono::high_resolution_clock::now();
    result = sum;
    seconds = std::chrono::duration<double>(end - start).count();
}

int main() {
    long long ns[] = {10000, 100000};

    for (long long n : ns) {
        double sum1 = 0, sum2 = 0;
        double time1 = 0, time2 = 0;

        std::thread t1(computeStep1, n, std::ref(sum1), std::ref(time1));
        std::thread t2(computeStep2, n, std::ref(sum2), std::ref(time2));

        t1.join();
        t2.join();

        auto start3 = std::chrono::high_resolution_clock::now();
        double sum3 = sum1 + sum2 - sum1;
        auto end3 = std::chrono::high_resolution_clock::now();
        double time3 = std::chrono::duration<double>(end3 - start3).count();

        std::cout << "n = " << n << ":" << std::endl;
        std::cout << "  step1 (formula1, thread): " << time1 << " s, sum1 = " << sum1 << std::endl;
        std::cout << "  step2 (formula2, thread): " << time2 << " s, sum2 = " << sum2 << std::endl;
        std::cout << "  step3 (combine, main):    " << time3 << " s, sum3 = " << sum3 << std::endl;
    }

    return 0;
}
