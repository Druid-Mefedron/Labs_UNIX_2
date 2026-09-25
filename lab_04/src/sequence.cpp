#include <iostream>
#include <chrono>

double formula1(double x) {
    return x * x - x * x + x * 4 - x * 5 + x + x;
}

double formula2(double x) {
    return x + x;
}

int main() {
    long long ns[] = {10000, 100000};

    for (long long n : ns) {
        auto start = std::chrono::high_resolution_clock::now();

        double sum1 = 0;
        for (long long i = 0; i < n; i++) {
            double x = static_cast<double>(i + 1);
            sum1 += formula1(x);
        }

        double sum2 = 0;
        for (long long i = 0; i < n; i++) {
            double x = static_cast<double>(i + 1);
            sum2 += formula2(x);
        }

        double sum3 = sum1 + sum2 - sum1;

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;

        std::cout << "n = " << n
                   << ": sum1=" << sum1
                   << ", sum2=" << sum2
                   << ", sum3=" << sum3
                   << ", total time = " << elapsed.count() << " s"
                   << std::endl;
    }

    return 0;
}
