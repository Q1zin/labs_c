#include <stdio.h>

int max(int a, int b){
    return (a > b) ? a : b;
}

int main() {
    FILE* inStream = fopen("in.txt", "r");
    if (inStream == NULL) {
        printf("Caput -_-");
        return 1;
    }

    unsigned N;
    unsigned max_w;

    if (fscanf(inStream, "%u %u", &N, &max_w) != 2) {
        fclose(inStream);
        return 1;
    }

    int P[N];
    int W[N];

    int k = 0, i, j;

    while (fscanf(inStream, "%d %d", &i, &j) == 2) {
        P[k] = j;
        W[k] = i;
        ++k;
    }

    unsigned A[N+1][max_w+1];

    for (unsigned s = 0; s <= max_w; ++s){
        A[0][s] = 0;
    }

    for (unsigned k = 1; k <= N; ++k){
        int price_now = P[k-1];
        unsigned weight_now = W[k-1];

        for (unsigned s = 0; s <= max_w; ++s) {
            if (s < weight_now) {
                A[k][s] = A[k-1][s];
            } else {
                A[k][s] = max(A[k-1][s], A[k-1][s-weight_now] + price_now);
            }
        }
    }

    printf("%u\n", A[N][max_w]);

    int out[N][2];
    int count = 0;
    unsigned s = max_w;
    for (int k = N; k > 0; --k) {
        while ((s >= 1) && (A[k][s] == A[k][s-1])){
                --s;
        }
        if (A[k][s] != A[k-1][s]) {
            out[count][0] = W[k-1];
            out[count][1] = P[k-1];
            ++count;
            s -= W[k-1];
        }
    }

    while (count > 0){
        --count;
        printf("%d %d\n",out[count][0],out[count][1]);
    }

    fclose(inStream);
    return 0;
}
