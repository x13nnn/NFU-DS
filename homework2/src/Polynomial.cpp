#include "Polynomial.h"
#include <cmath>

Polynomial::Polynomial() {
    capacity = 2;
    terms = 0;
    termArray = new Term[capacity];
}

// 每個物件各自有一份陣列。
Polynomial::Polynomial(const Polynomial& other) {
    capacity = other.capacity;
    terms = other.terms;
    termArray = new Term[capacity];
    for (int i = 0; i < terms; ++i) {
        termArray[i] = other.termArray[i];
    }
}

Polynomial& Polynomial::operator=(const Polynomial& other) {
    if (this == &other) return *this;
    Term* copy = new Term[other.capacity];
    for (int i = 0; i < other.terms; ++i) copy[i] = other.termArray[i];
    delete[] termArray;
    termArray = copy;
    capacity = other.capacity;
    terms = other.terms;
    return *this;
}

Polynomial::~Polynomial() {
    delete[] termArray;
}

// 由大到小排列，同次方直接相加，係數變成 0 就移除。
void Polynomial::Insert(float coef, int exp) {
    if (!std::isfinite(coef) || exp < 0) throw "Invalid term";
    if (coef == 0) return;
    int pos = 0;
    while (pos < terms && termArray[pos].exp > exp) ++pos;
    if (pos < terms && termArray[pos].exp == exp) {
        float sum = termArray[pos].coef + coef;
        if (!std::isfinite(sum)) throw "Coefficient overflow";
        termArray[pos].coef = sum;
        if (sum == 0) {
            for (int i = pos; i + 1 < terms; ++i) termArray[i] = termArray[i + 1];
            --terms;
        }
        return;
    }
    if (terms == capacity) {
        if (capacity > 100000000) throw "Too many terms";
        Term* larger = new Term[capacity * 2];
        for (int i = 0; i < terms; ++i) larger[i] = termArray[i];
        delete[] termArray;
        termArray = larger;
        capacity *= 2;
    }
    for (int i = terms; i > pos; --i) termArray[i] = termArray[i - 1];
    termArray[pos].coef = coef;
    termArray[pos].exp = exp;
    ++terms;
}

Polynomial Polynomial::Add(Polynomial poly) {
    Polynomial result(*this);
    for (int i = 0; i < poly.terms; ++i)
        result.Insert(poly.termArray[i].coef, poly.termArray[i].exp);
    return result;
}

Polynomial Polynomial::Mult(Polynomial poly) {
    Polynomial result;
    for (int i = 0; i < terms; ++i) {
        for (int j = 0; j < poly.terms; ++j) {
            // 先轉成較大的整數再加，避免 int 溢位。
            long long exp = static_cast<long long>(termArray[i].exp) + poly.termArray[j].exp;
            if (exp > 2147483647LL) throw "Exponent overflow";
            result.Insert(termArray[i].coef * poly.termArray[j].coef, static_cast<int>(exp));
        }
    }
    return result;
}

float Polynomial::Eval(float f) {
    if (!std::isfinite(f)) throw "Invalid x";
    double result = 0;
    for (int i = 0; i < terms; ++i)
        result += termArray[i].coef * std::pow(static_cast<double>(f), termArray[i].exp);
    if (!std::isfinite(result) || std::fabs(result) > 3.402823466e38) throw "Evaluation overflow";
    return static_cast<float>(result);
}

std::istream& operator>>(std::istream& in, Polynomial& poly) {
    int count;
    if (!(in >> count)) return in;
    if (count < 0) { in.setstate(std::ios::failbit); return in; }
    Polynomial temp;
    for (int i = 0; i < count; ++i) {
        float coef;
        int exp;
        if (!(in >> coef >> exp)) return in;
        if (!std::isfinite(coef) || exp < 0) {
            in.setstate(std::ios::failbit);
            return in;
        }
        temp.Insert(coef, exp);
    }
    // 全部讀完才更新，輸入失敗不會破壞原本的多項式。
    poly = temp;
    return in;
}

void Polynomial::Print(std::ostream& out) const {
    if (terms == 0) {
        out << 0;
        return;
    }
    for (int i = 0; i < terms; ++i) {
        float coef = termArray[i].coef;
        int exp = termArray[i].exp;
        if (i > 0) {
            if (coef < 0) out << " - ";
            else out << " + ";
        } else if (coef < 0) {
            out << '-';
        }
        float magnitude = std::fabs(coef);
        if (exp == 0 || magnitude != 1) out << magnitude;
        if (exp > 0) out << 'x';
        if (exp > 1) out << '^' << exp;
    }
}

std::ostream& operator<<(std::ostream& out, const Polynomial& poly) {
    poly.Print(out);
    return out;
}
