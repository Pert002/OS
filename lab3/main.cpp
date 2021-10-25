#include <iostream>
#include <vector>
#include <thread>
#include <string>

using namespace std;



void matrix_thread(vector<vector<double>> matrix, double (*convolution)[3], int line, int columns, double* resul)
{
    //double res = 0.0;
    //double buffer[3][3];
    for (int J = 0; J < columns - 2; J++) {
        double res = 0.0;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                res += matrix[line + i][J] * convolution[i][j];
                cout << matrix[line + i][J] * convolution[i][j] << " ";
            }
            cout << "\n\n\n\n";
        }
        cout << "--------------------------------------------------\n";
        resul[J] = res;
    }
}







int main(int argc, char* argv[]) {
    int lines, columns, k;
    cout << "k is \n";
    cin >> k;
    int ck = k;

    int thread_amount = atoi(argv[1]);

    cout << "Enter matrix's size:\n";
    cin >> lines >> columns;

    vector<vector<double>> matrix(lines, vector<double> (columns));
    vector<vector<double>> matrix2(lines, vector<double> (columns));

    cout << "Enter matrix:\n";
    for (int i = 0; i < lines; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            cin >> matrix[i][j];
            matrix2[i][j] = matrix[i][j];
        }
    }

    double convolution[3][3];
    cout << "Enter convolution matrix:\n";
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            cin >> convolution[i][j];
        }
    }

    //double buffer[3][3];
    //vector<vector<double>> result(lines - 2, vector<double>(columns - 2, 0));
    vector<thread> threads_matrix(lines - 2);

    while(k--) {
        double *resul = new double[columns - 2];
        vector<vector<double>> result(lines - 2, vector<double>(columns - 2, 0));
        if (thread_amount == -1 || thread_amount >= lines - 2) {
            for (int i = 0; i < lines - 2; i++) {
                threads_matrix[i] = thread(matrix_thread, matrix, convolution, i, columns, resul);
            }
            for (int i = 0; i < lines - 2; i++) {
                threads_matrix[i].join();
            }
            for (int i = 0; i < lines - 2; i++) {
                for (int j = 0; j < columns - 2; j++) {
                    result[i][j] = resul[j];
                    //cout << "\n" << result[i][j] << "\n";
                }
            }
        }

        else {
            for (int i = 0; i < lines - 2; i++) {
                if(i >= thread_amount && i != 0) {
                        threads_matrix[i - thread_amount].join();
                }
                threads_matrix[i] = thread(matrix_thread, matrix, convolution, i, columns, resul);
            }
            for (int i = lines - thread_amount - 2; i < lines - 2; i++) {
                threads_matrix[i].join();
            }
            for (int i = 0; i < lines - 2; i++) {
                for (int j = 0; j < columns - 2; j++) {
                    //cout << resul[j];
                    result[i][j] = resul[j];
                    //cout << "\n" << result[i][j] << "\n";
                }
            }
        }
        matrix = result;
        if (k != 0) {
        lines = lines - 2;
        columns = columns - 2;
        }
    }
    cout << "Result:\n";
    for (int i = 0; i < lines - 2; i++)
    {
        for (int j = 0; j < columns - 2; j++)
        {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
    //delete[] resul;
}

//------------------------------------------------------------------------

// First matrix (5x5)
// 1 1 1 0 0 0 1 1 1 0 0 0 1 1 1 0 0 1 1 0 0 1 1 0 0
//
// 1 1 1 0 0 
// 0 1 1 1 0
// 0 0 1 1 1
// 0 0 1 1 0
// 0 1 1 0 0
//
// Result for k = 1:
// 4 3 4
// 2 4 3
// 2 3 4

//------------------------------------------------------------------------

// Second matrix (6x6)
// 1 1 1 0 0 1 0 1 1 1 0 1 0 0 1 1 1 0 0 0 1 1 0 1 0 1 1 0 0 0 0 1 1 0 0 1
//
// 1 1 1 0 0 1
// 0 1 1 1 0 1
// 0 0 1 1 1 0
// 0 0 1 1 0 1
// 0 1 1 0 0 0
// 0 1 1 0 0 1

//------------------------------------------------------------------------

// Convolution matrix
// 1 0 1 0 1 0 1 0 1
//
// 1 0 1
// 0 1 0
// 1 0 1