#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

typedef struct point_t {
    int x;
    int y;
} point_t;

#ifdef __APPLE__
    int compare_points(void *p, const void *a, const void *b) {
#else
    int compare_points(const void *a, const void *b, void *p) {
#endif
    point_t *p0 = (point_t *)p;
    point_t *p1 = (point_t *)a;
    point_t *p2 = (point_t *)b;
    double x1 = (double)p1->x - p0->x;
    double y1 = (double)p1->y - p0->y;
    double x2 = (double)p2->x - p0->x;
    double y2 = (double)p2->y - p0->y;
    double det = x1 * y2 - y1 * x2;
    if (det == 0) {
        double length1 = x1 * x1 + y1 * y1;
        double length2 = x2 * x2 + y2 * y2;
        return length1 > length2 ? 1 : -1;
    }

    return (det > 0) ? -1 : 1;
}

int is_right_turn(point_t p, point_t q, point_t r) {
    double x1 = (double)q.x - p.x;
    double y1 = (double)q.y - p.y;
    double x2 = (double)r.x - q.x;
    double y2 = (double)r.y - q.y;
    double det = x1 * y2 - y1 * x2;
    return det <= 0;
}

void graham(point_t *points, int N) {
    int p0_index = 0;
    for (int i = 1; i < N; i++)
        if (points[i].x < points[p0_index].x || 
            (points[i].x == points[p0_index].x && points[i].y < points[p0_index].y)) 
            p0_index = i;

    point_t p0 = points[p0_index];
    points[p0_index] = points[0];
    points[0] = p0;

    #ifdef __APPLE__
        qsort_r(points + 1, N - 1, sizeof(point_t), &p0, compare_points);
    #else
        qsort_r(points + 1, N - 1, sizeof(point_t), compare_points, &p0);
    #endif
    

    point_t stack[N];
    int stack_size = 0;
    stack[stack_size++] = points[0];

    for (int i = 1; i < N; i++) {
        while (stack_size >= 2 && is_right_turn(stack[stack_size - 2], stack[stack_size - 1], points[i])) 
            stack_size--;

        stack[stack_size++] = points[i];
    }

    for (int i = 0; i < stack_size; i++) 
        printf("%d %d\n", stack[i].x, stack[i].y);
}

int main(void) {
    FILE *inStream = fopen("in.txt", "r");
    if (!inStream)
        return -1;

    int N;
    if (fscanf(inStream, "%d", &N) != 1) {
        fclose(inStream);
        return -1;
    }

    if (N < 0 || N > 100000) {
        printf("bad number of points");
        fclose(inStream);
        return 0;
    }

    if (N == 0) {
        fclose(inStream);
        return 0;
    }

    point_t *points = malloc(N * sizeof(point_t));
    if (!points) {
        fclose(inStream);
        return ENOMEM;
    }

    for (int i = 0; i < N; i++)
        if (fscanf(inStream, "%d %d", &points[i].x, &points[i].y) != 2) {
            printf("bad number of lines");
            free(points);
            fclose(inStream);
            return 0;
        }

    graham(points, N);

    fclose(inStream);
    free(points);
    return 0;
}
