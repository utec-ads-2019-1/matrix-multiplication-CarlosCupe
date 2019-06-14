#include <iostream>
#include <vector>
#include <pthread.h>
#include <time.h>
#include <thread>

using namespace std;

unsigned int NumThreads = thread::hardware_concurrency();

int **matrix_1, **matrix_0, **ans;

unsigned int row_size, col_size, cross_size;
unsigned int rows_m0, cols_m0, rows_m1, cols_m1;
bool rest_size;

clock_t t_ini, t_fin;
double secs;

void *task(void *val) {
    long index = (long) val;
    unsigned min_row, max_row, min_col, max_col;
    int temp_sum;
    
    min_row = index * row_size;
    min_col = index * col_size;

    if (index == NumThreads) {
        max_row = min_row + (rows_m0 % NumThreads);
        max_col = min_col + (cols_m1 % NumThreads);
    } else {
        max_row = min_row + row_size;
        max_col = min_col + col_size;
    }
    
    for(int i = min_row; i < max_row; ++i) {
        for (int j = min_col; j < max_col; ++j) {
            temp_sum = 0;
            for (int k = 0; k < cross_size; ++k) {
                temp_sum += (matrix_0[i][k] * matrix_1[k][j]);
            }
            ans[i][j] = temp_sum;
        }
    }
}

void create_matrix(bool index, int rows, int cols) {
    int **temp;
    
    temp = new int*[rows];

    for (int i = 0; i < rows; ++i) {
        temp[i] = new int[cols];
        for (int j = 0 ; j < cols; ++j) {
            temp[i][j] = i * rows + j;
        }
    }
    
    if (index) matrix_1 = temp;
    else matrix_0 = temp;
    
    cout << "[ OK ]\t Created matrix "<< index << endl;
}

void void_matrix(int rows, int cols) {
    ans = new int*[rows];
    for (int i = 0; i < rows; ++i) {
        ans[i] = new int[cols];
    }
    cout << "[ OK ]\t Created void matrix" << endl;
}

int main() {
    if (NumThreads == 0) return 1;

    cout << "[ OK ]\t Detect " << NumThreads << " threads"<< endl;

    cout << "[ IN ]\t Matrix 0 - Ingresar Filas Colummas: ";
    cin >> rows_m0 >> cols_m0;
    cout << "[ IN ]\t Matrix 1 - Ingresar Filas Colummas: ";
    cin >> rows_m1 >> cols_m1;

    if (cols_m0 != rows_m1) {
        cout << "[ ERROR ]\t Columna de Matrix 0 y Fila de Matrix 1 son diferentes" << endl;
        return 1;
    }

    cross_size = cols_m0;
    
    create_matrix(0, rows_m0, cross_size);
    create_matrix(1, cross_size, cols_m1);

    void_matrix(rows_m0, cols_m1);

    unsigned int max_size;

    max_size = (rows_m0 > cols_m1) ? rows_m0 : cols_m1;

    if (max_size < NumThreads) NumThreads = max_size;
    
    rest_size = rows_m0 % NumThreads != 0 || cols_m1 % NumThreads != 0;
    
    if (rest_size) --NumThreads;
    
    row_size = rows_m0 / NumThreads;
    col_size = cols_m1 / NumThreads;

    cout << "[ OK ]\t Groups of size " << row_size << " rows - "<< col_size << " cols" << endl;

    pthread_t threads[NumThreads + rest_size];
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    cout << "[ OK ]\t Configuration complete"<< endl;
    
    int rc, i;
    void *status;
    
    t_ini = clock();

    for (i = 0; i < NumThreads + rest_size; ++i) {
        cout << "[ OK ]\t Thread "<< i <<" in progress ";
        rc = pthread_create(&threads[i], &attr, task, (void*)i );
        cout << "- Complete" << endl;
    }

    pthread_attr_destroy(&attr);
    for(i = 0; i < NumThreads + rest_size; ++i) {
        rc = pthread_join(threads[i], &status);
        if (rc) {
            cout << "[ ERROR ]\t Unable to join," << rc << endl;
            exit(-1);
        }
        cout << "[ OK ]\t Completed thread id: " << i ;
        cout << "  exiting with status: " << status << endl;
    }
    t_fin = clock();

    secs = (double)(t_fin - t_ini) / CLOCKS_PER_SEC;
    cout << "[ OUT ]\t Tiempo de ejecucion " << secs * 1000.0 <<" milisegundos" << endl;

    cout << "Main: program exiting." << endl;
    pthread_exit(NULL);    
    return 0;
}