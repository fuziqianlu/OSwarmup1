#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "my402list.h"


typedef struct transaction {
	long timestamp;
	char* description;
	long amount;
	int isAdd;
} MyTransaction;

My402ListElem* CompareAndInsert(My402List* plist, My402ListElem* toInsert){
	My402ListElem* cur = My402ListPrev(plist, toInsert);
	My402ListElem* elemNext = My402ListNext(plist, toInsert);
	My402ListUnlink(plist, toInsert);
	MyTransaction* transToInsert = (MyTransaction*)toInsert->obj;
	free(toInsert);
	while(cur!=NULL){
		MyTransaction* curTrans = (MyTransaction*)cur->obj;
		if(curTrans->timestamp > transToInsert->timestamp){
			cur = My402ListPrev(plist, cur);
		}
		else{
			break;
		}
	}
	if(cur == NULL){
		(void)My402ListPrepend(plist, transToInsert);
	}
	else{
		(void)My402ListInsertAfter(plist, transToInsert, cur);
	}
	return elemNext;
}

long StringToLong(char* num){
	int len = strlen(num), i;
	long res = 0;
	for(i=0;i<len;i++){
		if(num[i] == '.') continue;
		res = res*10 + (num[i] - '0');
	}
	return res;
}

void InsertSort(My402List* plist){
	int num_items = plist->num_members;
	int i;
	My402ListElem* nextNode = My402ListFirst(plist);
	for(i=0;i<num_items;i++){
		if(nextNode == NULL){
			printf("%s\n", "LinedList Problem!");
		}
		nextNode = CompareAndInsert(plist, nextNode);
	}
}

char* buildAmount(long amount, int isAdd){
	char printAmount[15] = "              ";
	int i;
	printAmount[12] = '0';
	printAmount[11] = '0';
	printAmount[10] = '.';
	printAmount[9] = '0';
	if(isAdd == 0) {
		printAmount[0] = '(';
		printAmount[13] = ')';
	}
	if(amount>=10000000){
		for(i=12;i>=4;i--){
			if(i==10) continue;
			printAmount[i] = '?';
		}
		return printAmount;
	}
	for(i=12;i>=4;i--){
		if(amount==0) break;
		if(i==10) continue;
		char digit = '0' + amount%10;
		printAmount[i] = digit; 
		amount = amount/10;
	}
	return printAmount;
}

void sort(char *tfile){
	FILE *fp = NULL;
	int len = 1026;
	if(tfile!=NULL){
		fp = fopen(tfile, "r+");
	}
	else{
		fp = stdin;
	}
	
	if(fp==NULL){
		printf("%s\n", "cannot find file.");
		return;
	}
	
	printf("%s", "file opened\n");	
	
	My402List* trans = (My402List*)malloc(sizeof(My402List));
	(void)My402ListInit(trans);

	char line[1024];
	while(fgets(line, len, fp) != NULL){
		printf("%s\n", line);
		size_t size = sizeof(line);
		if(size>1024){
			exit(1);
		}
		char *signal = strtok(line, "\t");
		char *timestamp = strtok(NULL, "\t");
		char *amount = strtok(NULL, "\t");
		char *desc = strtok(NULL, "\t");
		//printf("%s\n", "finish split");
		MyTransaction* newTrans = (MyTransaction*)malloc(sizeof(MyTransaction));
		if(signal[0]=='+'){
			newTrans->isAdd = TRUE;
		}
		else{
			newTrans->isAdd = FALSE;
		}
		newTrans->timestamp = atol(timestamp);
		newTrans->amount = StringToLong(amount);
		newTrans->description = desc;
		(void)My402ListAppend(trans, newTrans);
	}
	InsertSort(trans);
	long balance = 0;
	My402ListElem* cur = My402ListFirst(trans);
	char* header1 = "+-----------------+--------------------------+----------------+----------------+";
	char* header2 = "|       Date      | Description              |         Amount |        Balance |";
	char* header3 = "+-----------------+--------------------------+----------------+----------------+";
	printf("%s\n", header1);
	printf("%s\n", header2);
	printf("%s\n", header3);
	while(cur != NULL){
		MyTransaction* p = (MyTransaction*) cur->obj;
		char line[81] = "| ";
		if(p->isAdd){
			balance += p->amount;
		}
		else{
			
			balance -= p->amount;
		}
		time_t date = (time_t)p->timestamp;
		char* dateString  = ctime(&date);
		if(dateString != NULL){
			dateString[strlen(dateString) - 1] = '\0';
		}
		strcat(line, dateString);
		strcat(line, " | ");
		char* amountString = buildAmount(p->amount, p->isAdd);
		
		/*char* balanceString = NULL;
		if(balance<0){
			balanceString = buildAmount(-balance, 0);
		}
		else{
			balanceString = buildAmount(balance, 1);
		}*/
		//char descString[25];
		//strncpy(descString, p->description, sizeof(descString));
		//descString[24] = '\0';
		//strcat(line, descString);
		strcat(line, " | ");
		strcat(line, amountString);
		//strcat(line, " | ");
		//strcat(line, balanceString);
		strcat(line, " |");
		//printf("%s\n", line);
		printf("%s\t%d\t%d\n", dateString, p->amount, balance);
		cur = My402ListNext(trans, cur);

	}
	printf("%s", header3);
}

int main(){
	sort("test.txt");
	return 0;
}
