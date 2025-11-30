#include <math.h>
#include "include/contracts.h"

float cos_derivative(float a, float dx) {
    return (cos(a + dx) - cos(a - dx)) / (2 * dx);
}

int gcd(int a, int b) {
    int res = 1;
    int min = (a < b) ? a : b;
    for (int i = 1; i <= min; i++) {
        if (a % i == 0 && b % i == 0)
            res = i;
    }
    return res;
}
