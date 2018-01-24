#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "my402list.h"

typedef struct transaction {
	time_t timestamp;
	char description[25];
	long amount;
	int isAdd;
} MyTransaction;

My402ListElem* CompareAndInsert(My402List* plist, My402ListElem* toInsert){
	My402ListElem* cur = My402ListPrev(plist, toInsert);
	My402ListElem* elemNext = My402ListNext(plist, toInsert);
	MyTransaction* transToInsert = (MyTransaction*)toInsert->obj;
	My402ListUnlink(plist, toInsert);
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

long AmountStringToLong(char* num){
	int len = strlen(num), i;
	int valid = 1;
	long res = 0;
	for(i=0;i<len;i++){
		if(num[i] == '.'){
			if(i == len - 3 && i != 0) continue;
			else{
				valid = 0;
				break;
			}
		}
		if(!isdigit(num[i])){
			valid = 0;
			break;
		}
		res = res*10 + (num[i] - '0');
	}
	if(!valid){
		fprintf(stderr, "%s\n", "Format error with amount");
		exit(-1);
	}
	return res;
}

time_t TimestampStringToTimeT(char* timestamp){
	int i;
	if(strlen(timestamp)>10){
		fprintf(stderr, "%s\n", "Timestamp input is too long!");
		exit(-1);
	}
	time_t res = 0;
	for(i=0;i<strlen(timestamp);i++){
		if(!isdigit(timestamp[i])){
			fprintf(stderr, "%s\n", "Timestamp input contains non-digit elements!");
			exit(-1);
		}
		res = res*10 + (timestamp[i] - '0');
	}
	return res;
}

void InsertSort(My402List* plist){
	int num_items = plist->num_members;
	int i;
	My402ListElem* nextNode = My402ListFirst(plist);
	for(i=0;i<num_items;i++){
		if(nextNode == NULL){
			fprintf(stderr, "%s\n", "LinedList's length is shorter than expected!");
		}
		nextNode = CompareAndInsert(plist, nextNode);
	}
}

char* formatNum(long amount, int isAdd){
	char *printAmount = (char*)malloc(15);
	memset(printAmount, ' ', 14);
	printAmount[14] = '\0';
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
		if(amount == 0) break;
		if(i == 10) continue;
		char digit = '0' + amount%10;
		printAmount[i] = digit; 
		amount = amount/10;
	}
	return printAmount;
}

char* formatDate(time_t timestamp){
	char *res = (char*)malloc(16);
	char* dateString  = ctime(&timestamp);
	if(dateString != NULL){
		dateString[strlen(dateString) - 1] = '\0';
	}
	//char date[11];
	strncpy(res, dateString, 11);
	char year[5];
	strncpy(year, dateString + 20, 4);
	strcat(res, year);
	res[15] = '\0';
	return res;
}

char* formatDesc(char* desc){
	int i;
	char* res = (char*)malloc(25);
	strncpy(res, desc, 24);
	for(i = strlen(res);i<24;i++){
		res[i] = ' ';
	}
	res[24] = '\0';
	return res;
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
		fprintf(stderr, "%s\n", "cannot find file.");
		exit(-1);
		return;
	}
	
	My402List* trans = (My402List*)malloc(sizeof(My402List));
	if(trans == NULL){
		fprintf(stderr, "%s\n", "malloc linkedlist failed.");
		exit(-1);
	}
	(void)My402ListInit(trans);

	char line[1024];
	while(fgets(line, len, fp) != NULL){
		size_t size = sizeof(line);
		if(size>1024){
			exit(-1);
		}
		char *signal = strtok(line, "\t");
		char *timestamp = strtok(NULL, "\t");
		char *amount = strtok(NULL, "\t");
		char *desc = strtok(NULL, "\t");
		MyTransaction* newTrans = (MyTransaction*)malloc(sizeof(MyTransaction));
		if(signal == NULL || strlen(signal) != 1){
			fprintf(stderr, "%s\n", "The length sign in input is invalid!");
			exit(-1);
		}
		if(signal[0]=='+'){
			newTrans->isAdd = TRUE;
		}
		else if(signal[0] == '-'){
			newTrans->isAdd = FALSE;
		}
		else{
			fprintf(stderr, "%s\n", "The sign in input is invalid!");
			exit(-1);
		}
		newTrans->timestamp = TimestampStringToTimeT(timestamp);
		newTrans->amount = AmountStringToLong(amount);
		desc[strlen(desc)-1] = '\0';
		strncpy(newTrans->description, desc, 24);
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
		char* dateString = formatDate(p->timestamp);

		strcat(line, dateString);
		strcat(line, " | ");
		char* amountString = NULL;
		amountString = formatNum(p->amount, p->isAdd);
		
		char* balanceString = NULL;
		if(balance<0){
			balanceString = formatNum(-balance, 0);
		}
		else{
			balanceString = formatNum(balance, 1);
		}
		
		char* descString = formatDesc(p->description);
		strcat(line, descString);
		strcat(line, " | ");
		strcat(line, amountString);
		strcat(line, " | ");
		strcat(line, balanceString);
		strcat(line, " |");
		printf("%s\n", line);
		cur = My402ListNext(trans, cur);

	}
	printf("%s\n", header3);
	fclose(fp);
}

int main(int argc, char* argv[]){
	sort("test.txt");
	return 0;
}
