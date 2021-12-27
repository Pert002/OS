#include <iostream>
#include "realizations.h"

int main(){
    int command;
    std::cout << "Insert a command\n  1 - employ Pi function,\n  2 - employ function of conversion to binary notation\n";
    while(scanf("%d", &command) != EOF) {
        switch (command) {
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
                std::cout << "Insert x that u want to convert to binary\n";
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
    return 0;
}