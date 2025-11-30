#include <math.h>
#include "include/contracts.h"

float cos_derivative(float a, float dx) {
    return (cos(a + dx) - cos(a)) / dx;
}

int gcd(int a, int b) {
    while (b != 0) {
        int t = b;
        b = a % b;
        a = t;
    }
    return a;
}
