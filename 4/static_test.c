#include <stdio.h>
#include "include/contracts.h"

int main() {
    int cmd;
    while (scanf("%d", &cmd) == 1) {
        if (cmd == 1) {
            float a, dx;
            scanf("%f %f", &a, &dx);
            printf("Result: %f\n", cos_derivative(a, dx));
        } else if (cmd == 2) {
            int a, b;
            scanf("%d %d", &a, &b);
            printf("Result: %d\n", gcd(a, b));
        }
    }
    return 0;
}
