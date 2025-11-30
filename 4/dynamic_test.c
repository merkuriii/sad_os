#include <stdio.h>
#include <dlfcn.h>

typedef float (*cos_deriv_t)(float, float);
typedef int (*gcd_t)(int, int);

int main() {
    void *handle = dlopen("./libimpl1.so", RTLD_LAZY);
    if (!handle) {
        printf("dlopen error\n");
        return 1;
    }

    cos_deriv_t cos_derivative = dlsym(handle, "cos_derivative");
    gcd_t gcd = dlsym(handle, "gcd");

    int cmd;
    while (scanf("%d", &cmd) == 1) {
        if (cmd == 0) {
            dlclose(handle);
            static int toggle = 0;
            toggle = !toggle;
            handle = dlopen(toggle ? "./libimpl2.so" : "./libimpl1.so", RTLD_LAZY);
            cos_derivative = dlsym(handle, "cos_derivative");
            gcd = dlsym(handle, "gcd");
            printf("Library switched\n");
        } else if (cmd == 1) {
            float a, dx;
            scanf("%f %f", &a, &dx);
            printf("Result: %f\n", cos_derivative(a, dx));
        } else if (cmd == 2) {
            int a, b;
            scanf("%d %d", &a, &b);
            printf("Result: %d\n", gcd(a, b));
        }
    }

    dlclose(handle);
    return 0;
}
