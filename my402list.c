#include <stdio.h>
#include <stdlib.h>
#include "my402list.h"

My402ListElem *My402ListFirst(My402List* clist){
	if(clist->anchor.next == &(clist->anchor)) return NULL;
	return clist->anchor.next;
}

My402ListElem *My402ListLast(My402List* clist){
	if(clist->anchor.prev == &(clist->anchor)) return NULL;
	return clist->anchor.prev;
}

My402ListElem *My402ListNext(My402List* clist, My402ListElem* item){
	if(item == NULL) return NULL;
	if(item->next == &(clist->anchor)) return NULL;
	return item->next;
}

My402ListElem *My402ListPrev(My402List* clist, My402ListElem* item){
	if(item == NULL) return NULL;
	if(item->prev == &(clist->anchor)) return NULL;
	return item->prev;
}

My402ListElem *My402ListFind(My402List* clist, void* obj){
	if(clist == NULL) return NULL;
	My402ListElem* p = clist->anchor.next;
	while(p != &(clist->anchor)){
		if(p->obj == obj) return p;
		p = p->next;
	}
	return NULL;
}

int My402ListInit(My402List* clist){
	clist->num_members = 0;
	(clist->anchor).next = &(clist->anchor);
	(clist->anchor).prev = &(clist->anchor);
	return 1;
}

int  My402ListLength(My402List* clist){
	if(clist == NULL) return 0;
	return clist->num_members;
}

int  My402ListEmpty(My402List* clist){
	if(clist == NULL) return 1;
	if(clist->num_members == 0) return 1;
	else return 0;
}

int  My402ListAppend(My402List* clist, void* obj){
	if(clist == NULL) return 0;
	My402ListElem *last = My402ListLast(clist);
	if(last == NULL) last = &(clist->anchor);
	struct tagMy402ListElem *newNode = NULL;
	newNode = (My402ListElem*)malloc(sizeof(My402ListElem));
	if(newNode == NULL) return 0;
	newNode->obj = obj;
	newNode->next = &(clist->anchor);
	(clist->anchor).prev = newNode;
	last->next = newNode;
	newNode->prev = last;
	clist->num_members++;
	return 1;
}

int  My402ListPrepend(My402List* clist, void* obj){
	if(clist == NULL) return 0;
	My402ListElem* first = My402ListFirst(clist);
	if(first == NULL) first = &(clist->anchor);
	struct tagMy402ListElem* newNode = (My402ListElem*)malloc(sizeof(My402ListElem));
	if(newNode == NULL) return 0;
	newNode->obj = obj;
	newNode->next = first;
	first->prev = newNode;
	clist->anchor.next = newNode;
	newNode->prev = &(clist->anchor);
	clist->num_members++;
	return 1;
}

void My402ListUnlink(My402List* clist, My402ListElem* item){
	if(&(clist->anchor) == item) return;
	My402ListElem* p = item->prev;
	My402ListElem* r = item->next;
	if(p != NULL) p->next = r;
	if(r != NULL) r->prev = p;
	item->prev = NULL;
	item->next = NULL;
	clist->num_members--;
}

void My402ListUnlinkAll(My402List* clist){
	if(clist == NULL) return;
	My402ListElem* p = &(clist->anchor);
	My402ListElem* r = &(clist->anchor);
	p = p->next;
	while(p != NULL){
		r->next = NULL;
		p->prev = NULL;
		r = p;
		p = p->next;
	}
	My402ListInit(clist);
}

int  My402ListInsertAfter(My402List* clist, void* obj, My402ListElem* pos){
	if(clist == NULL) return 0;
	if(pos == NULL) return My402ListAppend(clist, obj);
	My402ListElem* nextNode = pos->next;;
	struct tagMy402ListElem* newNode = (My402ListElem*)malloc(sizeof(My402ListElem));
	if(newNode == NULL) return 0;	
	newNode->obj = obj;
	pos->next = newNode;
	newNode->prev = pos;
	newNode->next = nextNode;
	nextNode->prev = newNode;
	clist->num_members++;
	return 1;
}

int  My402ListInsertBefore(My402List* clist, void* obj, My402ListElem* pos){
	if(clist == NULL) return 0;
	if(pos == NULL) return My402ListPrepend(clist, obj);
	My402ListElem* prevNode = pos->prev;;
	struct tagMy402ListElem* newNode = (My402ListElem*)malloc(sizeof(My402ListElem));
	if(newNode == NULL) return 0;	
	newNode->obj = obj;
	prevNode->next = newNode;
	newNode->prev = prevNode;
	newNode->next = pos;
	pos->prev = newNode;
	clist->num_members++;
	return 1;
}


