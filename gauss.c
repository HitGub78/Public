#include <stdio.h>
#include <stdlib.h>
#define EPS 1e-18

void swap(double *a, double *b){
    double tmp = *a;
    *a = *b;
    *b = tmp;
}

int equal(double a, double b){
    if (abs(a - b) < EPS) return 1;
    return 0; 
}

void move(int N, double mat[N][N], int A, int B){
    for(int i = 0; i < N; i++) 
        swap(mat[A] + i, mat[B] + i);
}

void subtract(int N, double mat[N][N], int A, int B, int x){
    for(int i = 0; i < N; i++)
        mat[A][i] -= mat[B][i]*x;
}

void print(int N, double mat[N][N]){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            printf("%lf ",mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int find_nonzero(int N, double mat[N][N], int j, int col){
    int flag = -1;
    for(int i = j; i < N; i++){
        if (flag == -1 && !equal(mat[i][col], 0)) flag = i;
    }
    return flag;
}

double Gauss(int N, double mat[N][N]){
    int sign = 1;
    for(int j = 0; j < N; j++){
        if(equal(mat[j][j], 0)){
            int s = find_nonzero(N, mat, j, j);
            if (s == -1) sign = 0;
            else{
                move(N, mat, j, s);
                sign = -1;
            }
        }
        if (sign != 0){
            for(int i = j+1; i < N; i++){
                if (!equal(mat[i][j],0)){
                    double x = mat[i][j]/mat[j][j];
                    subtract(N, mat, i, j, x);
                }
            }
        }
    }
    if (sign == 0) return 0.0;
    double det = 1.0;
    for(int k = 0; k < N; k++)
        det *= mat[k][k];
    return det*sign;
}

int main(){
    int N;
    scanf("%d",&N);
    double mat[N][N];
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
            scanf("%lf", mat[i] + j);
    printf("%lf",Gauss(N, mat));
}
