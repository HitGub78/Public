#include <stdio.h>
#include <stdlib.h>

#define CAP 2

typedef struct queue{
    int *ar;
    int front;
    int cap;
    int back;
    int size;
}queue;


queue *init(){
    queue *Q = malloc(sizeof(queue));
    Q->cap = CAP;
    Q->front = 0;
    Q->back = 0;
    Q->size = 0;
    Q->ar = malloc(CAP*sizeof(int));
    return Q;
}

void push(int num, queue* Q){
    if (Q->size == Q->cap){
        int newcap = Q->cap*2;
        int *tmp = malloc(newcap*sizeof(int));
        for(int i = 0; i < Q->size; i++){
            tmp[i] = Q->ar[(Q->back+i)%Q->cap];
        }
        free(Q->ar);
        Q->ar = tmp;
        Q->front = Q->size;
        Q->back = 0;
        Q->cap = newcap;
    }
    Q->ar[Q->front % Q->cap] = num;
    Q->front = (Q->front + 1) % Q->cap;
    Q->size++;
}

int pop(queue *Q){
    if(Q->size == 0) return;
    int ret = Q->ar[Q->back];
    Q->back = (Q->back + 1) % Q->cap;
    Q->size--;
    return ret;
}

int main(void){
    queue *Q = init();
    push(1, Q);
    push(2, Q);
    push(3, Q);
    push(4, Q);
    push(5, Q);
    push(6, Q);
    pop(Q);
    pop(Q);
    pop(Q);
    pop(Q);
}

void bfs(int N){
    int vis[N];
    for(int i = 0; i < N; i++) vis[i] = 0;

}