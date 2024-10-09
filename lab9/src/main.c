#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int is_first_smaller(int currentWeight, int minWeight){
    if (minWeight == -1 && currentWeight != -1)
        return 1;
    if (currentWeight == -1)
        return 0;
    if (currentWeight < minWeight)
        return 1;
    return 0;
}

void push_in_matrix(const int N, const unsigned int s, const unsigned int f, const unsigned int l, unsigned int* matrix) {
    matrix[(s - 1) * N + f - 1] = l;
    matrix[(f - 1) * N + s - 1] = l;
}

unsigned int get_length(const int N, const unsigned int s, const unsigned int f, unsigned int* matrix) {
    return matrix[s * N + f];
}

void dijkstra(const int N, const int start, const int finish, unsigned int* matrix, long long *path, int* error) {
    path[start - 1] = 0;
    int* visited = calloc(N, sizeof(int));
    if (!visited) {
        *error = ENOMEM;
        return;
    }

    while (1) {
        long long min_elem = -1;
        int min_index = -1;
        for (int i = 0; i < N; i++)
            if (path[i] != -1 && visited[i] == 0)
                if (is_first_smaller(path[i], min_elem)){
                    min_elem = path[i];
                    min_index = i;
                }

        if (min_index == -1)
            break;

        for (int i = 0; i < N; i++){
            unsigned int len = get_length(N, min_index, i, matrix);
            if ((visited[i] == 0 && len != 0) && (path[i] == -1 || path[i] > path[min_index] + len))
                path[i] = path[min_index] + len;
        }

        visited[min_index] = 1;
    }

    for (int i = 0; i < N; i++)
        if (path[i] == -1)
            printf("oo ");
        else if (path[i] > INT_MAX)
            printf("INT_MAX+ ");
        else
            printf("%lld ", path[i]);

    printf("\n");

    int index = finish - 1;
    if (path[index] == -1)
        printf("no path");
    else {
        int count = 0;

        for (int i = 0; i < N; i++){
            unsigned int len = get_length(N, finish - 1, i, matrix);
            if (len != 0 && path[finish-1] + len >= (unsigned)INT_MAX + 1)
                count++;
        }


        if (path[finish - 1] > INT_MAX && count > 1){
            printf("overflow");
            free(visited);
            return;
        }

        int *answer = malloc(N * sizeof(int));
        if (!answer) {
            *error = ENOMEM;
            free(visited);
            return;
        }

        count = 1;
        answer[0] = finish;

        while (index != start - 1){
            for (int i = 0; i < N; i++){
                unsigned int len = get_length(N, index, i, matrix);
                if (len != 0 && path[index] == path[i] + len){
                    answer[count] = i + 1;
                    index = i;
                    break;
                }
            }
            count++;
        }

        for (int i = 0; i < count; i++)
            printf("%d ", answer[i]);

        free(answer);
    }

    free(visited);
}

int main(void) {
    FILE* inStream = fopen("in.txt", "r");
    if (!inStream)
        return -1;

    int N;
    if (fscanf(inStream, "%d", &N) != 1) {
        fclose(inStream);
        return -1;
    }

    if (N < 0 || N > 5000){
        fclose(inStream);
        printf("bad number of vertices");
        return 0;
    }

    int S, F;
    if (fscanf(inStream, "%d %d", &S, &F) != 2) {
        fclose(inStream);
        return -1;
    }
    if (S < 1 || S > N || F < 1 || F > N) {
        printf("bad vertex");
        fclose(inStream);
        return 0;
    }

    int M;
    if (fscanf(inStream, "%d", &M) != 1) {
        fclose(inStream);
        return -1;
    }

    if (M < 0 || M > N*(N-1)/2){
        fclose(inStream);
        printf("bad number of edges");
        return 0;
    }

    unsigned int* matrix = calloc(N * N,sizeof(int));
    if (!matrix){
        fclose(inStream);
        return ENOMEM;
    }

    for (int i = 0; i < M; i++){
        int start, finish, len;
        if (fscanf(inStream, "%d %d %d", &start, &finish, &len) != 3) {
            printf("bad number of lines");
            fclose(inStream);
            free(matrix);
            return 0;
        }
        if (start < 1 || start > N || finish < 1 || finish > N) {
            printf("bad vertex");
            fclose(inStream);
            free(matrix);
            return 0;
        }

        if (len < 0 || len > INT_MAX) {
            printf("bad length");
            fclose(inStream);
            free(matrix);
            return 0;
        }

        push_in_matrix(N, start, finish, len, matrix);
    }

    long long* path = (long long*)calloc(1, N * sizeof(long long));
    if (!path) {
        free(matrix);
        fclose(inStream);
        return ENOMEM;
    }
    memset(path, -1, N * sizeof(long long));
    
    int error = 0;
    dijkstra(N, S, F, matrix, path, &error);

    free(path);
    free(matrix);
    fclose(inStream);
    return error;
}
