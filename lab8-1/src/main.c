#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

void add_edge(int* matrix, int N, int start, int end, int weight){
    matrix[(start-1) * N + (end-1)] = weight;
    matrix[(end-1) * N + (start-1)] = weight;
}

int get_edge(int* matrix, int N, int start, int end){
    return matrix[start * N + end];
}

int is_first_smaller(int currentWeight, int minWeight){
    if (minWeight == -1 && currentWeight != -1)
        return 1;
    if (currentWeight == -1)
        return 0;
    if (currentWeight < minWeight)
        return 1;
    return 0;
}

int get_index_min_weight(int* visited, int N, int* weight){
    int min_element = -1;
    int min_index = -1;
    for (int i = 0; i < N; ++i){
        if (visited[i] == 0 && is_first_smaller(weight[i], min_element)){
            min_element = weight[i];
            min_index = i;
        }
    }
    return min_index;
}

void update_weights(int* matrix, int N, int min_index, int* visited, int* weight, int* edges){
    if (min_index == -1)
        return;
    visited[min_index] = 1;
    for (int nextVertex = 0; nextVertex < N; nextVertex++){
        int weight_now = get_edge(matrix, N, min_index, nextVertex);
        if (weight_now != 0 && visited[nextVertex] == 0 && is_first_smaller(weight_now, weight[nextVertex])){
            edges[nextVertex] = min_index;
            weight[nextVertex] = weight_now;
        }
    }
}

void prim(int N, int* matrix, int* error){
    int* visited = calloc(N, sizeof(int));
    if (!visited){
        *error = ENOMEM;
        return;
    };
    int* weight = malloc(sizeof(int) * N);
    if (!weight){
        free(visited);
        *error = ENOMEM;
        return;
    };
    int* edges = malloc(sizeof(int) * N);
    if (!edges){
        free(visited);
        free(weight);
        *error = ENOMEM;
        return;
    };

    for (int i = 0; i < N; ++i){
        weight[i] = -1;
    }
    weight[0] = 0;
    edges[0] = -1;
    int min_index = -1;
    for (int i = 0; i < N; ++i){
        min_index = get_index_min_weight(visited, N, weight);
        update_weights(matrix, N, min_index, visited, weight, edges);
    }

    if (min_index != -1) {
        for (int i = 1; i < N; ++i)
            if (edges[i] != -1)
                printf("%d %d\n", edges[i] + 1, i + 1);
    } else
        printf("No spanning tree!\n");

    free(edges);
    free(weight);
    free(visited);
}

int main(){
    int N;
    FILE* inStream = fopen("in.txt","r");
    if (!inStream)
        return -1;

    if (fscanf(inStream,"%d",&N) != 1){
        fclose(inStream);
        return 0;
    }

    if (N < 0 || N > 5000) {
        printf("bad number of vertices");
        fclose(inStream);
        return 0;
    }

    if (N == 0) {
        printf("no spanning tree");
        fclose(inStream);
        return 0;
    }

    int M;
    if (fscanf(inStream,"%d",&M)!=1){
        fclose(inStream);
        return 0;
    }

    if (M < 0 || M > N*(N-1)/2) {
        printf("bad number of edges");
        fclose(inStream);
        return 0;
    }

    int* matrix = calloc(N*N, sizeof(int));
    if (!matrix){
        fclose(inStream);
        return ENOMEM;
    }

    for (int i = 0; i < M; ++i){
        int start, end, weight;
        if (fscanf(inStream, "%d %d %d", &start, &end, &weight) != 3){
            printf("bad number of lines");
            free(matrix);
            fclose(inStream);
            return 0;
        }

        if (start < 1 || start > N || end < 1 || end > N){
            printf("bad vertex");
            free(matrix);
            fclose(inStream);
            return 0;
        }

        if (weight < 0 || weight > INT_MAX) {
            printf("bad length");
            free(matrix);
            fclose(inStream);
            return 0;
        }

        add_edge(matrix, N, start, end, weight);
    }

    int error = 0;
    prim(N, matrix, &error);
    free(matrix);
    fclose(inStream);

    return error;
}
