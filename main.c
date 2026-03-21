#include <stdio.h>
#include <stdlib.h> 
#include <strings.h>
#include <math.h> 
#include "lodepng.h" 

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
    if(Q->size == 0) return -1;
    int ret = Q->ar[Q->back];
    Q->back = (Q->back + 1) % Q->cap;
    Q->size--;
    return ret;
}

//standart BFS
void BFS(int start, int *ar, int bw_size, int *CSR, int *vis, unsigned char *grey, int *count) {
    queue *Q = init();
    queue *connected = init();
    int num;
    push(start, Q);
    push(start, connected);
    vis[start] = 1;
    while(Q->size) {
        num = pop(Q);
        if (num < bw_size-1) {
            for(int i = ar[num]; i < ar[num+1]; i++) {
                if (CSR[i] != 0 && !vis[CSR[i]]) {
                    push(CSR[i], Q);
                    push(CSR[i], connected); 
                    vis[CSR[i]] = 1;
                }
            }
        }
    }
    // anything consisting of this much pixels is likely not a tanker
    if (connected->size > 8){
        while(connected->size){
            int s = pop(connected);
            grey[s] = 0;
        }
    }
    else{
        *count += 1;
    }
    free(connected);
    free(Q->ar);
    free(Q);
}

int graphify(unsigned char *grey, int width, int height) {
    int bw_size = height * width;
    int *ar = malloc(bw_size * sizeof(int));
    int total = 0;
    // standart CSR graph creation
    for(int i = 0; i < bw_size; i++) {
        ar[i] = total;
        int neighbours = 0;
        if (i >= width) neighbours++;
        if (i < (height-1)*width) neighbours++;
        if (i % width != 0) neighbours++;
        if (i % width != width-1) neighbours++;
        total += neighbours;
    }

    int *car = malloc(total * sizeof(int));
    
    for(int i = 0; i < bw_size; i++) {
        if (grey[i] > 128) {
            int t = 0;
            if (i >= width && grey[i-width] > 128) {
                car[ar[i] + t] = i-width;
                t++;
            }
            if (i < (height-1)*width && grey[i+width] > 128) {
                car[ar[i] + t] = i+width;
                t++;
            }
            if (i % width != 0 && grey[i-1] > 128) {
                car[ar[i] + t] = i-1;
                t++;
            }
            if (i % width != width-1 && grey[i+1] > 128) {
                car[ar[i] + t] = i+1;
                t++;
            }
        }
    }
    int count = 0;

    int *vis = calloc(bw_size,sizeof(int));
    // if we find a lump of pixels connected to each other, we either remove it from the 
    // picture, or count it as a ship
    for(int i = 0; i < bw_size; i++){
        if(!vis[i] && grey[i] > 0){
            BFS(i, ar, bw_size, car, vis, grey, &count);
        }
    }

    free(ar);
    free(car);
    free(vis);
    return count;
}

// принимаем на вход: имя файла, указатели на int для хранения прочитанной ширины и высоты картинки
// возвращаем указатель на выделенную память для хранения картинки
// Если память выделить не смогли, отдаем нулевой указатель и пишем сообщение об ошибке
unsigned char* load_png(const char* filename, unsigned int* width, unsigned int* height) 
{
  unsigned char* image = NULL; 
  int error = lodepng_decode32_file(&image, width, height, filename);
  if(error != 0) {
    printf("error %u: %s\n", error, lodepng_error_text(error)); 
  }
  return (image);
}

// принимаем на вход: имя файла для записи, указатель на массив пикселей,  ширину и высоту картинки
// Если преобразовать массив в картинку или сохранить не смогли,  пишем сообщение об ошибке
void write_png(const char* filename, const unsigned char* image, unsigned width, unsigned height)
{
  unsigned char* png;
  long unsigned int pngsize;
  int error = lodepng_encode32(&png, &pngsize, image, width, height);
  if(error == 0) {
      lodepng_save_file(png, pngsize, filename);
  } else { 
    printf("error %u: %s\n", error, lodepng_error_text(error));
  }
  free(png);
}

//turns rgb picture into black and white for which each pixel 
// is determined by 1 number
void blackwhite(unsigned char *col, unsigned char *grey, int bw_size){
    for(int i = 0; i < bw_size; i++){
        grey[i] = (unsigned char)(0.299*col[i*4] + 0.587*col[i*4+1] + 0.114*col[i*4+2]);        
    }
    return;
}
// turns black and white picture to "colored" for write_png to work
void torgb(unsigned char *col, unsigned char *grey, int bw_size){
    for(int i = 0; i < bw_size; i++){
        col[4*i] = grey[i];
        col[4*i+1] = grey[i];
        col[4*i+2] = grey[i];
        col[4*i+3] = 255; 
    }
    return;
}
// вариант огрубления серого цвета в ЧБ 
void contrast(unsigned char *col, int bw_size)
{ 
    int i; 
    for(i=0; i < bw_size; i++)
    {
        if(col[i] >= 100)
        col[i] = 255;
    } 
    return; 
} 
// for simplicity
void radical(unsigned char *col, int bw_size){
    for(int i = 0; i < bw_size; i++){
        if (col[i] > 30) col[i] = 255;
        else col[i] = 0;
    }
    
}

// Гауссово размыттие
void Gauss_blur(unsigned char *col, unsigned char *blr_pic, int width, int height)
{ 
    int i, j; 
    for(i=1; i < height-1; i++) 
        for(j=1; j < width-1; j++)
        { 
            blr_pic[width*i+j] = 0.111*col[width*i+j] +           
                                0.111*col[width*(i+1)+j] +    
                                0.111*col[width*(i-1)+j] + 
                                0.111*col[width*i+(j+1)] + 
                                0.111*col[width*i+(j-1)] + 
                                0.111*col[width*(i+1)+(j+1)] + 
                                0.111*col[width*(i+1)+(j-1)] + 
                                0.111*col[width*(i-1)+(j+1)] + 
                                0.111*col[width*(i-1)+(j-1)];
        }
   return; 
} 

//  Место для экспериментов
void color(unsigned char *blr_pic, unsigned char *res, int size)
{ 
  int i;
    for(i=1;i<size;i++) 
    { 
        res[i*4]=40+blr_pic[i]+0.35*blr_pic[i-1]; 
        res[i*4+1]=65+blr_pic[i]; 
        res[i*4+2]=170+blr_pic[i]; 
        res[i*4+3]=255; 
    } 
    return; 
} 
  
int main() 
{ 
    const char* filename = "skull.png"; 
    unsigned int width, height;
    int size;
    int bw_size;
    
    // Прочитали картинку
    unsigned char* picture = load_png("skull.png", &width, &height); 
    if (picture == NULL)
    { 
        printf("Problem reading picture from the file %s. Error.\n", filename); 
        return -1; 
    } 

    size = width * height * 4;
    bw_size = width * height;
    
    unsigned char* blr_pic = (unsigned char*)malloc(size*sizeof(unsigned char)); 
    unsigned char* contr = (unsigned char*)malloc(size*sizeof(unsigned char)); 
    unsigned char* grey = (unsigned char*)malloc(bw_size*sizeof(unsigned char));
    unsigned char *blr_grey = malloc(bw_size*sizeof(unsigned char));

    blackwhite(picture, grey, bw_size);
    torgb(contr, grey, bw_size);
    contrast(contr, size);
    
    // Contrasted 
    write_png("contrast.png", contr, width, height);
    blackwhite(picture, grey, bw_size);
    for(int i = 0; i < 10; i++){
        Gauss_blur(grey, blr_grey, width, height);
        for(int j = 0; j < bw_size; j++) grey[j] = blr_grey[j];
    }
    torgb(blr_pic, blr_grey, bw_size);
    // blurred (original)
    write_png("gauss.png", blr_pic, width, height);
    blackwhite(contr, grey, bw_size);
    // subtract the blurred from contrasted, in hopes of removing the clouds
    for(int i = 0; i < bw_size; i++){
        int value = (int)grey[i] - (int)blr_grey[i];
        grey[i] = (value > 0) ? value : 0; 
    }
    radical(grey, bw_size);
    torgb(picture, grey, bw_size);
    write_png("intermediate_result.png", picture, width, height);
    // manually clearing areas with no ships
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if (j <= 500 || (j > 720 && i < 250) || i < 5 || i > (height-5))
            grey[width*i+j] = 255;
        }
    }
    int tankers = graphify(grey, width, height);
    torgb(picture, grey, bw_size);
    // to make sure modified pictures mostly includes all the same dots as the original
    write_png("picture_out.png", picture, width, height);

    printf("%d tankers are in the strait\n", tankers);
    
    
    free(grey); 
    free(blr_pic); 
    free(contr); 
    free(picture); 
    
    return 0; 
}
