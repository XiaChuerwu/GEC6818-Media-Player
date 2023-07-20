#ifndef __LINKLIST_PIC_H__
#define __LINKLIST_PIC_H__

typedef struct node{
    char* data;   //本来要保存的数据，称之为数据域
    struct node* next;   //后继元素的的地址，称之为指针域
    struct node* prev;
}Node;

typedef struct linklist{
    Node* first;//保存链表的头节点地址
    Node* last;//保存链表最后一个节点的地址
    int num;//保存链表元素个数
}Linklist;
//管理节点

/*
    功能：创建一个空链表(没有一个元素节点，只有一个管理节点)
*/
Linklist* create_linklist();

void insert_linklist(Linklist* linklist, const char * data);

void print_linklist(Linklist* linklist);

void free_linklist(Node* first);

Node* search_linklist(Linklist* linklist, const char * data);

void free_linklinst(Linklist* linklist);

void destroy_linklist(Linklist* linklist);

#endif