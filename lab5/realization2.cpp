#include "realizations.h"
#include <string>
#include <algorithm>
#include <cstring>
#include <cmath>

float Pi(int K){
    float pi = 1;
    for (int i = 1; i <= K; i++){
        pi *= (4 * pow(i, 2)) / (4 * pow(i, 2) - 1);
    }
    return pi * 2;
}

char* translation(long x) {
    std::string s;
    if(x==0) s += "0";
    while (x > 0) {
        if (x % 3 == 0) {
            x /= 3;
            s += "0";
        }
        else if(x % 3 == 1) {
            x /= 3;
            s += "1";
        }
        else {
            s += "2";
        }
    }
    std::reverse(s.begin(), s.end());
    char* ans = (char*) malloc((s.size() + 1) * sizeof(char));
    strcpy(ans, s.c_str());//копирует string в char*
    return ans;
}