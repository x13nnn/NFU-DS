# 41241127

作業二

## 解題說明

要完成 Polynomial 類別，使用題目指定的 Term 動態陣列儲存多項式，提供 Add、Mult、Eval，並且重載輸入和輸出運算子。兩個問題使用同一個類別，所以放在同一份程式裡。

### 解題策略

1. 每一項記錄係數 coef 和指數 exp。例如 $2x^2-3x+1$ 存成 `(2,2)`、`(-3,1)`、`(1,0)`。只存非零項，才不會為了很大的指數開一大段沒用到的陣列。
2. 陣列照指數由大到小排列。新增時先找位置，遇到相同指數就把係數相加。這樣輸入順序不同也不影響結果。
3. 如果合併後係數是 0，就移除那一項。例如 $x-x$ 最後要印成 0，不能留下沒有作用的項。
4. 陣列滿了就開兩倍大的空間，把舊資料搬過去，再釋放舊陣列。capacity 是空間大小，terms 是目前項數，這兩個數字不能混用。
5. 加法先複製第一個多項式，再把第二個的每一項插入。相同指數會在插入時合併。例如 $(2x^2-3x+1)+(x-1)=2x^2-2x$。
6. 乘法用兩層迴圈，讓第一個多項式的每一項乘上第二個的每一項。係數相乘、指數相加，再插入結果。例如 $(x-1)(x+1)=x^2-1$，中間的兩個一次項會抵消。
7. Eval 逐項計算係數乘上 x 的指定次方，再加總。常數項的指數是 0，所以 x 是 0 時，常數也要保留下來。
8. 輸入先讀項數，再讀每項的「係數 指數」。先存到暫時物件，全部成功才更新原物件，避免讀到一半失敗時留下半份資料。
9. 輸出要處理正負號、係數 1、一次項和常數。比如負的一次項印成 `-x`，零多項式印成 `0`。
10. 類別自己管理動態陣列，所以要寫複製建構子、指定運算子和解構子。複製時另外開陣列，才不會改了一個物件，另一個也跟著變，或兩個物件釋放同一段記憶體。

## 程式實作

程式放在 `src/Polynomial.h`、`src/Polynomial.cpp` 和 `src/main.cpp`。`src/tests.cpp` 是另外執行的測試程式，不和 main.cpp 一起編譯。

下方附上完整程式碼。

### Polynomial.h

```cpp
#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H
#include <iostream>

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
#endif
```

### Polynomial.cpp

```cpp
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
```

### main.cpp

```cpp
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
```

### tests.cpp

```cpp
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
```

## 效能分析

設兩個多項式分別有 m、n 個非零項，乘法結果最多有 k 個非零項，且 $k\le mn$。

| 操作 | 時間複雜度 | 額外空間 |
| --- | --- | --- |
| 插入一項，目前有 t 項 | O(t) | 擴充時 O(t)，沒有擴充時 O(1) |
| Add | O(m+n(m+n)) | O(m+n) |
| Mult | O(mn(k+1))，最壞 O((mn)^2) | O(n+k) |
| Eval | O(m log(E+1))，以整數次方用平方乘法估算 | O(1) |
| 輸入 n 項 | O(n^2) | O(n) |
| 輸出 m 項 | O(m)，不計數字轉字串長度 | O(1) |

E 是最大指數。實作使用標準函式 pow，其實際成本取決於函式庫，上表 Eval 的時間是用常見整數次方算法估算，不是保證 pow 的內部實作。固定寬度指數的運算也常簡化記為 O(m)。

乘法雖然只有兩層迴圈，但每次插入還要找位置、搬動陣列，所以不能直接寫成 O(mn)。函式參數照題目的介面使用傳值，會多複製第二個多項式，空間分析也把它算進去。

## 測試與驗證

### 測試案例

這邊除了測一般的加法和乘法，也另外測了一些比較容易出錯的情況。使用 GCC 16.2.0 編譯後，74 項檢查全部通過，結果是 `74 checks, 0 failures`。

| 情況 | 預期結果或檢查目的 |
| --- | --- |
| 預設建構 | 輸出 0 |
| 一般加法 | 2x^2 - 2x |
| 一般乘法 | 2x^3 - 5x^2 + 4x - 1 |
| x = 2、0、-1 | 分別得到 3、1、6 |
| 與零多項式相加或相乘 | 原多項式或 0 |
| 亂序輸入、重複指數 | 排序並合併 |
| 係數為零、全部抵消 | 不留下零項 |
| 係數為 -1、小數 | 正確處理符號及數值 |
| 複製後修改原物件 | 複製品不變 |
| 指定後修改來源、自我指定 | 資料不會損壞 |
| 負項數、負指數 | 輸入失敗 |
| 缺資料、文字、超出 int 的指數 | 輸入失敗，保留原物件 |
| 80 個不同指數 | 多次擴充後 Eval(1) = 80 |
| 指數相加超出 int 範圍 | 拋出錯誤 |
| x 從 -10 到 10 | 驗證 (A+B)(x)=A(x)+B(x)，以及乘法對應等式 |

### 編譯與執行指令

在專案根目錄執行，這邊使用 PowerShell 和 GCC 16.2.0。編譯時有開啟警告檢查，沒有出現警告。先把 g++ 所在的資料夾加入目前視窗的 PATH：

```powershell
$ $env:PATH = 'C:\Users\TUF GAMING A15\.local\compilers\w64devkit\bin;' + $env:PATH
```

```shell
$ g++ -std=c++17 -Wall -Wextra -pedantic homework2/src/Polynomial.cpp homework2/src/main.cpp -o homework2.exe
$ "3 2 2 -3 1 1 0 2 1 1 -1 0 2" | .\homework2.exe
```

實際輸出：

```text
A = 2x^2 - 3x + 1
B = x - 1
A + B = 2x^2 - 2x
A * B = 2x^3 - 5x^2 + 4x - 1
A(2) = 3
B(2) = 1
```

```shell
$ g++ -std=c++17 -Wall -Wextra -pedantic homework2/src/Polynomial.cpp homework2/src/tests.cpp -o homework2-tests.exe
$ .\homework2-tests.exe
```

實際輸出：

```text
74 checks, 0 failures
```

另外用 `-D_GLIBCXX_ASSERTIONS` 重新編譯測試，結果也相同。這不代表所有可能的輸入都沒問題，但表格列出的情況都有實際檢查。

### 結論

這次測試下來，加法、乘法和代入算出來的結果都符合測試的答案，74 項檢查也都有通過。另外有確認輸入錯誤時原本的多項式不會被改掉，還有複製的兩個物件不會互相影響。

### 效能量測

這邊用 `measure.ps1` 測量整支程式的時間，兩個多項式都是 n 項，係數都是 1，指數從 0 到 n-1，代入值也是 1。主程式用 `-O2` 編譯，每組執行 5 次取平均。

| 每個多項式的項數 | 平均時間（毫秒） |
| --- | --- |
| 10 | 34.763 |
| 100 | 30.795 |
| 1000 | 305.741 |

```shell
$ g++ -std=c++17 -O2 homework2/src/Polynomial.cpp homework2/src/main.cpp -o homework2.exe
$ .\homework2\measure.ps1 -Program .\homework2.exe
```

這個時間包含程式啟動、輸入、運算和輸出，輸出由腳本接收後丟棄。所以 10 項和 100 項的差異不能直接拿來判斷算法快慢，短時間的測量會受到啟動和系統狀態影響。1000 項花的時間比較多，也可以看出項數增加後，乘法和插入的工作量會增加。

## 申論及開發報告

### 選擇這個資料結構的原因

這題是按照投影片的方式，用 Term 陣列來存，所以保留了 termArray、capacity、terms 這三個成員。這樣可以只存有用的項，也能練習陣列滿了要怎麼擴充。比較要注意的是指標複製：只把位址複製過去雖然程式很短，但物件結束時就可能重複釋放，所以這裡用各自的陣列。

加法和乘法最後都會用到 Insert，主要是想把相同次方的合併放在同一個地方，這樣比較不會漏掉。不過每次插入都可能搬資料，項數很多時會慢。之後可以把加法改成同時走訪兩個已排序陣列，讓時間降到 O(m+n)。

浮點數有精度限制，所以只在係數確實等於 0 時刪除，沒有隨便用一個門檻把很小的數當成零。即使如此，很多小數連續運算還是可能有誤差，這是這份程式要注意的限制。

### 開發時遇到的問題

第一次實際編譯時，發現輸出函式不能直接讀 Term 裡面的 coef 和 exp。原本以為它是 Polynomial 的朋友函式就可以，但 Term 只把 Polynomial 類別設為朋友，這個權限不會再傳給其他函式。後來改成由 Polynomial 的 Print 處理輸出，`<<` 再呼叫它，才順利編譯。

另外，測試有特別放入同次方抵消、陣列超過原本容量，還有把物件指定給自己這幾種情況。這些平常只輸入一兩組數字不一定會遇到，所以另外寫成測試比較好確認。
