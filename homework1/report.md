# 41241127

作業一

## 解題說明

### Problem 1：Ackermann 函數

題目給定 Ackermann 函數：

$$
A(m,n)=
\begin{cases}
n+1, & \text{if }m=0 \\
A(m-1,1), & \text{if }n=0 \\
A(m-1,A(m,n-1)), & \text{otherwise}
\end{cases}
$$

題目要求寫出一個遞迴函式，另外再寫一個不使用遞迴的演算法。

### Problem 2：Power Set

如果集合 $S$ 有 $n$ 個元素，Power Set 就是 $S$ 所有可能的子集合。例如 `{1, 2, 3}` 的 Power Set 有 8 個結果，包含空集合。

### 解題策略

1. Ackermann 的遞迴版本直接按照題目的三個情況撰寫。非遞迴版本使用陣列當作 stack，模擬遞迴時電腦保存函式工作的方式。
2. Power Set 每次處理一個元素，分成「選這個元素」和「不選這個元素」兩條路。當所有元素都處理完，就印出目前的子集合。

## 程式實作

### Problem 1：`problem1.cpp`

```cpp
#include <iostream>

using namespace std;

const int MAX_STACK_SIZE = 10000;

// 遞迴版本：直接按照題目給的三個規則計算
long long ackermannRecursive(int m, long long n) {
    if (m < 0 || n < 0)
        return -1;

    if (m == 0)
        return n + 1;

    if (n == 0)
        return ackermannRecursive(m - 1, 1);

    return ackermannRecursive(m - 1,
                              ackermannRecursive(m, n - 1));
}

// 非遞迴版本：用陣列模擬遞迴時使用的 stack
long long ackermannIterative(int m, long long n) {
    if (m < 0 || n < 0)
        return -1;

    int stack[MAX_STACK_SIZE];
    int top = 0;
    stack[top++] = m;

    while (top > 0) {
        int currentM = stack[--top];

        if (currentM == 0) {
            ++n;
        } else if (n == 0) {
            n = 1;
            if (top >= MAX_STACK_SIZE)
                return -1;
            stack[top++] = currentM - 1;
        } else {
            if (top + 2 > MAX_STACK_SIZE)
                return -1;

            --n;
            stack[top++] = currentM - 1;
            stack[top++] = currentM;
        }
    }

    return n;
}

int main() {
    int m;
    long long n;

    if (!(cin >> m >> n) || m < 0 || n < 0) {
        cerr << "輸入要負的 m 和 n。\n";
        return 1;
    }

    cout << "Ackermann (recursive): "
         << ackermannRecursive(m, n) << '\n';
    cout << "Ackermann (nonrecursive): "
         << ackermannIterative(m, n) << '\n';
    return 0;
}
```

### Problem 2：`problem2.cpp`

```cpp
#include <iostream>

using namespace std;

// index 表示目前處理到第幾個元素
// selected 裡面放目前已經選到的元素
void powerSetRecursive(const int elements[], int size, int index,
                       int selected[], int selectedSize) {
    if (index == size) {
        cout << "{";
        for (int i = 0; i < selectedSize; ++i) {
            if (i > 0)
                cout << ", ";
            cout << selected[i];
        }
        cout << "}\n";
        return;
    }

    // 不選目前的元素
    powerSetRecursive(elements, size, index + 1, selected, selectedSize);

    // 選目前的元素
    selected[selectedSize] = elements[index];
    powerSetRecursive(elements, size, index + 1, selected,
                      selectedSize + 1);
}

int main() {
    int size;

    if (!(cin >> size) || size < 0) {
        cerr << "集合大小必須是非負數。\n";
        return 1;
    }

    int* elements = new int[size];
    for (int i = 0; i < size; ++i)
        cin >> elements[i];

    if (!cin) {
        cerr << "集合元素數量不足。\n";
        delete[] elements;
        return 1;
    }

    int* selected = new int[size];
    powerSetRecursive(elements, size, 0, selected, 0);

    delete[] selected;
    delete[] elements;
    return 0;
}
```

## 效能分析

1. **Problem 1：Ackermann 函數**

   - 時間複雜度：Ackermann 會產生很多次函式呼叫。令 $T(m,n)$ 表示總共呼叫幾次，時間複雜度就是 $O(T(m,n))$。遞迴版和非遞迴版做的工作相同，所以兩者的時間複雜度相同。
   - 空間複雜度：遞迴版要保存函式呼叫的層數，非遞迴版要使用陣列 stack。兩者都可以寫成 $O(D(m,n))$，其中 $D(m,n)$ 表示最多同時保存幾層工作。

2. **Problem 2：Power Set**

   - 時間複雜度：每個元素都有選和不選兩種情況，所以會有 $2^n$ 個子集合。每個子集合最多要印出 $n$ 個元素，因此是 $O(n2^n)$。
   - 空間複雜度：程式只保留目前正在組合的子集合，最多保存 $n$ 個元素，因此是 $O(n)$。

## 測試與驗證

### 測試案例

| 測試案例 | 程式 | 輸入 | 預期輸出 |
|----------|------|------|----------|
| 測試一 | Problem 1 | `0 0` | 遞迴和非遞迴結果都是 `1` |
| 測試二 | Problem 1 | `1 2` | 遞迴和非遞迴結果都是 `4` |
| 測試三 | Problem 1 | `3 4` | 遞迴和非遞迴結果都是 `125` |
| 測試四 | Problem 2 | `0` | 輸出 `{}` |
| 測試五 | Problem 2 | `3 1 2 3` | 輸出 8 個子集合 |

### 編譯與執行指令

Problem 1：

```shell
$ g++ homework1/src/problem1.cpp --std=c++17 -o problem1.exe
$ echo "3 4" | .\problem1.exe
Ackermann (recursive): 125
Ackermann (nonrecursive): 125
```

Problem 2：

```shell
$ g++ homework1/src/problem2.cpp --std=c++17 -o problem2.exe
$ echo "3`n1 2 3" | .\problem2.exe
{}
{3}
{2}
{2, 3}
{1}
{1, 3}
{1, 2}
{1, 2, 3}
```

### 結論

Problem 1 的遞迴和非遞迴程式在測試資料 `3 4` 都得到 `125`。Problem 2 輸入 3 個元素後，成功列出 $2^3=8$ 個子集合，結果符合題目要求。

## 申論及開發報告

### 選擇遞迴的原因

Problem 1 的 Ackermann 函數本來就是用遞迴方式定義的，所以遞迴版可以直接按照題目公式寫。非遞迴版使用陣列 stack，主要是練習不用函式自己呼叫自己時，如何保存還沒有做完的工作。

Problem 2 使用遞迴是因為每個元素只有兩種選擇：放進子集合或是不放進去。每次分成兩條路，最後就可以把所有可能的子集合列出來。

這次使用的標頭只有 `<iostream>`，符合上學期規範。Ackermann 因為成長很快，只用小數字測試；Power Set 則用 3 個元素確認輸出數量為 8。
