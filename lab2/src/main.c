#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool is_valid_permutation(char* str, int strLen) {
    int count[10] = {0};
    for (int i = 0; i < strLen; i++) {
        if ((str[i] < '0') || (str[i] > '9')) 
            return false;
        if (count[str[i] - '0']++) 
            return false;
    }
    return true;
}

bool next_permutation(char *str, int strLen) {
    int left_index = strLen - 2;
    while ((left_index >= 0) && (str[left_index] >= str[left_index + 1]))
        --left_index;

    if (left_index < 0) 
        return false;

    int right_index = strLen - 1;
    while (str[right_index] <= str[left_index])
        --right_index;

    char temp = str[left_index];
    str[left_index] = str[right_index];
    str[right_index] = temp;

    int i = left_index + 1;
    int j = strLen - 1;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }

    return true;
}

int main() {
    char string[13];
    int count_print;

    if (fgets(string, sizeof(string) / sizeof(string[0]), stdin) == NULL){
        return 0;
    }

    int strLen = strlen(string) - 1;

    if (scanf("%d", &count_print) != 1){
        printf("bad input");
        return 0;
    }

    if (!is_valid_permutation(string, strLen)) {
        printf("bad input");
        return 0;
    }

    for (int i = 0; i < count_print; i++) {
        if (!next_permutation(string, strLen)) 
            break;
        printf("%s", string);
    }

    return 0;
}
