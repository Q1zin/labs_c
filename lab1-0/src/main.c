#include <stdio.h>
#include <string.h>

#define MAX_PATTERN_SIZE 18
#define BUFF_LEN 40

void create_shift_table(int* shift_table, char* pattern, int pattern_len) {
    for (int i = 0; i < 256; i++) { 
        shift_table[i] = pattern_len;
    }

    for (int i = 0; i < pattern_len - 1; i++) {
        shift_table[(unsigned char)pattern[i]] = pattern_len - 1 - i;
    }
}


int bm_search(char* str, int str_len, char* pattern, int pattern_len, int* shift_table, int global_pos) {
    int search_pos = pattern_len;
    while (search_pos <= str_len) {
        int pattern_pos = pattern_len;
        int current_match_pos = search_pos;

        do {
            --pattern_pos;
            --current_match_pos;
            printf("%d ", global_pos + current_match_pos);
        } while ((str[current_match_pos] == pattern[pattern_pos]) && (pattern_pos > 0));

        search_pos += shift_table[(unsigned char)str[search_pos-1]];
    }
    return str_len + pattern_len - search_pos;
}

int main(void) {
    FILE* inStream = fopen("in.txt", "r"); 
    char pattern[MAX_PATTERN_SIZE]; 
    if (fgets(pattern, MAX_PATTERN_SIZE, inStream) == NULL) {
        fclose(inStream); 
        return 0;
    }

    int pattern_len = strlen(pattern) - 1;
    int shift_table[256] = { 0 };

    create_shift_table(shift_table, pattern, pattern_len); 

    char str[BUFF_LEN];
    int read_len = 0, tail_len = 0, global_pos = 1;
    while ((read_len = fread(str + tail_len, 1, BUFF_LEN - tail_len, inStream)) > 0) {
        int str_len = read_len + tail_len;
        tail_len = bm_search(str, str_len, pattern, pattern_len, shift_table, global_pos);
        int used_len = str_len - tail_len;
        memmove(str, str + used_len, tail_len);
        global_pos += used_len;
    }

    fclose(inStream);

    return 0;
}
