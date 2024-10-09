#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

typedef struct node_t {
    int count;
    int* keys;
    struct node_t** links;
    bool is_leaf;
} node_t;

node_t* create_node(int B, bool leaf, int* error) {
    node_t* new_node = malloc(sizeof(node_t));
    if (!new_node) {
        *error = ENOMEM;
        return NULL;
    }

    new_node->count = 0;
    new_node->is_leaf = leaf;
    new_node->keys = malloc((2 * B - 1) * sizeof(int));
    if (!new_node->keys) {
        free(new_node);
        *error = ENOMEM;
        return NULL;
    }

    new_node->links = malloc(2 * B * sizeof(node_t*));
    if (!new_node->links) {
        free(new_node->keys);
        free(new_node);
        *error = ENOMEM;
        return NULL;
    }

    return new_node;
}

void split_child(node_t* parent, node_t* child, int index, int B, int* error) {
    node_t* new_node = create_node(B, child->is_leaf, error);
    if (*error)
        return;

    new_node->count = B - 1;

    memmove(new_node->keys, &child->keys[B], (B - 1) * sizeof(int));

    if (!child->is_leaf)
        memmove(new_node->links, &child->links[B], B * sizeof(node_t*));

    child->count = B - 1;

    memmove(&parent->links[index + 2], &parent->links[index + 1], (parent->count - index) * sizeof(node_t*));
    parent->links[index + 1] = new_node;

    memmove(&parent->keys[index + 1], &parent->keys[index], (parent->count - index) * sizeof(int));
    parent->keys[index] = child->keys[B - 1];

    parent->count++;
}

int get_pos(int count, int* keys, int key) {
    int start = 0;
    int end = count;
    int pos = (start + end) / 2;
    
    while (end - start > 1) {
        pos = (start + end) / 2;
        if (key < keys[pos]) 
            end = pos;
        else
            start = pos;
    }

    if (key > keys[pos])
        return pos+1;
    return pos;
}

void insert_non_full(node_t* node, int key, int B, int* error) {
    int pos = get_pos(node->count, node->keys, key);
    if (node->is_leaf) {
        memmove(&node->keys[pos + 1], &node->keys[pos], (node->count - pos) * sizeof(int));
        node->keys[pos] = key;
        node->count++;
    } else {
        if (node->links[pos]->count == 2 * B - 1) {
            split_child(node, node->links[pos], pos, B, error);
            if (*error)
                return;

            if (key > node->keys[pos]) {
                pos++;
            }
        }
        insert_non_full(node->links[pos], key, B, error);
        if (*error)
            return;
    }
}

node_t* insert(node_t* rootNode, int B, int key, int* error) {
    if (rootNode == NULL) {
        rootNode = create_node(B, true, error);
        if (*error)
            return rootNode;
        rootNode->keys[0] = key;
        rootNode->count = 1;
        return rootNode;
    }

    if (rootNode->count == 2 * B - 1) {
        node_t* new_root = create_node(B, false, error);
        if (*error)
            return rootNode;

        new_root->links[0] = rootNode;
        split_child(new_root, rootNode, 0, B, error);
        if (*error)
            return rootNode;

        insert_non_full(new_root, key, B, error);
        if (*error)
            return rootNode;

        return new_root;
    } else
        insert_non_full(rootNode, key, B, error);

    return rootNode;
}

void free_tree(node_t* node) {
    if (node == NULL)
        return;

    if (!node->is_leaf)
        for (int i = 0; i <= node->count; ++i)
            free_tree(node->links[i]);

    free(node->keys);
    free(node->links);
    free(node);
}

int get_height(node_t* node) {
    if (node == NULL) return 0;
    if (node->is_leaf) return 1;
    return 1 + get_height(node->links[0]);
}

int main() {
    FILE* inStream = fopen("in.txt", "r");
    if (!inStream) return -1;

    int B;
    if (fscanf(inStream, "%d", &B) != 1) {
        fclose(inStream);
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

    int error = 0;
    node_t* rootNode = NULL;

    for (int i = 0; i < N; i++) {
        int input;
        if (fscanf(inStream, "%d", &input) != 1) {
            free_tree(rootNode);
            fclose(inStream);
            return -1;
        }

        rootNode = insert(rootNode, B, input, &error);
        if (error) {
            free_tree(rootNode);
            fclose(inStream);
            return error;
        }
    }

    printf("%d\n", get_height(rootNode));

    free_tree(rootNode);
    fclose(inStream);
    return 0;
}
