#include <stdio.h>

#include <stdlib.h>

void swap(int * a, int * b) {
    int temp = * a;
    * a = * b;
    * b = temp;
}

void heapify(int mas[], size_t n, size_t i) {
    size_t maximum_i = i;
    size_t child_left = 2 * i + 1;
    size_t child_right = 2 * i + 2;

    if (child_left < n && mas[child_left] > mas[maximum_i])
        maximum_i = child_left;

    if (child_right < n && mas[child_right] > mas[maximum_i])
        maximum_i = child_right;

    if (maximum_i != i) {
        swap( & mas[i], & mas[maximum_i]);
        heapify(mas, n, maximum_i);
    }
}

void heapSort(int mas[], size_t n) {
    for (size_t i = n / 2-1; i+1 >= 1; i--)
        heapify(mas, n, i);

    for (size_t i = n-1; i+1 > 1; i--) {
        swap( & mas[0], & mas[i]);
        heapify(mas, i, 0);
    }
}

int main(void) {
    FILE * inStream = fopen("in.txt", "r");
    if (!inStream) {
        return 0;
    }

    size_t n;
    if (fscanf(inStream, "%zu", & n) != 1) {
        fclose(inStream);
        return 0;
    }

    if (n == 0) {
        fclose(inStream);
        return 0;
    }

    int * mas = (int * ) malloc(sizeof(int) * n);
    if (mas == NULL) {
        fclose(inStream);
        fprintf(stderr, "%d", __LINE__);
        return 0;
    }

    for (size_t i = 0; i < n; i++) {
        int elem;
        if (fscanf(inStream, "%d", & elem) != 1) {
            free(mas);
            fclose(inStream);
            return 0;
        }
        mas[i] = elem;
    }

    heapSort(mas, n);

    FILE * streamOut = fopen("out.txt", "w");
    if (!streamOut) {
        fclose(inStream);
        free(mas);
        return 0;
    }

    for (size_t i=0;i<n;i++){
        fprintf(streamOut,"%d ",mas[i]);
    }

    fclose(inStream);
    fclose(streamOut);
    free(mas);

    return 0;
}
