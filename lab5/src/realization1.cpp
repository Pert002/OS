#include "realizations.h"
#include <string>
#include <algorithm>
#include <cstring>

float Pi(int K) {
    float pi = 0;
    float n = 1;
    for (int i = 0; i < K; i++) {
        if (i % 2 == 0) pi += 4/n;
        else pi -= 4/n;
        n += 2;
    }
    return pi;
}

char* translation(long x) {
    std::string s;
    if(x==0) s += "0";
    while (x > 0) {
        if (x % 2 == 0) {
            x /= 2;
            s += "0";
        }
        else {
            x /= 2;
            s += "1";
        }
    }
    std::reverse(s.begin(), s.end());
    char* ans = (char*) malloc((s.size() + 1) * sizeof(char));
    strcpy(ans, s.c_str());//копирует string в char*
    return ans;
}