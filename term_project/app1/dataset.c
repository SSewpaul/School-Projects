#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "dataset.h"
#define MAXSTUD 3000

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


STUDENT *createDataSet()
{
	STUDENT *sp=(STUDENT *)malloc(sizeof(STUDENT)*12);
	for(int i=0;i<13;i++)
	{
		sp[i].age=i+18;
		sp[i].id=(LIST *)malloc(sizeof(LIST));
		(sp[i].id)->head=malloc(sizeof(NODE));
		(sp[i].id)->head->next=(sp[i].id)->head;
		(sp[i].id)->count=0;
	}
	return sp;
}

void destroyDataSet(STUDENT *sp)
{
	for(int i=0;i<13;i++)
	{
		NODE *pDel=(sp[i].id)->head->next;
		for(int j=0;j<sp[i].id->count;j++)
		{
			(sp[i].id)->head->next=pDel->next;
			free(pDel);
			pDel=(sp[i].id)->head->next;
		}
	}
}

void insertion(STUDENT *sp,int age,int ID)
{
	NODE *temp=(NODE *)malloc(sizeof(NODE));
	temp->data=ID;
	temp->next=(sp[age-18].id)->head->next;
	(sp[age-18].id)->head->next=temp;
	((sp[age-18].id)->count)++;
}

void deletion(STUDENT *sp,int age)
{
	if(sp[age-18].id->count==0)
	{
		printf("Deletion unsuccessful\n");
	}
	else
	{
		int i=sp[age-18].id->count;
		NODE *pDel=(sp[age-18].id)->head->next;
                for(int j=0;j<i;j++)
                {
			printf("successfully deleted: ID:%d  Age:%d",pDel->data,age);
                        (sp[age-18].id)->head->next=pDel->next;
                        free(pDel);
                        pDel=(sp[age-18].id)->head->next;
			(sp[age-18].id)->count--;
		}
	}
}

void searchAge(STUDENT *sp,int age)
{
	if(sp[age-18].id->count==0)
	{
		printf("No one of has age %d\n",age);
		return;
	}
	NODE *pTrav=(sp[age-18].id)->head->next;
	for(int i=0;i<sp[age-18].id->count;i++)
	{
		printf("Found: ID:%d    Age:%d\n",pTrav->data,age);
		pTrav=pTrav->next;
	}
}

void maxAgeGap(STUDENT *sp)
{
	int min=30;
	int max=18;
	int gap=0;
	for(int i=0;i<13;i++)
	{
		if(sp[i].id->count!=0)
		{
			if(sp[i].age<min&&sp[i].age>0)
			{
				min=sp[i].age;
			}
			if(sp[i].age>max)
			{
				max=sp[i].age;
			}
		}
	}
	gap=max-min;
	printf("Max Age Gap:%d\n",gap);
}
