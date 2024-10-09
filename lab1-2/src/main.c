#include <stdio.h>
#include <stdlib.h>
#define PATTERN_SIZE 17
#define BUFF_SIZE 20

void initPi(unsigned char* pattern, int len, int* pi){
    pi[0] = 0;
    int i = 1, j = 0;

    while (i <= len-1){
        if (pattern[i] != pattern[j]){
            if (j == 0){
                pi[i] = 0;
                ++i;
            } else if (j>0) {
                j = pi[j-1];
            }
        } else {
            pi[i] = j+1;
            ++i;
            ++j;
        }
    }
}

void showPi(int* pi, int len){
    for (int k = 0; k < len; ++k) {
        printf("%d ", pi[k]);
    }
    printf("\n");
}

int bufferRewrite(unsigned char *text, int *text_index, int *read_len, int pattern_size, int *del_symbol, FILE * inStream) {
    for (int i = 0; i < pattern_size; i++) {
        text[i] = text[*read_len - pattern_size + i]; // Копирование ласт символов
    }
    *del_symbol += *read_len - pattern_size;
    *read_len = (int)fread(text + pattern_size, sizeof(unsigned char), *read_len - pattern_size, inStream);
    *read_len += pattern_size;
    *text_index = 0;
    if (*read_len != BUFF_SIZE) {
        return 0;
    } else {
        return 1;
    }
}

int checkPrefixCoincidences(int pattern_index, int pattern_size, int read_len, int text_index, unsigned char *pattern, unsigned char *text) {
    if (pattern_index == pattern_size) {
        return 1;
    } else if (text_index + 1 != read_len && pattern[pattern_index] != text[text_index + 1] && text_index - pattern_index + pattern_size < read_len) {
        return 1;
    }
    return 0;
}

int main() {
    FILE * inStream = fopen("in.txt", "r");
    if (!inStream) {
        return 0;
    }

    unsigned char pattern[PATTERN_SIZE];

    int pattern_size = 0;
    while (1){
        char item = getc(inStream);
        if (item=='\n')
            break;
        pattern[pattern_size] = item;
        ++pattern_size;
    }

    if (pattern_size == 0){
        fclose(inStream);
        return 0;
    }

    int pi[PATTERN_SIZE];

    initPi(pattern, pattern_size, pi);
    showPi(pi, pattern_size);

    int end_flag = 1;
    unsigned char *text = (unsigned char *) malloc(sizeof(unsigned char) * (BUFF_SIZE));
    int read_len = fread(text, sizeof(unsigned char), BUFF_SIZE, inStream);
    if (read_len != BUFF_SIZE) {
        end_flag = 0;
    }

    int pattern_index = 0;
    int del_symbol = 0;
    int text_index = 0;
    while (text_index < read_len) {
        while (pattern_index > 0 && pattern[pattern_index] != text[text_index]) {
            pattern_index = pi[pattern_index - 1];
        }
        if (pattern[pattern_index] == text[text_index]) {
            pattern_index++;
            if (checkPrefixCoincidences(pattern_index, pattern_size, read_len, text_index, pattern, text)) {
                printf("%d %d ", text_index - pattern_index + 2 + del_symbol, pattern_index);
            }
        }
        text_index++;
        if (text_index + pattern_size >= read_len && end_flag) {
            end_flag = bufferRewrite(text, &text_index, &read_len, pattern_size, &del_symbol, inStream);
        }
    }

    fclose(inStream);
    free(text);
    return 0;
}
