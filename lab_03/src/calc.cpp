#include <iostream>
#include <cstdlib>
#include <cstring>

double add(double a, double b) {
    return a + b;
}

double subtract(double a, double b) {
    return a - b;
}

double power(double base, int exponent) {
    double result = 1;
    for (int i = 0; i < exponent; i++) {
        result = result * base;
    }
    return result;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: calc <plus|minus|power> <operand1> <operand2>" << std::endl;
        return 1;
    }

    std::string op = argv[1];
    double a = std::atof(argv[2]);
    double b = std::atof(argv[3]);

    if (op == "plus") {
        std::cout << a << " + " << b << " = " << add(a, b) << std::endl;
    } else if (op == "minus") {
        std::cout << a << " - " << b << " = " << subtract(a, b) << std::endl;
    } else if (op == "power") {
        int exp = static_cast<int>(b);
        if (exp < 0) {
            std::cout << "Negative exponent is not supported (only +, -, * are allowed)" << std::endl;
            return 1;
        }
        std::cout << a << " ^ " << exp << " = " << power(a, exp) << std::endl;
    } else {
        std::cout << "Unknown operator: " << op << ". Use plus, minus or power." << std::endl;
        return 1;
    }

    return 0;
}
