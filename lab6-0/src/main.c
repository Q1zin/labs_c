#include <stdio.h>
#include <stdlib.h>

typedef struct node_s{
    int data;
    int height;
    struct node_s *left;
    struct node_s *right;
} node_t;

node_t *createNode(node_t *arrayPoint, int input){
    node_t *newNode = arrayPoint;
    newNode->data = input;
    newNode->height = 1;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

int height(node_t *node){
    int lsh = node->left ? node->left->height : 0;
    int rsh = node->right ? node->right->height : 0;
    return (lsh > rsh ? lsh : rsh) + 1;
}

int balance_factor(node_t *node){
    int lsh = node->left ? node->left->height : 0;
    int rsh = node->right ? node->right->height : 0;
    return lsh-rsh;
}

node_t *ll_rotate(node_t *node){
    node_t *lchild = node->left;
    node->left = lchild->right;
    lchild->right = node;

    node->height = height(node);
    lchild->height = height(lchild);

    return lchild;
}

node_t *lr_rotate(node_t *node){
    node_t *bchild = node->left->right;
    node_t *lchild = node->left;

    node->left = bchild->right;
    lchild->right = bchild->left;
    bchild->right = node;
    bchild->left = lchild;

    node->height = height(node);
    lchild->height = height(lchild);
    bchild->height = height(bchild);

    return bchild;
}

node_t *rl_rotate(node_t *node){
    node_t *bchild = node->right->left;
    node_t *rchild = node->right;

    node->right = bchild->left;
    rchild->left = bchild->right;
    bchild->left = node;
    bchild->right = rchild;

    node->height = height(node);
    rchild->height = height(rchild);
    bchild->height = height(bchild);

    return bchild;
}

node_t *rr_rotate(node_t *node){
    node_t *rchild = node->right;

    node->right = rchild->left;
    rchild->left = node;

    node->height = height(node);
    rchild->height = height(rchild);

    return rchild;
}

node_t *balance(node_t *rootNode){
    rootNode->height = height(rootNode);

    int node_bf = balance_factor(rootNode);

    if (node_bf == 2){
        int left_subtree_bf = balance_factor(rootNode->left);
        if (left_subtree_bf == 1)
            return ll_rotate(rootNode);

        if (left_subtree_bf == -1)
            return lr_rotate(rootNode);

    } 
    if (node_bf == -2){
        int right_subtree_bf = balance_factor(rootNode->right);
        if (right_subtree_bf == 1)
            return rl_rotate(rootNode);

        if (right_subtree_bf == -1)
            return rr_rotate(rootNode);

    }

    return rootNode;
}

node_t *insert(node_t *rootNode, node_t *newNode){
    if (rootNode == NULL) 
        return newNode;

    if (newNode->data > rootNode->data) {
        rootNode->right = insert(rootNode->right, newNode);
    } else {
        rootNode->left = insert(rootNode->left, newNode);
    }

    return balance(rootNode);
}

int main(void) {
    FILE * inStream = fopen("in.txt", "r");
    if (!inStream) 
        return -1;


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

    node_t *rootNode = NULL;

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
