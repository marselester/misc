#include <iostream>

// Calculates (a + b) - (c + d) + 7.
extern "C" int AddSubI32_a(int a, int b, int c, int d);

int main() {
    int r = AddSubI32_a(1, 2, 3, 4);
    std::cout << "C++ says " << r << std::endl;
    return 0;
}
