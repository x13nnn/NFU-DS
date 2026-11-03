# 41241127

作業二

## 解題說明

這次作業是要完成 Polynomial 類別，讓程式可以輸入多項式，做加法、乘法，還有代入數字算出結果。另外也要寫出 >> 和 <<，用來輸入及輸出。

### 解題策略

1. 先用 Term 存一項資料，coef 是係數，exp 是指數。例如 3x^2 就存係數 3、指數 2。這樣每一項的兩個數字就不會搞混。
2. 再用題目指定的 termArray 存全部的項。terms 是已經存了幾項，capacity 是陣列可以放幾項，因為陣列可能還沒放滿，所以這兩個數字要分開。
3. 輸入時先讀項數，再讀每項的係數和指數。例如輸入「2 3 2 1 0」就是 3x^2 + 1。先存到暫時的多項式，全部讀完才更新，這樣資料少打一項時，原本的資料才不會被改掉。
4. 每次放入一項，都按照指數由大到小找位置。如果指數一樣，就把係數加起來。比如 2x 和 3x 要合成 5x，不用分開存。
5. 如果加起來是 0，就把那一項移除。因為像 2x - 2x 最後就是 0，不需要再存一個係數是 0 的項。
6. 陣列放滿時，另外開兩倍大的陣列，把原本的項複製過去，再刪掉舊陣列。這樣輸入超過一開始的容量也可以繼續存。
7. 加法先複製第一個多項式，再把第二個的項一個一個放進去。因為前面已經寫好合併同次方的處理，所以這邊可以直接用。例如 (x + 1) + (x - 1) 會得到 2x。
8. 乘法用兩層迴圈，讓兩邊每一項都乘到。係數相乘，指數相加，再放進結果。例如 (x + 1)(x - 1) 會得到 x^2 - 1。
9. 代入時，就把每項的係數乘上 x 的指定次方，最後全部加起來。例如 2x^2 - 3x + 1 代入 2，結果是 8 - 6 + 1 = 3。
10. 最後處理輸出的正負號。一次項不用印 ^1，係數是 1 的 x 項不用印 1，沒有任何項時就印 0，這樣比較容易看。
11. 因為用了動態陣列，複製物件時也要另外開一份陣列。不然兩個物件共用同一個位址，改其中一個就可能影響另一個，刪除時也可能出錯。

## 程式實作

全部程式放在 src/main.cpp，編譯這一個檔案就可以了。只使用規範內的 iostream 和 cmath，沒有用 vector 或其他規範外的標準標頭。

```cpp
#include <iostream>
#include <cmath>

class Polynomial;
class Term {
    friend class Polynomial;
private:
    float coef;
    int exp;
};

class Polynomial {
private:
    Term* termArray;
    int capacity;
    int terms;
    void Insert(float coef, int exp);
    void Print(std::ostream& out) const;
public:
    Polynomial();
    Polynomial(const Polynomial& other);
    Polynomial& operator=(const Polynomial& other);
    ~Polynomial();
    Polynomial Add(Polynomial poly);
    Polynomial Mult(Polynomial poly);
    float Eval(float f);
    friend std::istream& operator>>(std::istream& in, Polynomial& poly);
    friend std::ostream& operator<<(std::ostream& out, const Polynomial& poly);
};

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
```

## 效能分析

這邊把第一個多項式的項數記成 m，第二個記成 n。k 是乘法過程中，結果陣列最多同時存了幾項，最多是 mn 項。

1. 加法時間是 O(m + n(m+n))。因為要先複製第一個多項式，再插入第二個的每一項，而插入時還要找位置和搬資料。額外空間是 O(m+n)。
2. 乘法時間是 O(mn(k+1))，最壞是 O((mn)^2)。雖然有兩層迴圈，但每次乘完還有插入的工作，所以不能只算 O(mn)。額外空間是 O(n+k)，包含傳入參數的複製。
3. 代入會走過 m 項。如果把固定範圍數字的 pow 運算算成固定成本，時間是 O(m)，額外空間是 O(1)。
4. 輸入 n 項最壞是 O(n^2)，因為插入可能搬動前面的資料。暫存多項式需要 O(n) 空間。輸出只走過所有項，時間是 O(m)，額外空間是 O(1)。

## 測試與驗證

### 測試案例

用另外的測試程式實際檢查過 74 項，結果是 74 checks, 0 failures。測試程式只在開發時使用，繳交的 src 裡面只留下 main.cpp。

| 測試內容 | 結果 |
| --- | --- |
| 2x^2 - 3x + 1 加上 x - 1 | 2x^2 - 2x |
| 2x^2 - 3x + 1 乘上 x - 1 | 2x^3 - 5x^2 + 4x - 1 |
| 2x^2 - 3x + 1 代入 2、0、-1 | 3、1、6 |
| 零多項式相加和相乘 | 答案正確，零會印成 0 |
| 輸入順序不同、指數重複 | 會排序，也會合併 |
| 同次方抵消 | 會移除係數是 0 的項 |
| 係數是 -1 或小數 | 符號和數值正確 |
| 輸入 80 個不同次方的項 | 陣列擴充後，代入 1 得到 80 |
| 複製後修改原物件 | 複製品不受影響 |
| 自我指定、連續指定 | 資料正確 |
| 負項數、負指數、文字、資料不足 | 輸入失敗，原物件不變 |
| 指數或係數超出範圍 | 拒絕輸入或回報錯誤 |
| x 從 -10 到 10 | 加法及乘法的代入結果符合運算等式 |

### 編譯與執行指令

使用 GCC 16.2.0，在專案根目錄執行：

```shell
$ g++ -std=c++17 -Wall -Wextra -pedantic homework2/src/main.cpp -o homework2.exe
$ echo "3 2 2 -3 1 1 0 2 1 1 -1 0 2" | .\homework2.exe
A = 2x^2 - 3x + 1
B = x - 1
A + B = 2x^2 - 2x
A * B = 2x^3 - 5x^2 + 4x - 1
A(2) = 3
B(2) = 1
```

第一個 3 是 A 的項數，後面三組是 A 的係數和指數。接下來的 2 是 B 的項數，再後面兩組是 B 的資料，最後的 2 是要代入的 x。

### 結論

測試下來，加法、乘法和代入的結果都有符合答案。除了正常的輸入，也有測一些容易漏掉的情況，像是同次方抵消和複製物件。這幾個地方如果只看一般的範例，其實不太容易看出有沒有問題。

### 效能量測

用 measure.ps1 測量，兩個多項式都是 n 項，係數都是 1，指數從 0 到 n-1，代入 1。主程式用 -O2 編譯，每組跑 5 次取平均。

| 每個多項式的項數 | 平均時間，毫秒 |
| --- | --- |
| 10 | 65.667 |
| 100 | 31.190 |
| 1000 | 291.371 |

這是合併成單一檔案後重新量到的時間。時間包含啟動、輸入、運算和輸出，所以小筆數的差異不一定是運算本身造成的，不能只用這個表格判斷演算法的快慢。

```shell
$ g++ -std=c++17 -O2 homework2/src/main.cpp -o homework2.exe
$ .\homework2\measure.ps1 -Program .\homework2.exe
```

## 申論及開發報告

### 選擇這個資料結構的原因

這題依照投影片用 Term 陣列來存多項式，只存係數不為 0 的項。如果一個多項式只有 x^1000 + 1，也只要存兩項，不需要把中間的次方全部存下來。

加法和乘法都有用到 Insert，主要是因為兩邊都需要合併同次方，所以放在同一個函式比較好修改。不過每次插入可能要搬資料，項數多的時候還是會變慢。

### 開發時遇到的問題

第一次編譯時，輸出函式讀不到 Term 的私有資料。後來才確認，Term 只把 Polynomial 類別設為朋友，並不是 Polynomial 的朋友函式也能直接讀。所以改成讓類別裡面的 Print 處理輸出，<< 再去呼叫它。

另外有測複製後再修改資料，確認兩個物件的陣列是分開的。浮點數也有精度限制，所以這邊只有係數真的等於 0 才刪除，不會把很小的係數直接當成 0。
