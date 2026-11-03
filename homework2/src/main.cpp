#include "Polynomial.h"

int main() {
    try {
        Polynomial a, b;
        float x;
        // 每個多項式先輸入項數，再輸入各項的「係數 指數」。
        if (!(std::cin >> a >> b >> x)) {
            std::cerr << "Invalid input\n";
            return 1;
        }
        std::cout << "A = " << a << '\n' << "B = " << b << '\n';
        std::cout << "A + B = " << a.Add(b) << '\n';
        std::cout << "A * B = " << a.Mult(b) << '\n';
        std::cout << "A(" << x << ") = " << a.Eval(x) << '\n';
        std::cout << "B(" << x << ") = " << b.Eval(x) << '\n';
    } catch (const char* message) {
        std::cerr << message << '\n';
        return 1;
    }
}
