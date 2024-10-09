#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct Edge{
    unsigned short start, end;
    int value;
} Edge;

int cmp(const void* a, const void* b) {
    const Edge* edgeA = (const Edge *)a;
    const Edge* edgeB = (const Edge *)b;
    if (edgeA->value == edgeB->value) {
        return edgeA->start - edgeB->start;
    }
    return edgeA->value - edgeB->value;
}

int valid_data(int N, int M){
    if (N > 5000){
        printf("bad number of vertices");
        return 1;
    }

    if (M > (N * (N - 1) / 2)){
        printf("bad number of edges");
        return 1;
    }

    if (N == 0 || M < N - 1){
        printf("no spanning tree");
        return 1;
    }

    if (M == 0)
        return 1;

    return 0;
}

int find_parent(int head, int* parent) {
    if (parent[head] == -1)
        parent[head] = head;

    if (head != parent[head])
        parent[head] = find_parent(parent[head], parent);

    return parent[head];
}

int fill_table(int N, int M, Edge* table, FILE* inStream) {
    int i = 0;

    while(i < M) {
        Edge* edge = &table[i++];

        if (fscanf(inStream, "%hu %hu", &edge->start, &edge->end) != 2){
            printf("bad number of lines");
            return 1;
        }

        if (edge->start == edge->end){
            printf("no spanning tree");
            return 1;
        }

        if (edge->start < 1 || edge->start > N || edge->end < 1 || edge->end > N){
            printf("bad vertex");
            return 1;
        }

        if (fscanf(inStream, "%d", &edge->value) != 1  || edge->value < 0){
            printf("bad length");
            return 1;
        }
    }

    if (i != M){
        printf("bad number of lines");
        return 1;
    }

    qsort(table, M, sizeof(Edge), cmp);

    return 0;
}

void kruskal(int N, int M, Edge* table, int* error) {
    Edge* result = (Edge *)malloc(sizeof(Edge) * (M));
    if (!result) {
        *error = ENOMEM;
        return;
    }

    int* parent = (int *)malloc(sizeof(int) * (N + 1));
    if (!parent) {
        *error = ENOMEM;
        free(result);
        return;
    }

    for (int i = 0; i < N + 1; i++)
        parent[i] = -1;

    int edgeCount = 0;
    for (int i = 0; i < M && edgeCount < N - 1; i++) {
        unsigned short startParent = find_parent(table[i].start, parent);
        unsigned short endParent = find_parent(table[i].end, parent);

        if (startParent != endParent) {
            parent[endParent] = startParent;
            result[edgeCount++] = table[i];
        }
    }

    if (edgeCount < N - 1)
        printf("no spanning tree\n");
    else
        for (int i = 0; i < edgeCount; i++)
            printf("%hu %hu\n", result[i].start, result[i].end);

    free(result);
    free(parent);
}

int main(void){
    FILE* inStream = fopen("in.txt", "r");

    int N, M;
    if (fscanf(inStream, "%d %d", &N, &M) != 2){
        printf("bad input");
        fclose(inStream);
        return 0;
    }

    if (valid_data(N, M)){
        fclose(inStream);
        return 0;
    }

    Edge* table = (Edge*)malloc(sizeof(Edge) * M);
    if (!table) {
        fclose(inStream);
        return ENOMEM;
    }

    if (fill_table(N, M, table, inStream)){
        free(table);
        fclose(inStream);
        return 0;
    }

    int error = 0;
    kruskal(N, M, table, &error);

    free(table);

    fclose(inStream);

    return error;
}
