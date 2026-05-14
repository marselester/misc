#include <stdio.h>

// Calculates (a + b) - (c + d) + 7.
extern int AddSubI32_a(int a, int b, int c, int d);

int main() {
    int r = AddSubI32_a(1, 2, 3, 4);
    printf("C says %d\n", r);
    return 0;
}
