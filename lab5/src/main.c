#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#define SIZE_ASCII 256

typedef struct TTree {
    unsigned char symbol;
    size_t count;
    char* code;
    struct TTree* left;
    struct TTree* right;
} TTree;

typedef struct TQueue {
    TTree* elem;
    struct TQueue* next;
} TQueue;

TQueue* push_elem(TQueue* root, TTree* tree) {
    if (root == NULL) {
        root = malloc(sizeof(TQueue));
        if (root == NULL) exit(ENOMEM);
        root->elem = tree;
        root->next = NULL;
        return root;
    }

    if (root->elem->count < tree->count)
        root->next = push_elem(root->next, tree);
    else {
        TQueue* newElem = malloc(sizeof(TQueue));
        if (newElem == NULL) exit(ENOMEM);
        newElem->elem = tree;
        newElem->next = root;
        root = newElem;
    }
    return root;
}

TTree* pop_elem(TQueue** root) {
    if (root == NULL)
        return NULL;

    TTree* pop_elem = (*root)->elem;
    TQueue* next_elem = (*root)->next;
    free(*root);
    *root = next_elem;
    return pop_elem;
}

TQueue* create_Queue(TTree* mas_chars, int count) {
    TQueue* resTable = NULL;
    for (int i = 0; i < count; i++) {
        if (mas_chars[i].count != 0)
            resTable = push_elem(resTable, &mas_chars[i]);
    }

    return resTable;
}

int is_leaf(TTree* tree) {
    return (tree->left == NULL && tree->right == NULL);
}

void fill_code_elem(TTree* tree, char* buffer, unsigned char height) {
    if (is_leaf(tree)) {
        tree->code = calloc(height + 1, sizeof(char));
        if (tree->code == NULL) exit(ENOMEM);
        memcpy(tree->code, buffer, sizeof(char) * height);
    }

    if (tree->left) {
        buffer[height] = '0';
        fill_code_elem(tree->left, buffer, height + 1);
    }

    if (tree->right) {
        buffer[height] = '1';
        fill_code_elem(tree->right, buffer, height + 1);
    }
}

void insert_to_byte(char* byte, int* index) {
    *byte |= (1 << (8 - *index - 1));
    *index += 1;
}

void print_byte(char* byte, int* index, FILE* outStream) {
    fprintf(outStream, "%c", *byte);
    *byte = 0;
    *index = 0;
}

void insert_char_to_print(char* byte, int* index, unsigned char sym, FILE* outStream) {
    for (int i = 0; i < 8; i++) {
        if (*index == 8)
            print_byte(byte, index, outStream);
        if (sym & (1 << (8 - i - 1)))
            *byte |= (1 << (8 - *index - 1));
        *index += 1;
    }

    if (*index == 8)
        print_byte(byte, index, outStream);
}

void print_huf_tree(TTree* tree, char* byte, int* index, FILE* outStream) {
    if (*index == 8)
        print_byte(byte, index, outStream);

    if (tree->left) {
        insert_to_byte(byte, index);
        print_huf_tree(tree->left, byte, index, outStream);
    }

    if (tree->right) {
        print_huf_tree(tree->right, byte, index, outStream);
    }

    if (is_leaf(tree)) {
        *index += 1;
        insert_char_to_print(byte, index, tree->symbol, outStream);
    }
}

void print_code(char* byte, int* index, unsigned char sym, TTree* table, FILE* outStream) {
    int len_code = strlen(table[(int)sym].code);
    for (int i = 0; i < len_code; i++) {
        if (*index == 8)
            print_byte(byte, index, outStream);
        if (table[(int)sym].code[i] == '1')
            *byte |= (1 << (8 - *index - 1));
        *index += 1;
    }
}

void print_huf(TTree* table, char* byte, int* index, FILE* inStream, FILE* outStream) {
    char sym = fgetc(inStream);
    while (!feof(inStream)) {
        print_code(byte, index, sym, table, outStream);
        sym = fgetc(inStream);
    }
    print_byte(byte, index, outStream);
}

void free_tree(TTree* tree) {
    if (is_leaf(tree)) {
        free(tree->code);
        return;
    }
    if (tree->left) free_tree(tree->left);
    if (tree->right) free_tree(tree->right);
    free(tree);
}

void calculating_bits_in_tree(TTree* tree, int* count) {
    if (is_leaf(tree)){
        *count += 9;
    }

    if (tree->left) {
        *count += 1;
        calculating_bits_in_tree(tree->left, count);
    }

    if (tree->right) {
        calculating_bits_in_tree(tree->right, count);
    }
}

int calculating_bits_in_string(TTree* mas_chars) {
    int count = 0;
    for (int i = 0; i < SIZE_ASCII; i++) {
        if (mas_chars[i].count != 0)
            count += mas_chars[i].count * strlen(mas_chars[i].code);
    }
    return count;
}

void encode(FILE* inStream, FILE* outStream, int start) {
    unsigned char c = fgetc(inStream);
    if (feof(inStream))
        return;

    TTree* mas_chars = calloc(SIZE_ASCII, sizeof(TTree));
    if (mas_chars == NULL) exit(ENOMEM);
    while (!feof(inStream)) {
        mas_chars[c].symbol = c;
        mas_chars[c].count += 1;
        c = fgetc(inStream);
    }

    TQueue* queue = create_Queue(mas_chars, SIZE_ASCII);

    while (queue->next != NULL) {
        TTree* tree1 = pop_elem(&queue);
        TTree* tree2 = pop_elem(&queue);
        TTree* newTree = calloc(1, sizeof(TTree));
        if (newTree == NULL) exit(ENOMEM);
        newTree->left = tree1;
        newTree->right = tree2;
        newTree->count = tree1->count + tree2->count;
        queue = push_elem(queue, newTree);
    }

    TTree* tree = pop_elem(&queue);

    if (is_leaf(tree)) {
        tree->code = calloc(2, sizeof(char));
        if (tree->code == NULL) exit(ENOMEM);
        *tree->code = '0';
    } else {
        char buffer[SIZE_ASCII];
        fill_code_elem(tree, buffer, 0);
    }

    fseek(inStream, start, SEEK_SET);

    int len_bit_text = calculating_bits_in_string(mas_chars);

    int len_bit_tree = 1;
    calculating_bits_in_tree(tree, &len_bit_tree);
    int index = (8 - ((len_bit_tree + len_bit_text) % 8)) % 8;
    char byte = 0;

    insert_to_byte(&byte, &index);

    print_huf_tree(tree, &byte, &index, outStream);
    print_huf(mas_chars, &byte, &index, inStream, outStream);

    free_tree(tree);
    free(mas_chars);
}

unsigned char get_char(char* byte, int* index, FILE* inStream) {
    unsigned char res = 0;
    for (int i = 0; i < 8; i++) {
        if (*index == 8) {
            *byte = fgetc(inStream);
            *index = 0;
        }

        if (*byte & (1 << (8 - *index - 1))) {
            res += pow(2, 8 - i - 1);
        }
        *index += 1;
    }

    if (*index == 8) {
        *byte = fgetc(inStream);
        *index = 0;
    }
    return res;
}

void make_tree(TTree** tree, char* byte, int* index, FILE* inStream) {
    if ((*tree) == NULL) {
        (*tree) = calloc(1, sizeof(TTree));
        if ((*tree) == NULL) exit(ENOMEM);
        (*tree)->left = NULL;
        (*tree)->right = NULL;
        (*tree)->symbol = 0;
        (*tree)->count = 0;
    }

    if (*index == 8) {
        *byte = fgetc(inStream);
        *index = 0;
    }

    if (feof(inStream)) {
        return;
    }

    if (*byte & (1 << (8 - *index - 1))) {
        if ((*tree)->left == NULL) {
            *index += 1;
            make_tree(&((*tree)->left), byte, index, inStream);
        }

        if ((*tree)->right == NULL) {
            make_tree(&((*tree)->right), byte, index, inStream);
        }
    } else {
        *index += 1;
        (*tree)->symbol = get_char(byte, index, inStream);
    }
}

void RunDecoding(TTree* tree, char* byte, int* index, FILE* inStream, FILE* outStream) {
    TTree* temp = tree;
    while (!feof(inStream)) {
        if (*index == 8) {
            *byte = fgetc(inStream);
            *index = 0;
            if (feof(inStream)) break;
        }

        if (*byte & (1 << (8 - *index - 1))) {
            temp = temp->right;
            *index += 1;
        } else if (temp->left) {
            temp = temp->left;
            *index += 1;
        }

        if (is_leaf(temp)) {
            fprintf(outStream, "%c", temp->symbol);
            if (is_leaf(tree))
                *index += 1;
            temp = tree;
        }
    }
}

void FreeDecodingTree(TTree* tree) {
    if (tree->left) FreeDecodingTree(tree->left);
    if (tree->right) FreeDecodingTree(tree->right);
    free(tree);
}

void decode(FILE* inStream, FILE* outStream) {
    char byte = fgetc(inStream);
    if (feof(inStream)) 
        return;

    int index = 0;
    while (!(byte & (1 << (8 - index - 1)))) {
        index += 1;
        if (index == 8) {
            fprintf(outStream, "%c", byte);
            char sym = fgetc(inStream);
            while (!feof(inStream)) {
                fprintf(outStream, "%c", sym);
                sym = fgetc(inStream);
            }
            return;
        }
    }

    index += 1;

    if (index == 8) {
        byte = fgetc(inStream);
        index = 0;
    }

    TTree* tree = NULL;

    if (byte & (1 << (8 - index - 1)))
        make_tree(&tree, &byte, &index, inStream);
    else {
        tree = calloc(1, sizeof(TTree));
        if (tree == NULL) exit(ENOMEM);
        index += 1;
        tree->symbol = get_char(&byte, &index, inStream);
    }

    RunDecoding(tree, &byte, &index, inStream, outStream);
    FreeDecodingTree(tree);
}

void print_help() {
    printf("Usage: huffman.exe <command> <file.in> <file.out>\n");
    printf("<command> - command symbol: c - compress, d - decompress\n");
    printf("<file.in> - input file\n");
    printf("<file.out> - output file\n");
    printf("Example:\n");
    printf("huffman.exe c movie.mkv movie.huf - compress\n");
    printf("huffman.exe d movie.huf movie_copy.mkv - decompress\n");
    printf("huffman.exe help - print this help\n");
}

int main(int argc, char **argv) {
    if (argc == 4) {
        FILE* inStream = fopen(argv[2], "r");
        FILE* outStream = fopen(argv[3], "w");

        char status = argv[1][0];
        if (status == 'c')
            encode(inStream, outStream, 0);
        else if (status == 'd')
            decode(inStream, outStream);

        fclose(inStream);
        fclose(outStream);

        return 0;
    } else if (argc == 1) {
        FILE* inStream = fopen("in.txt", "r");
        FILE* outStream = fopen("out.txt", "w");

        char status = fgetc(inStream);
        if (status == 'c')
            encode(inStream, outStream, 1);
        else if (status == 'd')
            decode(inStream, outStream);

        fclose(inStream);
        fclose(outStream);

        return 0;
    } else {
        print_help();
        return 0;
    }
}
