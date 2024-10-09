#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define MAX_LEN_STR 10000

typedef struct node_t {
    char state;
    short len;
    char* str;
    struct node_t* child;
    struct node_t* r_child;
} node_t;

char* slice(char* str, int from, int to) {
    int len = to - from + 1;
    char* buff = malloc(len + 1);
    if (!buff)
        return NULL;
    strncpy(buff, str + from, len);
    buff[len] = '\0';
    return buff;
}

short comp_str(char* str1, char* str2) {
    short length = 0;
    while (str1[length] && str2[length] && str1[length] == str2[length]) {
        ++length;
    }
    return length;
}

char* xor_str(char* str1, char* str2) {
    int index = comp_str(str1, str2);
    return slice(str1, index, strlen(str1) - 1);
}

node_t* create_node(char* str, short len_str, char state, int* error){
    node_t* new_node = (node_t*)calloc(1, sizeof(node_t));
    if (!new_node) {
        *error = ENOMEM;
        return NULL;
    }
    
    new_node->str = calloc(1, (len_str + 1) * sizeof(char));
    if (!new_node->str) {
        free(new_node);
        *error = ENOMEM;
        return NULL;
    }
    strncpy(new_node->str, str, len_str);
    new_node->str[len_str] = '\0';
    new_node->len = len_str;
    new_node->state = state;
    new_node->child = NULL;
    new_node->r_child = NULL;
    return new_node;
}

void split(node_t* rootNode, short split_len, char state, int* error) {
    node_t* new_node = create_node(rootNode->str + split_len, rootNode->len - split_len, 0, error);
    if (!new_node) {
        return;
    }
    new_node->child = rootNode->child;
    rootNode->child = new_node;

    char* new_str = calloc(1, (split_len + 1) * sizeof(char));
    if (!new_str) {
        *error = ENOMEM;
        return;
    }

    strncpy(new_str, rootNode->str, split_len);
    new_str[split_len] = '\0';
    free(rootNode->str);
    rootNode->str = new_str;
    rootNode->len = split_len;
    new_node->state = state;
}

node_t* insert(node_t* rootNode, char* str, short len_str, int* error) {
    if (rootNode == NULL)
        return create_node(str, len_str, 1, error);

    short length = comp_str(rootNode->str, str);

    if (length == 0){
        if (rootNode->r_child != NULL)
            rootNode->r_child = insert(rootNode->r_child, str, len_str, error);
        else
            rootNode->r_child = create_node(str, len_str, 1, error);
    } else if (length == len_str && length == rootNode->len) {
        rootNode->state = 1;
    } else if (length == rootNode->len) {
        rootNode->child = insert(rootNode->child, str + length, len_str - length, error);
    } else if (length == len_str) {
        split(rootNode, length, 1, error);
    } else {
        split(rootNode, length, rootNode->state, error);
        rootNode->state = 0;
        rootNode->child = insert(rootNode->child, str + length, len_str - length, error);
    }

    return rootNode;
}

int count_nodes(node_t* rootNode) {
    if (rootNode == NULL) {
        return 0;
    }

    int count = 1;
    count += count_nodes(rootNode->child);
    count += count_nodes(rootNode->r_child);
    return count;
}

int get_count(node_t* rootNode){
    int count = count_nodes(rootNode);
    if (rootNode->r_child)
        ++count;

    return count;
}

void free_node(node_t* rootNode){
    if (!rootNode) return;
    free(rootNode->str);
    free_node(rootNode->child);
    free_node(rootNode->r_child);
    free(rootNode);
}

void print_comp_final(node_t* rootNode, char* prefix, int* error) {
    if (rootNode == NULL) {
        return;
    }

    int prefixLen = strlen(prefix);
    int strLen = strlen(rootNode->str);
    char* newPrefix = (char*)malloc(prefixLen + strLen + 1);
    if (!newPrefix) {
        *error = ENOMEM;
        return;
    }

    snprintf(newPrefix, prefixLen + strLen + 1, "%s%s", prefix, rootNode->str);

    if (rootNode->state == 1) {
        printf("%s ", newPrefix);
    }

    print_comp_final(rootNode->child, newPrefix, error);
    free(newPrefix);

    print_comp_final(rootNode->r_child, prefix, error);
}

void print_comp(node_t* rootNode, char* str, char* prefix, int* error){
    if (!rootNode) {
        printf("None");
        return;
    }

    int prefixLen = strlen(prefix);
    int strLen = strlen(rootNode->str);
    char* newPrefix = (char*)malloc(prefixLen + strLen + 1);
    if (!newPrefix) {
        *error = ENOMEM;
        return;
    } 
        

    snprintf(newPrefix, prefixLen + strLen + 1, "%s%s", prefix, rootNode->str);

    short equalLen = comp_str(rootNode->str, str);
    if (equalLen > 0) {
        char* diff1 = xor_str(rootNode->str, str);
        char* diff2 = xor_str(str, rootNode->str);

        if (strlen(diff2) == 0) {
            if (rootNode->state == 1) 
                printf("%s ", newPrefix);

            print_comp_final(rootNode->child, newPrefix, error);
        } else if (strlen(diff1) == 0) 
            print_comp(rootNode->child, diff2, newPrefix, error);
        else
            print_comp(rootNode->r_child, str, prefix, error);

        free(diff1);
        free(diff2);
    } else
        print_comp(rootNode->r_child, str, prefix, error);

    free(newPrefix);
}

int main(void) {
    FILE* inStream = fopen("in.txt", "r");
    if (!inStream)
        return -1;

    int N;
    if (fscanf(inStream, "%d", &N) == -1 || N <= 0){
        printf("None\n0");
        fclose(inStream);
        return 0;
    }

    node_t* rootNode = NULL;

    char* input_str = calloc(N, sizeof(char) * (MAX_LEN_STR + 1));
    if (!input_str) {
        fclose(inStream);
        return -1;
    }

    int error = 0;

    for (int i = 0; i < N; i++){
        if (fscanf(inStream, "%10000s", input_str) == -1){
            fclose(inStream);
            free(input_str);
            return -1;
        }

        rootNode = insert(rootNode, input_str, strlen(input_str), &error);
    }

    if (error) {
        fclose(inStream);
        free(input_str);
        free_node(rootNode);
        return error;
    }

    if (fscanf(inStream, "%10000s\n", input_str) == -1) {
        fclose(inStream);
        free(input_str);
        free_node(rootNode);
        return -1;
    }

    char* res = malloc(sizeof(char) * (MAX_LEN_STR + 1));
    if (!res){
        fclose(inStream);
        free(input_str);
        free_node(rootNode);
        return ENOMEM;
    }
    res[0] = '\0';

    print_comp(rootNode, input_str, res, &error);
    if (error){
        fclose(inStream);
        free(input_str);
        free(res);
        free_node(rootNode);
        return error;
    }
    printf("\n%d", get_count(rootNode));

    free(input_str);
    free(res);

    free_node(rootNode);
    fclose(inStream);
    return 0;
}
