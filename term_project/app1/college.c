#include <stdio.h>
#include "dataset.h"
#include <time.h>
#include <stdlib.h>
	
typedef struct student
{
        int age;
        struct list *id;
}STUDENT;

typedef struct list
{
        struct node *head;
        int count;
}LIST;

typedef struct node
{
        struct node *next;
        int data;
}NODE;

int numgen(int prev)
{
	prev+=rand()%2+1;
	return prev;
}
int main()
{
	STUDENT *sp=createDataSet();
	srand(time(NULL));
	int id=rand()%2000+1;
	int age=rand()%13+18;
	printf("age:%d",age);
	printf("id:%d",id);
	insertion(sp,age,id);
	for(int i=0;i<999;i++)
	{	
		id=numgen(id);
		age=rand()%13+18;
		insertion(sp,age,id);
	}
	maxAgeGap(sp);
	for(int i=0;i<10;i++)
        {
                int target=rand()%13+18;
                searchAge(sp,target);
        }	
	for(int i=0;i<10;i++)
        {       
                int target=rand()%13+18;
                deletion(sp,target);
        }
	for(int i=0;i<10;i++)
        {
                int target=rand()%13+18;
                searchAge(sp,target);
        }

	maxAgeGap(sp);
	destroyDataSet(sp);
}
	

