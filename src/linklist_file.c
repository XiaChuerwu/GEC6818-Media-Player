#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linklist_file.h"
/*
    功能：创建一个空链表(没有一个元素节点，只有一个管理节点)
    返回值:
        管理节点的地址
*/

Linklist* create_linklist(){
    //给管理节点分配空间
    Linklist* linklist = (Linklist*)malloc(sizeof(Linklist));
    linklist->first = NULL;
    linklist->last = NULL;
    linklist->num = 0;    
    return linklist;
}

/*
    功能：往链表中插入数据/节点
    参数：
        linklist    该链表管理节点的地址(相当于多个数据)
        data        要插入的数据
*/
void insert_linklist(Linklist* linklist, const char * data){
    //为新节点分配空间
    Node* New = (Node*)malloc(sizeof(Node));

    //赋值
    New->data = (char *)malloc(strlen(data) + 1);
    strcpy(New->data,data);
    New->next = NULL;
    New->prev = NULL;

    //把新节点插入到链表中

    if(linklist->first == NULL){//如果当前链表为空
        //当前插入的元素是第一个元素
        linklist->first = New;
        linklist->last = New;
        linklist->num ++;
    } else {
        #if 0
        //头插法
        New->next = linklist->first;
        linklist->first = New;

        #else
        //尾插法
        linklist->last->next = New;
        New->prev = linklist->last;
        linklist->last = New;
        #endif
        linklist->num ++;
    }
    linklist->last->next = linklist->first;
    linklist->first->prev = linklist->last;
}

/*
    功能：打印链表所有元素
    参数：linklist
        
*/
void print_linklist(Linklist* linklist){
    #if 0
    Node* head = linklist->first;
    if(linklist->num == 0){
        printf("该链表没有元素\n");
        return;
    }
    while(head){
        printf("%d ",head->data);
        head = head->next;
    }
    printf("\n");
    #else
    Node* last = linklist->first;
    if(linklist->num == 0){
        printf("该链表没有元素\n");
        return;
    }
    // while(last){
    //     printf("%s ",last->data);
    //     last = last->prev;
    // }
    for(int i = 0; i < linklist->num; i++){
        printf("%s\n",last->data);
        last = last->next;
    }
    printf("\n");
    #endif
}

/*
    功能：查找链表中是否存在某个元素

*/
Node* search_linklist(Linklist* linklist, const char * data){
    Node* p = linklist->first;
    while(p){
        int len_data = strlen(p->data);
        int len_photo = strlen(data);
        //查找任意图片
        if(strcmp(p->data + len_data - len_photo,data) == 0){
            return p;
        }
        p = p->next;
    }
    return NULL;
}

/*
    功能：清空一个链表，把所有的节点都释放只剩下管理节点
*/
void free_linklinst(Linklist* linklist){
    Node* p = linklist->first;
    while(linklist->num > 0){
        Node* temp = p;
        p = p->next;
        temp->next = NULL;
        temp->prev = NULL;
        free(temp);
        //printf("linklist->num = %d\n",linklist->num);
        linklist->num--;
        
    }
    
    linklist->first = NULL;
    linklist->last = NULL;
}

/*
    功能：销毁一个链表，把所有的节点都释放
*/
void destroy_linklist(Linklist* linklist){
    free_linklinst(linklist);
    free(linklist->first);
    free(linklist->last);
}

