#include <iostream>
#include <vector>
#include <thread>
#include <string>

using namespace std;



void get_pos(double (*original)[3], double (*convolution)[3], int orig_line, int conv_col, double (*result)[3])
{
    double res = 0.0;
    res = original[orig_line][conv_col] * convolution[orig_line][conv_col];

    result[orig_line][conv_col] = res;
}


void get_matr(double (*buffer)[3], double (*conv)[3], double (*res)[3], int thread_amount)
{
    vector<thread> th(thread_amount);
    int current = 0;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            th[current] = thread(get_pos, buffer, conv, i, j, res);
            current++;
            if (current == thread_amount)
            {
                current = 0;
                for (int t = 0; t < thread_amount; t++)
                {
                    th[t].join();
                }
            }
        } 
    }

    for (int t = 0; t < current; t++)
    {
        th[t].join();
    }
}






int main() {
    int lines, columns, k;
    cout << "k is \n";
    cin >> k;
    int ck = k;

    cout << "Enter matrix's size:\n";
    cin >> lines >> columns;

    vector<vector<double>> orig(lines, vector<double> (columns));
    vector<vector<double>> orig2(lines, vector<double> (columns));

    cout << "Enter matrix:\n";
    for (int i = 0; i < lines; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            cin >> orig[i][j];
            orig2[i][j] = orig[i][j];
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

    int thread_amount;
    cout << "Enter threads amount:\n";
    cin >> thread_amount;
    cout << "\n";

    int m = 0;
    double buffer[3][3];
    vector<vector<double>> result(lines - ck + k - m, vector<double>(columns - ck + k - m, 0));

    while(k--) {
        m++;
        for (int I = 0; I < lines - ck + k - m; I++) {
            for (int J = 0; J < columns - ck + k - m; J++) {
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        buffer[i][j] = orig[i + I][j + J];
                    }
                }


                double res[3][3];
                get_matr(buffer, convolution, res, thread_amount);
                for(int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        result[I][J] += res[i][j];
                    }
                }

                orig2[I][J] = result[I][J];
                result[I][J] = 0;
            }
        }
        orig = orig2;
    }
    cout << "Result:\n";
    for (int i = 0; i < lines - ck - m; i++)
    {
        for (int j = 0; j < columns - ck - m; j++)
        {
            cout << orig[i][j] << " ";
        }
        cout << endl;
    }
    return 0;
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