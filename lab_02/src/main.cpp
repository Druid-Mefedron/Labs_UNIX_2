#include <iostream>
#include <chrono>
#include <string>

double calculate(double x) {
    return x * x - x * x + x * 4 - x * 5 + x + x;
}

int main() {
    while (true) {
        std::cout << "Enter number of iterations: ";
        std::string input;
        std::cin >> input;

        long long n;
        try {
            size_t pos;
            n = std::stoll(input, &pos);
            if (pos != input.size()) throw std::invalid_argument("not a number");
        } catch (...) {
            std::cout << "Input is not a number, exiting." << std::endl;
            break;
        }

        auto start = std::chrono::high_resolution_clock::now();
        double sum = 0;
        for (long long i = 0; i < n; i++) {
            double x = static_cast<double>(i + 1);
            sum += calculate(x);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        std::cout << "Sum: " << sum << ", iterations: " << n
                   << ", time: " << elapsed.count() << " ms" << std::endl;

        std::cout << "Repeat? (y/n): ";
        std::string again;
        std::cin >> again;
        if (again != "y" && again != "Y") break;
    }
    return 0;
}
