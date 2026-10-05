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
        cerr << "輸入必須是非負的 m 和 n。\n";
        return 1;
    }

    cout << "Ackermann (recursive): "
         << ackermannRecursive(m, n) << '\n';
    cout << "Ackermann (nonrecursive): "
         << ackermannIterative(m, n) << '\n';
    return 0;
}
