#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define BUFF_IN 14
#define BUFF_OUT 90

int char_to_number(char sym, int base, int *error) {
    int result = isdigit(sym) ? sym - '0' : tolower(sym) - 'a' + 10;

    if (result < 0 || result >= base) {
        *error = 1;
        return 0;
    }

    return result;
}

char number_to_char(int v) {
    if (v >= 0 && v <= 9) {
        return '0' + v;
    }

    return 'a' + v - 10;
}

double from_b1_to_number(char* str, int b1, int *error) { 
    double fractional_part = 0,
    whole_part = 0,
    divisor = b1;
    int i = 0;

    if (str[i] == '.') {
        *error = 1;
        return 0;
    }

    while (str[i] != '.' && str[i] != '\n') {
        whole_part = whole_part * b1 + char_to_number(str[i], b1, error);
        i++;
    }

    if (str[i] == '.' && (str[i + 1] == '\0' || str[i + 1] == '\n')) {
        *error = 1;
        return 0;
    }

    i++;

    while (str[i] != '\0' && str[i] != '\n') {
        fractional_part = fractional_part + (char_to_number(str[i], b1, error) / divisor);
        divisor *= b1;
        i++;
    }

    return whole_part + fractional_part;
}

int from_number_to_b2(double number, int b2, char* res_b2) {
    long long whole_b2 = number / 1;
    double fraction_b2 = number - whole_b2;
    int i = 1,
    n = BUFF_OUT / 2 - 1,
    remainder;

    res_b2[n] = '.';

    do {
        remainder = whole_b2 % b2;
        res_b2[n - i] = number_to_char(remainder);
        whole_b2 /= b2;
        i++;
    } while (whole_b2 > 0);

    if (fraction_b2 == 0) {
        res_b2[n] = '\0';
        return n - i + 1;
    }

    int j = 1;

    while (j < 13) {
        remainder = (fraction_b2 * b2) / 1;
        res_b2[n + j] = number_to_char(remainder);
        fraction_b2 = fraction_b2 * b2 - remainder;
        j++;
    }

    res_b2[n + j] = '\0';
    return n - i + 1;
}

int main() {
    FILE * inStream = fopen("in.txt", "r");
    if (!inStream) 
        return -1;

    int b1, b2;

    if (fscanf(inStream, "%d %d\n", &b1, &b2) != 2) {
        fclose(inStream);
        return -1;
    }

    if (b1 < 2 || b1 > 16 || b2 < 2 || b2 > 16){
        printf("bad input");
        fclose(inStream);
        return 0;
    }

    char str[BUFF_IN];
    char res_b2[BUFF_OUT];

    if (!fgets(str, sizeof(str), inStream)) {
        printf("bad input");
        fclose(inStream);
        return 0;
    }

    int error = 0;

    double number = from_b1_to_number(str, b1, &error);

    if (error) {
        printf("bad input");
        fclose(inStream);
        return 0;
    }

    int pos = from_number_to_b2(number, b2, res_b2);

    printf("%s", res_b2 + pos);

    fclose(inStream);
    return 0;
}
