#ifndef LEAKTEST_H
#define LEAKTEST_H

void simpleLeak();
void complexLeak();

int freeingIsMyJob();
int* freeingIsYourJob();

typedef struct Node {
    int data;
    struct Node* next;
} Node;

Node *createNode(int value);
Node *addNode(Node *head, Node *node);
void deleteNode(Node *head, int value);

#endif