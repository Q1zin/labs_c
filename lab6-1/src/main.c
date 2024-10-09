#include <stdio.h>
#include <stdlib.h>

#define BLACK 0
#define RED 1
#define nil NULL

typedef struct node_s{
    int color;
    int data;
    struct node_s *left;
    struct node_s *right;
} node_t;

node_t *createNode(node_t *arrayPoint, int input){
    node_t *newNode = arrayPoint;
    newNode->data = input;
    newNode->color = RED;
    newNode->left = nil;
    newNode->right = nil;

    return newNode;
}

int height(node_t *rootNode){
    if (rootNode == nil) 
        return 1;

    return height(rootNode->right) + (rootNode->color == BLACK);
}

node_t *rotateLeft(node_t *rootNode, node_t *newNode, node_t **family, int length){
    node_t *tmp = newNode->right;
    newNode->right = tmp->left;
    tmp->left = newNode;

    if (length == 0) {
        return tmp;
    } else if (family[length - 1]->left == newNode) {
        family[length - 1]->left = tmp;
    } else {
        family[length - 1]->right = tmp;
    }

    return rootNode;
}

node_t *rotateRight(node_t *rootNode, node_t *newNode, node_t **family, int length){
    node_t *tmp = newNode->left;
    newNode->left = tmp->right;
    tmp->right = newNode;

    if (length == 0) {
        return tmp;
    } else if (family[length - 1]->right == newNode) {
        family[length - 1]->right = tmp;
    } else {
        family[length - 1]->left = tmp;
    }

    return rootNode;
}

node_t *balance(node_t *rootNode, node_t *newNode, node_t **family, int length){
    node_t *newRoot = rootNode;
    if (length == 0 || family[length - 1]->color == BLACK)
        return newRoot;

    node_t *parent = family[length - 1];
    node_t *grandParent = family[length - 2];
    if (grandParent->left == parent) {
        node_t *uncle = grandParent->right;
        if ((uncle != nil) && (uncle->color == RED)) {
            parent->color = BLACK;
            grandParent->color = RED;
            uncle->color = BLACK;
            if (length > 2) {
                newRoot = balance(rootNode, grandParent, family, length - 2);
            }
        } else {
            if (parent->right == newNode) {
                rootNode = rotateLeft(rootNode, parent, family, length - 1);
                parent = family[length - 2];
            }
            parent->color = BLACK;
            grandParent->color = RED;
            newRoot = rotateRight(rootNode, grandParent, family, length - 2);
        }
    } else {
        node_t *uncle = grandParent->left;
        if ((uncle != nil) && (uncle->color == RED)) {
            parent->color = BLACK;
            grandParent->color = RED;
            uncle->color = BLACK;
            if (length > 2) {
                newRoot = balance(rootNode, grandParent, family, length - 2);
            }
        } else {
            if (parent->left == newNode) {
                rootNode = rotateRight(rootNode, parent, family, length - 1);
                parent = family[length - 2];
            }
            parent->color = BLACK;
            grandParent->color = RED;
            newRoot = rotateLeft(rootNode, grandParent, family, length - 2);
        }
    }
    
    newRoot->color = BLACK;

    return newRoot;
}

node_t *insert(node_t *rootNode, node_t *newNode){
    if (rootNode == nil){
        newNode->color = BLACK;
        return newNode;
    }

    node_t *family[42];
    int length = 0;

    node_t *currentNode = rootNode;
    node_t *lastNode = currentNode;
    while (currentNode != nil) {
        lastNode = currentNode;
        family[length] = currentNode;
        ++length;
        currentNode = newNode->data > currentNode->data ? currentNode->right : currentNode->left;
    }

    if (newNode->data > lastNode->data) {
        lastNode->right = newNode;
    } else {
        lastNode->left = newNode;
    }

    return balance(rootNode, newNode, family, length);
}

int main(void) {
    FILE * inStream = fopen("in.txt", "r");
    if (!inStream) {
        return -1;
    }

    int N;
    if (fscanf(inStream, "%d", &N) != 1) {
        fclose(inStream);
        return -1;
    }

    if (N == 0) {
        fclose(inStream);
        printf("0");
        return 0;
    }

    node_t *array = malloc(N * sizeof(node_t));
    if (array == NULL) {
        fclose(inStream);
        return -1;
    }

    node_t *rootNode = nil;

    for (int i = 0; i < N; i++) {
        int input;
        if (fscanf(inStream, "%d", &input) != 1) {
            fclose(inStream);
            free(array);
            return -1;
        }

        node_t *newNode = createNode(array + i, input);

        rootNode = insert(rootNode, newNode);
    }

    printf("%d", height(rootNode));

    fclose(inStream);

    free(array);

    return 0;
}
