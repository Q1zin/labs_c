#include <stdio.h>
#include <stdlib.h>
#define NOT_VISITED 0
#define IN_PROGRESS 1
#define VISITED 2

typedef struct Node {
  int value;
  struct Node* next;
} Node;

typedef struct Stack {
  Node* top;
} Stack;

Stack createStack() {
  Stack new_stack = { NULL };
  return new_stack;
}

void push(Stack* s, int value) {
  Node* new_node = (Node*)malloc(sizeof(Node));
  if (new_node == NULL) {
    return;
  }
  if (s->top) {
    new_node->value = value;
    new_node->next = s->top;
    s->top = new_node;
  } else {
    new_node->value = value;
    new_node->next = NULL;
    s->top = new_node;
  }
}

int pop(Stack* s) {
  if (s->top == NULL) {
    return -1;
  }
  Node* tmp_node = s->top;
  int value = tmp_node->value;
  s->top = s->top->next;
  free(tmp_node);
  return value;
}

void freeStack(Stack* s) {
  Node* current = s->top;
  while (current != NULL) {
    Node* tmp_node = current;
    current = current->next;
    free(tmp_node);
  }
}

char* createMatrix(int N) {
  const int size = N * N / 8 + 1;
  char* new_matrix = (char*)calloc(1, size);

  if (new_matrix == NULL) {
    return NULL;
  }
  return new_matrix;
}

void pushEdgeIntoMatrix(char matrix[], int N, int start, int end) {
  int byte = (start * N - (N - end) - 1) / 8;
  char bit;
  if ((8 - ((start * N - (N - end)) % 8)) == 8){
    bit = 1;
  } else {
    bit = 1 << (8 - ((start * N - (N - end)) % 8));
  }
  
  matrix[byte] = matrix[byte] | bit;
}

int checkEdge(char matrix[], int N, int start, int end) {
  int byte = (start * N - (N - end) - 1) / 8;
  char bit;
  if ((8 - ((start * N - (N - end)) % 8)) == 8){
    bit = 1;
  } else {
    bit = 1 << (8 - ((start * N - (N - end)) % 8));
  }
  
  return matrix[byte] & bit;
}

int DFS(char matrix[], Stack* sorted, int vertex, int N, char* color) {
  int flag_to_sort = 1;

  color[vertex] = IN_PROGRESS;
  for (int i = N; i > 0; --i) {
    if (i != vertex && color[i] == IN_PROGRESS && checkEdge(matrix, N, vertex, i)) {
      return 0;
    }
    if (i != vertex && color[i] == NOT_VISITED && checkEdge(matrix, N, vertex, i)) {
      flag_to_sort = DFS(matrix, sorted, i, N, color);
    }
  }

  color[vertex] = VISITED;
  push(sorted, vertex);
  
  return flag_to_sort;
}

void printTopologicalSort(char matrix[], int N) {
  char* color = calloc(N + 1, sizeof(char));

  if (color == NULL) {
    return;
  }

  Stack sorted = createStack();
  for (int i = 1; i <= N; ++i) {
    if (color[i] == NOT_VISITED) {
      if (DFS(matrix, &sorted, i, N, color) == 0) {
        printf("impossible to sort");
        freeStack(&sorted);
        free(color);
        return;
      }
    }
  }

  while (sorted.top) {
    printf("%d ", sorted.top->value);
    pop(&sorted);
  }

  free(color);
}

int main() {
  FILE* inStream = fopen("in.txt", "r");
  if (!inStream) {
    return 0;
  }

  int N;
  if (fscanf(inStream, "%d", &N) != 1) {
      fclose(inStream);
      return 0;
  }

  if (N < 0 || N > 2000) {
    printf("bad number of vertices");
    fclose(inStream);
    return 0;
  }

  int M;
  if (fscanf(inStream, "%d", &M) != 1) {
      printf("bad number of lines");
      fclose(inStream);
      return 0;
  }

  if (M < 0 || M > N * (N - 1) / 2) {
    printf("bad number of edges");
    fclose(inStream);
    return 0;
  }

  char* matrix = createMatrix(N);

  for (int i = 0; i < M; ++i){
    int start, end;
    if (fscanf(inStream, "%d %d", &start, &end) != 2) {
      printf("bad number of lines");
      fclose(inStream);
      free(matrix);
      return 0;
    }

    if ((!(start >= 1 && start <= N)) || (!(end >= 1 && end <= N))){
      printf("bad vertex");
      fclose(inStream);
      free(matrix);
      return 0;
    }

    pushEdgeIntoMatrix(matrix, N, start, end);
  }

  printTopologicalSort(matrix, N);
  fclose(inStream);
  free(matrix);
  return 0;
}
