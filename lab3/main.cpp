#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <unistd.h>

using namespace std;
 
 
void matrix_thread(int numberOfStr, int n, int m, int convolutionSize, double *convolution, double *matrix, double* results) {
 
    double matrix2[n][m];
    double convolution2[convolutionSize][convolutionSize];
    double mresults[n - 2];
    int q = 0;
 
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            matrix2[i][j] = matrix[q++];
        }
    }
 
    q = 0;
    for (int i = 0; i < convolutionSize; i++) {
        for (int j = 0; j < convolutionSize; j++) {
            convolution2[i][j] = convolution[q++];
        }
    }
 
    for(int i = 0; i < m - convolutionSize + 1; i++) {
        double res = 0;
        int a = 0, b = 0;
        for(int j = numberOfStr; j < numberOfStr + convolutionSize; j++) {
            for(int k = i; k < i + convolutionSize; k++) {
                res += matrix2[j][k] * convolution2[a][b];
                b++;
            }
            b = 0;
            a++;
        }
        mresults[i] = res;
    }
 
    for (int i = 0; i < n - 2; i++) {
        results[numberOfStr * (n - convolutionSize + 1) + i] = mresults[i];
    }
}
 
bool notCenter(int j, int n, int m) {                                                      
    return (j >= 0 && j < m) || (j % m == 0) || ((j + 1) % m == 0) || (j < n * m && j > n * m - m);
}
 
 
int main(int argc, char* argv[]) {
 
    if (argc != 2) {
        cout << "Не достаточное кол-во аргументов" << endl                                                                                   ;
        return 1;
    } 
    int K;
    int threads_amount = atoi(argv[1]);
 
    cout << "K:\n";
    cin >> K;
 
    if(!threads_amount || (threads_amount < 0 && threads_amount != -1))  {
        cout << "Incorrect amount of threads" << endl;
        return 1;
    }
 
    double *convolution = new double[9];
    cout << "Convolution matrix elements: " << endl;
    for(int i = 0; i < 9; i++) {
        cin >> convolution[i];
    }                                                                                           
 
    int n, m;
    cout << "Matrix size:\n";
    cin >> n >> m;
    double *matrix = new double[n*m];
    cout << "Matrix elements: " << endl;
    for(int i = 0; i < n*m; i++) {
        cin >> matrix[i];
    } 
 
    thread convolutionThreads[n - 2];
 
    double *convolutionResult = new double[(n - 2) * (m - 2)];
    for(int i = 0; i < (n - 2) * (m - 2); i++) {
        convolutionResult[i] = 0;
    }
 
    while(K > 0) {
 
        if(threads_amount == -1 || threads_amount >= n - 2) {
            for(int i = 0; i < n - 2; i++) {
                convolutionThreads[i] = thread(matrix_thread, i, n, m, 3, convolution, matrix, convolutionResult);
            }
            for(int i = 0; i < n - 2; i++) {
                convolutionThreads[i].join();
            }
        } 
        else  {
            for(int i = 0; i < n - 2; i++) {
                if(i >= threads_amount && i != 0) {
                    convolutionThreads[i - threads_amount].join();
                }
                convolutionThreads[i] = thread(matrix_thread, i, n, m, 3, convolution, matrix, convolutionResult);
            }
            for(int i = n - 3 - threads_amount + 1; i < n - 2; i++) {
                convolutionThreads[i].join();
            }
        }
 
 
 
        int q = 0;
        for (int j = 0; j < n * m; j++) {
            if (notCenter(j, n, m)) { 
                matrix[j] = 0;
            } else  {
                matrix[j] = convolutionResult[q];
                q++;
            }
        }
        K--;
    }
 
 
    cout << "Convolution result: " << endl;
    for (int i = 0; i < (n * m); i++) {
        if (!notCenter(i, n, m)) { 
        cout << matrix[i] << " ";
        }
        if ((i + 1) % (m) == 0) cout << endl; 
    }
 
    delete[] matrix;    
    delete[] convolution;
    delete[] convolutionResult;
 
}