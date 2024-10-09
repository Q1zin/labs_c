#include <stdio.h>
#include <stdlib.h>

void swap(int *a, int *b){
    int c = *a;
    *a = *b;
    *b = c;
}

size_t getPivot(int *mas,const size_t start, const size_t end){
    int pivot = mas[(start+end)/2];
    size_t left_zone = start;
    size_t right_zone = end;
    while (left_zone<=right_zone){
        while (mas[left_zone]<pivot){
            left_zone++;
        }
        while (pivot<mas[right_zone]){
            right_zone--;
        }
        if (left_zone>=right_zone){
            break;
        }
        swap(&mas[left_zone++],&mas[right_zone--]);
    }
    return right_zone;
}

void QuickSort(int *inputArray, size_t start, size_t end)
{
    if (start>=end){
        return;
    }
    size_t pivot = getPivot(inputArray,start,end);
    QuickSort(inputArray,start,pivot);
    QuickSort(inputArray,pivot+1,end);
}

int main(void){
    FILE * inStream = fopen("in.txt", "r");
    if (!inStream){
        return 0;
    }

    size_t count;

    if (fscanf(inStream, "%zu", &count) != 1) {
        fclose(inStream);
        return 0;
    }

    if (count == 0) {
        fclose(inStream);
        return 0;
    }
    int *mas = (int*)malloc(sizeof(int)*count);
    if (mas == NULL){
        fclose(inStream);
        fprintf(stderr,"%d",__LINE__);
        return 0;
    }
    for (size_t i=0;i<count;i++){
        int elem;
        if (fscanf(inStream,"%d",&elem) != 1){
            free(mas);
            fclose(inStream);
            return 0;
        }
        mas[i] = elem;
    }

    QuickSort(mas,0,count-1);
    FILE * streamOut = fopen("out.txt", "w");
    for (size_t i=0;i<count;i++){
        fprintf(streamOut,"%d ",mas[i]);
    }
    fclose(inStream);
    fclose(streamOut);
    free(mas);
    return 0;
}
