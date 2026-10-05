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
