#include <stdio.h>
#include <stdlib.h>

struct Node{
    int y;
    int x;
    struct Node *next;
};

void insert_node(struct Node **head, int y, int x){
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    new_node->y = y;
    new_node->x = x;
    new_node->next = *head;
    *head = new_node;
}

void delete_start(struct Node **head){
    if(*head == NULL)
        return;
    struct Node *temp = *head;
    *head = (*head)->next;
    free(temp);
}

void free_list(struct Node *head){
    if(head == NULL)
        return;
    while(head->next != NULL){
        struct Node *temp = (head->next)->next;
        free(head->next);
        head->next = temp;
    }
    free(head);
}

void delete_next(struct Node *n){
    if(n == NULL)
        return;
    if(n->next == NULL)
        return;
    struct Node *temp = n->next;
    n->next = temp->next;
    free(temp);
}
