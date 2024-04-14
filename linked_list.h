#include <stdio.h>
#include <stdlib.h>
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

struct Node{
    int y;
    int x;
    struct Node *next;
};

#endif

void insert_node(struct Node **head, int y, int x);

void delete_start(struct Node **head);

void free_list(struct Node *head);

void delete_next(struct Node *n);

void reverse_list(struct Node **head);
