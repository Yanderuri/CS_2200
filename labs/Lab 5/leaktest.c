#include <stdio.h>
#include <stdlib.h>
#include "leaktest.h"

/**
Use Valgrind to find the memory leak in this program. First start with the simple leak, then
move on to the complex leak. You will need to compile with the -g flag to get line numbers.
*/
int main() {
	simpleLeak();
	// complexLeak();

	return 0;
}

void simpleLeak() {
	printf("Welcome to my simple program!\n");

	int return1 = freeingIsMyJob();
	printf("return1: %d\n", return1);

	int *return2 = freeingIsYourJob();
	printf("return2: %d\n", *return2);
	int myNumber = 6;
	*return2 = myNumber;
	printf("return2 after modification: %d\n", *return2);
	printf("Done!");
}

int freeingIsMyJob() {
	int *t = malloc(sizeof(int));
	*t = 4;
	int value = *t;
	free(t);
	return value;
}

int* freeingIsYourJob() {
	int *t = malloc(sizeof(int));
	*t = 5;
	return t;
}

void complexLeak() {
	printf("Welcome to my complex program!\n");

	Node *head = NULL;
	for (int i = 0; i < 10; i++) {
		Node *n = createNode(i);
		head = addNode(head, n);
	}

	int valueToRemove = 5;
	printf("Removing %d from the list.\n", valueToRemove);
	deleteNode(head, valueToRemove);
	printf("Done!");
}

Node *createNode(int value) {
	Node *n = malloc(sizeof(Node));
	n->data = value;
	n->next = NULL;
	return n;
}

Node *addNode(Node *head, Node *node) {
	if (head == NULL) {
		return node;
	}
	Node *curr = head;
	while (curr->next != NULL) {
		curr = curr->next;
	}
	curr->next = node;
	return head;
}

void deleteNode(Node *head, int value) {
	Node *curr = head;
	while (curr != NULL) {
		if (curr->data == value) {
			Node *temp = curr->next;
			free(curr);
			return temp;
		}

		curr = curr->next;
	}
}