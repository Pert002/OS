#include <iostream>
#include <cstdlib>
#include <dlfcn.h>

int main() {
    void* handle = NULL;//адрес, в будущем нужный нам для получения доступа к библиотеке
    float (*Pi)(int K);//объявление указателей на функции
    char* (*translation)(long x);//объявление указателей на функции
    const char* libs[] = {"libd1.so", "libd2.so"};
    int cur_lib;
    int start_lib;
    std::cout << "Enter start library: \n";
    std::cout << "1 for using first library\n";
    std::cout << "2 for using second library\n";
    std::cin >> start_lib;


    bool flag = true;
    while (flag) {
        if (start_lib == 1) {
            cur_lib = 0;
            flag = false;
        }
        else if (start_lib == 2) {
            cur_lib = 1;
            flag = false;
        }
        else {
            std::cout << "Error\nYou should enter only 1 or 2\n";
            std::cin >> start_lib;
        }
    }


    handle = dlopen(libs[cur_lib], RTLD_LAZY); //rtld lazy выполняется поиск только тех символов, на которые есть ссылки из кода
    if (!handle) {
        std::cout << "Error\nCan not open library\n";
        exit(EXIT_FAILURE);
    }
    Pi = (float(*)(int))dlsym(handle, "Pi");//возвращаем адрес функции из памяти библиотеки
    translation = (char*(*)(long x))dlsym(handle, "translation");//dlsym присваивает указателю на функцию, объявленному в начале, ее адрес в библиотеке
    int command;
    std::cout << "Insert a command\n  0 - change the contract\n  1 - employ Pi function,\n  2 - employ function of conversion to binary notation\n";


    while (scanf("%d", &command) != EOF) {
        switch (command) {
            case 0: {
                dlclose(handle); //освобождает указатель на библиотеку и программа перестает ей пользоваться
                cur_lib = 1 - cur_lib;
                handle = dlopen(libs[cur_lib], RTLD_LAZY);
                if (!handle) {
                    std::cout << "Error\nCan not open library\n";
                    exit(EXIT_FAILURE);
                }
                Pi = (float(*)(int))dlsym(handle, "Pi");
                translation = (char*(*)(long x))dlsym(handle, "translation");
                std::cout << "You have changed contracts!\n";
                break;
            }

            case 1: {
                int K;
                std::cout << "Insert K\n";
                std::cin >> K;
                float pi = Pi(K);
                std::cout << "Result is " << pi << "\n";
                break;
            }

            case 2: {
                long x;
                std::cout << "Insert x that u want to convert to binary or tenary\n";
                std::cin >> x;
                char* result = translation(x);
                std::cout << "Result is " << result << "\n";
                free(result);
                break;
            }

            default: {
                std::cout << "Wrong command\n";
                std::cout << "Insert a command\n 1 - employ Pi function,\n 2 - employ function of conversion to binary notation\n";
            }
        }
    }


    dlclose(handle);
    return 0;
}