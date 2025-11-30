# Лабораторная работа №4
## Вариант 10
##

## Компиляция, запуск, тесты
Библиотека №1 (реализация 1)
gcc -fPIC -shared impl1.c -o libimpl1.so -lm

Библиотека №2 (реализация 2)
gcc -fPIC -shared impl2.c -o libimpl2.so -lm

Эта программа использует библиотеку на этапе линковки.
gcc static_test.c -L. -limpl1 -lm -o static_test

export LD_LIBRARY_PATH=.

./static_test


Тесты для static_test
Тест 1 — производная cos(x)
Ввод:
1 0 0.001
Вывод:
Result: -0.000500

Тест 2 — НОД двух чисел
Ввод:
2 48 18
Вывод:
Result: 6


Эта программа загружает библиотеки во время выполнения.

gcc dynamic_test.c -ldl -lm -o dynamic_test

./dynamic_test

Тесты для dynamic_test
Тест 4 — начальная библиотека (impl1)
Ввод:
1 0 0.001
Вывод:
Result: -0.000500

Тест 5 — переключение реализации
Ввод:
0
Вывод:
Library switched

Тест 6 — производная после переключения (impl2)
Ввод:
1 0 0.001
Вывод:
Result: 0.000000

Тест 7 — НОД (наивный алгоритм)
Ввод:
2 81 27
Вывод:
Result: 27