#include "Polynomial.h"
#include <sstream>
#include <string>
#include <cmath>

int checks = 0;
int failures = 0;
void check(bool ok, const char* name) {
    ++checks;
    if (!ok) { ++failures; std::cout << "FAIL: " << name << '\n'; }
}
Polynomial read(const std::string& text) {
    std::istringstream in(text);
    Polynomial p;
    in >> p;
    return p;
}
std::string show(const Polynomial& p) {
    std::ostringstream out;
    out << p;
    return out.str();
}
int main() {
    Polynomial zero, a = read("3 2 2 -3 1 1 0"), b = read("2 1 1 -1 0");
    check(show(zero) == "0", "default zero");
    check(show(a) == "2x^2 - 3x + 1", "format");
    check(show(a.Add(b)) == "2x^2 - 2x", "addition");
    check(show(a.Mult(b)) == "2x^3 - 5x^2 + 4x - 1", "multiplication");
    check(a.Eval(2) == 3, "evaluation");
    check(a.Eval(0) == 1, "constant at zero");
    check(a.Eval(-1) == 6, "negative x");
    check(show(a.Mult(zero)) == "0", "multiply zero");
    check(show(a.Add(zero)) == show(a), "add zero");
    check(show(read("4 1 0 2 3 -2 3 0 8")) == "1", "unsorted duplicate cancellation");
    check(show(read("2 1 1 -1 1")) == "0", "cancel all");
    check(show(read("2 -1 1 -1 0")) == "-x - 1", "negative one");
    check(show(read("2 0.5 2 0.25 0")) == "0.5x^2 + 0.25", "fraction coefficients");
    Polynomial copy(a); a = zero;
    check(show(copy) == "2x^2 - 3x + 1", "copy independence");
    a = copy; copy = zero;
    check(show(a) == "2x^2 - 3x + 1", "assignment independence");
    a = a;
    check(show(a) == "2x^2 - 3x + 1", "self assignment");
    const char* bad[] = {"-1", "1 2 -1", "2 1 1", "hello", "1 abc 2", "1 1 2147483648"};
    for (const char* text : bad) {
        Polynomial p(a);
        std::istringstream in(text); in >> p;
        check(in.fail() && show(p) == show(a), "bad input keeps original");
    }
    Polynomial large;
    std::ostringstream data; data << 80;
    for (int i = 0; i < 80; ++i) data << " 1 " << i;
    large = read(data.str());
    check(large.Eval(1) == 80, "array growth");
    bool caught = false;
    try { read("1 1 2147483647").Mult(read("1 1 1")); }
    catch (const char*) { caught = true; }
    check(caught, "exponent overflow");
    caught = false;
    try { read("1 3e38 1").Add(read("1 3e38 1")); }
    catch (const char*) { caught = true; }
    check(caught, "coefficient addition overflow");
    caught = false;
    try { read("1 3e38 1").Mult(read("1 2 1")); }
    catch (const char*) { caught = true; }
    check(caught, "coefficient multiplication overflow");
    caught = false;
    try { read("1 3e38 2").Eval(2); }
    catch (const char*) { caught = true; }
    check(caught, "evaluation overflow");
    check(show(read("1 0.00000001 1")) != "0", "keep small coefficient");
    check(show(a.Add(a)) == "4x^2 - 6x + 2", "add itself");
    check(show(read("0").Mult(read("0"))) == "0", "two zero polynomials");
    check(show(read("2 1 2 -1 0").Mult(read("2 1 2 1 0"))) == "x^4 - 1", "middle product cancels");
    Polynomial chained;
    chained = copy = a;
    check(show(chained) == show(a) && show(copy) == show(a), "chained assignment");
    // 用代入值驗證運算，避免只檢查輸出的字串。
    for (int i = -10; i <= 10; ++i) {
        Polynomial p = read("3 2 2 -3 1 1 0"), q = read("2 1 1 -1 0");
        check(p.Add(q).Eval(i) == p.Eval(i) + q.Eval(i), "sum identity");
        check(p.Mult(q).Eval(i) == p.Eval(i) * q.Eval(i), "product identity");
    }
    std::cout << checks << " checks, " << failures << " failures\n";
    return failures ? 1 : 0;
}
