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
		else if(curTrans->timestamp == transToInsert->timestamp){
			fprintf(stderr, "%s\n", "Two same timestamps exist!");
			exit(-1);
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

int TypeToInt(char* type, int lineNumber){
	int res = TRUE;
	if(type == NULL || strlen(type) != 1){
		fprintf(stderr, "%d: %s\n", lineNumber, "The length of type in input is invalid!");
		exit(-1);
	}
	if(type[0]=='+'){
		res = TRUE;
	}
	else if(type[0] == '-'){
		res = FALSE;
	}
	else{
		fprintf(stderr, "%d: %s\n", lineNumber, "The type in input is invalid!");
		exit(-1);
	}
	return res;
}

char* CutInputDesc(char* desc, int lineNumber){
	char* res = (char*)malloc(25);
	if(desc == NULL){
		fprintf(stderr, "%d: %s\n", lineNumber, "Description cannot be empty!");
		exit(-1);
	}
	int i = 0;
	while(i<strlen(desc) && desc[i] == ' ') i++;
	strncpy(res, desc + i, 24);
	if(strlen(res) == 0 || (strlen(res) == 1&& res[strlen(res)-1] == '\n')){
		fprintf(stderr, "%d: %s\n", lineNumber, "Description cannot be empty after removing leading spaces!");
		exit(-1);
	}
	if(strlen(res)>0&&res[strlen(res)-1] == '\n'){
		res[strlen(res)-1] = '\0';
	}
	else{
		res[strlen(res)] = '\0';
	}
	
	return res;
}

long AmountStringToLong(char* num, int lineNumber){
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
		fprintf(stderr, "%d: %s\n", lineNumber, "Format error with amount");
		exit(-1);
	}
	return res;
}

time_t TimestampStringToTimeT(char* timestamp, int lineNumber){
	int i;
	time_t curTime;
	time(&curTime);
	if(strlen(timestamp)>10){
		fprintf(stderr, "%d: %s\n", lineNumber, "Timestamp input is too long!");
		exit(-1);
	}
	time_t res = 0;
	for(i=0;i<strlen(timestamp);i++){
		if(!isdigit(timestamp[i])){
			fprintf(stderr, "%d: %s\n", lineNumber, "Timestamp input contains non-digit elements!");
			exit(-1);
		}
		res = res*10 + (timestamp[i] - '0');
	}
	if(res > curTime){
		fprintf(stderr, "%d: %s\n", lineNumber, "Timestamp is larger than current time!");
		exit(-1);
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

	if(amount >= 1000000000){
		for(i=12;i>=1;i--){
			if(i==10) continue;
			if(i == 6||i == 2){
				printAmount[i] = ',';
				continue;
			}
			printAmount[i] = '?';
		}
		return printAmount;
	}
	for(i=12;i>=1;i--){
		if(amount == 0) break;
		if(i == 10) continue;
		if(i == 6||i == 2){
			printAmount[i] = ',';
			continue;
		}
		char digit = '0' + amount%10;
		printAmount[i] = digit; 
		amount = amount/10;
	}
	return printAmount;
}

char* formatDate(time_t timestamp){
	char *res = (char*)malloc(16);
	char* dateString  = ctime(&timestamp);
	strncpy(res, dateString, 11);
	res[11] = '\0';
	char *year = (char*)malloc(5);
	strncpy(year, dateString + 20, 4);
	year[4] = '\0';
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

My402List* BuildList(char* tfile){
	FILE *fp = NULL;
	int len = 1026;
	if(tfile!=NULL){
		fp = fopen(tfile, "r");
	}
	else{
		fp = stdin;
	}
	
	if(fp==NULL){
		fprintf(stderr, "%s: %s\n", tfile, "No such file or directory!");
		exit(-1);
	}
	
	My402List* trans = (My402List*)malloc(sizeof(My402List));
	if(trans == NULL){
		fprintf(stderr, "%s\n", "malloc linkedlist failed.");
		exit(-1);
	}
	(void)My402ListInit(trans);

	char *line = (char*)malloc(1027);
	int lineNumber = 1;
	while(fgets(line, len, fp) != NULL){
		size_t size = strlen(line);
		if(size>1024){
			fprintf(stderr, "%d: %s\n", lineNumber, "Input line is too long!");
			exit(-1);
		}
		char *signal = strtok(line, "\t");
		if(signal == NULL){
			fprintf(stderr, "%d : %s\n", lineNumber, "Less fields in input than expected!");
			exit(-1);
		}
		char *timestamp = strtok(NULL, "\t");
		if(timestamp == NULL){
			fprintf(stderr, "%d : %s\n", lineNumber, "Less fields in input than expected!");
			exit(-1);
		}
		char *amount = strtok(NULL, "\t");
		if(amount == NULL){
			fprintf(stderr, "%d : %s\n", lineNumber, "Less fields in input than expected!");
			exit(-1);
		}
		char *desc = strtok(NULL, "\t");
		if(desc == NULL){
			fprintf(stderr, "%d : %s\n", lineNumber, "Less fields in input than expected!");
			exit(-1);
		}
		if(strtok(NULL, "\t") != NULL){
			fprintf(stderr, "%d : %s\n", lineNumber, "More fields in input than expected!");
			exit(-1);
		}
		MyTransaction* newTrans = (MyTransaction*)malloc(sizeof(MyTransaction));
		newTrans->isAdd = TypeToInt(signal, lineNumber);
		newTrans->timestamp = TimestampStringToTimeT(timestamp, lineNumber);
		newTrans->amount = AmountStringToLong(amount, lineNumber);
		strncpy(newTrans->description, CutInputDesc(desc, lineNumber), 24);
		(void)My402ListAppend(trans, newTrans);
		lineNumber++;
	}
	fclose(fp);
	return trans;
}

int main(int argc, char* argv[]){
	if(argc<2 || strcmp(argv[1], "sort")!=0 || argc>3){
		fprintf(stderr, "%s\n", "Malformed Command, it should be format like ./warmup1 sort [tfile]");
		exit(-1);
	}
	char* tfile;
	if(argc == 3) tfile = argv[2];
	else tfile = NULL;
	My402List* trans = BuildList(tfile);
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
	return 0;
}
