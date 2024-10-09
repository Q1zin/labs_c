#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool IsDigit(char item) {
    return item >= '0' && item <= '9';
}

bool IsDelimeter(char item){
    return item == ' ';
}

bool IsOperation(char item){
    return item == '+' || item == '-' || item == '/' ||
           item == '*' || item == '(' || item == ')';
}

int GetPriority(char item){
    switch (item) {
        case '(':
        case ')': return 1;
        case '+':
        case '-': return 2;
        case '*':
        case '/': return 3;
        default: return 6;
    }
}

struct StackStruct{
    char string[10000];
    int lastDelimeter;
    int length;
};

void StackInsert(struct StackStruct *Stack, const char* item) {
    int itemLength = strlen(item);

    if (Stack->length > 0){
        Stack->lastDelimeter = Stack->length;
        Stack->string[Stack->length++] = ' ';
    }
    strncpy(&Stack->string[Stack->length], item, itemLength);

    Stack->length += itemLength;
    Stack->string[Stack->length] = '\0';
}

char* StackPop(struct StackStruct *Stack) {
    if (Stack->length == 0) {
        return NULL;
    }

    int start = Stack->lastDelimeter;
    if (start > 0) {
        start++;
    }

    int itemLength = Stack->length - start;

    char* item = (char*)malloc((itemLength + 1) * sizeof(char));
    if (item == NULL) {
        return NULL;
    }

    strncpy(item, &Stack->string[start], itemLength);
    item[itemLength] = '\0';

    Stack->string[start] = '\0';
    Stack->length = start;
    if (start > 0) {
        Stack->length--;
        for (int i = Stack->length - 1; i >= 0; i--) {
            if (Stack->string[i] == ' ') {
                Stack->lastDelimeter = i;
                break;
            }
            if (i == 0) {
                Stack->lastDelimeter = 0;
            }
        }
    } else {
        Stack->lastDelimeter = 0;
    }

    return item;
}

char* GetExpression(const char* input, int* error) {
    char* output = malloc(10000 * sizeof(char));
    if (output == NULL) {
        return NULL;
    }
    output[0] = '\0';
    struct StackStruct Stack = {"\0", 0, 0};
    int length = strlen(input);
    bool issetDigital = false;
    for (int i = 0; i < length; i++) {
        if (IsDigit(input[i])) {
            issetDigital = true;
            while (IsDigit(input[i])) {
                char digit[2] = {input[i], '\0'};
                strcat(output, digit);

                i++;
                if (i == length) break;
            }
            strcat(output, " ");
            i--;
        } else if (IsOperation(input[i])) {
            if ((IsOperation(input[i+1])) && ((input[i] == '(' && input[i+1] == ')') || (GetPriority(input[i]) >= 2 && GetPriority(input[i+1]) >= 2))){
                *error = 1;
                free(output);
                return 0;
            } else if ((i == length-1) && (input[i] != ')')){
                *error = 1;
                free(output);
                return 0;
            }
            if (input[i] == '(') {
                char op[2] = {input[i],'\0'};
                StackInsert(&Stack, op);
            } else if (input[i] == ')') {
                char* s = StackPop(&Stack);

                while (*s != '(') {
                    strcat(output, s);
                    strcat(output, " ");
                    free(s);
                    s = StackPop(&Stack);
                }
                free(s);
            } else {
                if (Stack.length > 0) {
                    if (GetPriority(input[i]) <= GetPriority(Stack.string[Stack.length - 1])) {
                        char* lastOper = StackPop(&Stack);
                        strcat(output, lastOper);
                        strcat(output, " ");
                        free(lastOper);
                    }
                }
                char op[2] = {input[i],'\0'};
                StackInsert(&Stack, op);
            }
        } else {
            *error = 1;
            free(output);
            return 0;
        }
    }

    if (!issetDigital){
        *error = 1;
        free(output);
        return 0;
    }

    while (Stack.length > 0) {
        char* op = StackPop(&Stack);
        strcat(output, op);
        strcat(output, " ");
        free(op);
    }

    return output;
}

int Counting(const char* input, int* error) {
    struct StackStruct Stack = {"\0", 0, 0};
    char number[10000]; // Для временного хранения чисел
    int result = 0;
    int length = strlen(input);

    for (int i = 0; i < length; i++) {
        if (IsDigit(input[i]) || (input[i] == '-' && IsDigit(input[i + 1]))) {
            int numIndex = 0;

            if (input[i] == '-') { // Проверяем на отрицательное число
                number[numIndex++] = '-';
                i++;
            }

            while (!IsDelimeter(input[i]) && !IsOperation(input[i])) {
                number[numIndex++] = input[i];
                i++;
                if (i == length) break;
            }
            number[numIndex] = '\0';
            StackInsert(&Stack, number); // Добавляем число в стек
            i--;
        } else if (IsOperation(input[i])) {
            char* a_str = StackPop(&Stack); // Извлекаем первый операнд
            char* b_str = StackPop(&Stack); // Извлекаем второй операнд
            int a = atoi(a_str);
            int b = atoi(b_str);
            free(a_str);
            free(b_str);

            switch (input[i]) { // Выполняем операцию
                case '+': result = b + a; break;
                case '-': result = b - a; break;
                case '*': result = b * a; break;
                case '/': {
                    if (a != 0) {
                        result = b / a;
                        break;
                    }
                    *error = 2;
                    return 0;
                }
            }
            sprintf(number, "%d", result); // Преобразуем результат обратно в строку
            StackInsert(&Stack, number); // Вставляем результат обратно в стек
        }
    }

    char* result_str = StackPop(&Stack); // Извлекаем итоговый результат
    result = atoi(result_str); // Преобразуем результат в int
    free(result_str);
    return result;
}

int Calculate(char* input, int* error) {
    char* strOPN = GetExpression(input, error);
    if (*error == 0){
        int result = Counting(strOPN, error);
        free(strOPN);
        return result;
    }
    return 0;
}

void InitCheck(char* text, int text_size, int* error){
    if (!strcmp(text, "")){
        *error = 1;
    }

    int countBreckets = 0;
    for (int i = 0; i < text_size; ++i) {
        if (text[i] == ')'){
            --countBreckets;
        } else if (text[i] == '('){
            ++countBreckets;
        }
        if (countBreckets < 0){
            *error = 1;
        }
    }

    if (countBreckets != 0){
        *error = 1;
    }
}

int main(void){
    FILE * inStream = fopen("in.txt", "r");
    if (!inStream) {
        return 0;
    }

    FILE * streamOut = fopen("out.txt", "w");
    if (!streamOut) {
        fclose(inStream);
        return 0;
    }

    int text_size = 0;
    char text[10000];
    while (1){
        char item = getc(inStream);
        if (item=='\n')
            break;
        text[text_size] = item;
        ++text_size;
    }

    int error = 0;

    InitCheck(text, text_size, &error);

    int result;
    if (!error)
        result = Calculate(text, &error);

    switch (error) {
        case 0: printf("%d", result); break;
        case 1: printf("syntax error"); break;
        case 2: printf("division by zero"); break;
        default: printf("fatal mistake"); break;
    }

    fclose(inStream);
    fclose(streamOut);

    return 0;
}
