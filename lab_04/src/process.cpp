#include <iostream>
#include <chrono>
#include <unistd.h>
#include <sys/wait.h>

double formula1(double x) {
    return x * x - x * x + x * 4 - x * 5 + x + x;
}

double formula2(double x) {
    return x + x;
}

struct StepResult {
    double sum;
    double seconds;
};

StepResult runFormula1(long long n) {
    auto start = std::chrono::high_resolution_clock::now();
    double sum = 0;
    for (long long i = 0; i < n; i++) {
        double x = static_cast<double>(i + 1);
        sum += formula1(x);
    }
    auto end = std::chrono::high_resolution_clock::now();
    return StepResult{sum, std::chrono::duration<double>(end - start).count()};
}

StepResult runFormula2(long long n) {
    auto start = std::chrono::high_resolution_clock::now();
    double sum = 0;
    for (long long i = 0; i < n; i++) {
        double x = static_cast<double>(i + 1);
        sum += formula2(x);
    }
    auto end = std::chrono::high_resolution_clock::now();
    return StepResult{sum, std::chrono::duration<double>(end - start).count()};
}

int main() {
    long long ns[] = {10000, 100000};

    for (long long n : ns) {
        int pipe1[2];
        int pipe2[2];
        pipe(pipe1);
        pipe(pipe2);

        pid_t pid1 = fork();
        if (pid1 == 0) {
            close(pipe1[0]);
            StepResult r = runFormula1(n);
            write(pipe1[1], &r, sizeof(r));
            close(pipe1[1]);
            _exit(0);
        }

        pid_t pid2 = fork();
        if (pid2 == 0) {
            close(pipe2[0]);
            StepResult r = runFormula2(n);
            write(pipe2[1], &r, sizeof(r));
            close(pipe2[1]);
            _exit(0);
        }

        close(pipe1[1]);
        close(pipe2[1]);

        StepResult r1{};
        StepResult r2{};
        read(pipe1[0], &r1, sizeof(r1));
        read(pipe2[0], &r2, sizeof(r2));
        close(pipe1[0]);
        close(pipe2[0]);

        waitpid(pid1, nullptr, 0);
        waitpid(pid2, nullptr, 0);

        auto start3 = std::chrono::high_resolution_clock::now();
        double sum3 = r1.sum + r2.sum - r1.sum;
        auto end3 = std::chrono::high_resolution_clock::now();
        double time3 = std::chrono::duration<double>(end3 - start3).count();

        std::cout << "n = " << n << ":" << std::endl;
        std::cout << "  step1 (formula1, process): " << r1.seconds << " s, sum1 = " << r1.sum << std::endl;
        std::cout << "  step2 (formula2, process): " << r2.seconds << " s, sum2 = " << r2.sum << std::endl;
        std::cout << "  step3 (combine, parent):   " << time3 << " s, sum3 = " << sum3 << std::endl;
    }

    return 0;
}
