#include <iostream>
#include <string>
#include <stack>
#include <cctype>
#include <stdexcept>
#include <cmath>
#include <locale.h>

int priority(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

double apply(double a, double b, char op) {
    switch (op) {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/':
        if (b == 0) throw std::runtime_error("Деление на ноль");
        return a / b;
    }
    throw std::runtime_error("Неизвестная операция");
}

double evaluate(const std::string& expr) {
    std::stack<double> nums;
    std::stack<char> ops;

    auto applyTop = [&]() {
        if (nums.size() < 2 || ops.empty())
            throw std::runtime_error("Некорректное выражение");
        double b = nums.top(); nums.pop();
        double a = nums.top(); nums.pop();
        char op = ops.top(); ops.pop();
        nums.push(apply(a, b, op));
        };

    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];

        if (std::isspace(c)) continue;

        if (std::isdigit(c) || c == '.') {
            std::string num;
            while (i < expr.size() &&
                (std::isdigit(expr[i]) || expr[i] == '.')) {
                num += expr[i++];
            }
            --i;
            nums.push(std::stod(num));
        }
        else if (c == '(') {
            ops.push(c);
        }
        else if (c == ')') {
            while (!ops.empty() && ops.top() != '(') applyTop();
            if (ops.empty()) throw std::runtime_error("Лишняя )");
            ops.pop();
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/') {
            bool unary = (c == '-' || c == '+') &&
                (i == 0 || expr[i - 1] == '(' ||
                    priority(expr[i - 1]) > 0);
            if (unary) {
                size_t j = i + 1;
                while (j < expr.size() && std::isspace(expr[j])) ++j;
                if (j >= expr.size() ||
                    !(std::isdigit(expr[j]) || expr[j] == '('))
                    throw std::runtime_error("Ожидалось число после унарного знака");

                if (expr[j] == '(') {
                    // -(...) => 0 - (...)
                    nums.push(0.0);
                    ops.push(c);
                    continue;
                }
                std::string num;
                while (j < expr.size() &&
                    (std::isdigit(expr[j]) || expr[j] == '.')) {
                    num += expr[j++];
                }
                double v = std::stod(num);
                nums.push(c == '-' ? -v : v);
                i = j - 1;
            }
            else {
                while (!ops.empty() && ops.top() != '(' &&
                    priority(ops.top()) >= priority(c)) {
                    applyTop();
                }
                ops.push(c);
            }
        }
        else {
            throw std::runtime_error(std::string("Непонятный символ: ") + c);
        }
    }

    while (!ops.empty()) {
        if (ops.top() == '(') throw std::runtime_error("Лишняя (");
        applyTop();
    }

    if (nums.size() != 1) throw std::runtime_error("Некорректное выражение");
    return nums.top();
}

int main() {
    setlocale(LC_ALL,"RUS");
    std::cout << "Калькулятор. Пример: (2 + 3) * 4 - 1.5\n";
    std::cout << "Введите 'q' для выхода.\n\n";

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        if (line == "q" || line == "quit") break;
        if (line.empty()) continue;

        try {
            double result = evaluate(line);
            std::cout << "= " << result << "\n";
        }
        catch (const std::exception& e) {
            std::cout << "Ошибка: " << e.what() << "\n";
        }
    }
    return 0;
}