#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "dataset.h"
#define MAXSIZE 2999

typedef struct student
{
	int id;
	int age;
	char flag;
}STUDENT;

STUDENT *createDataSet()
{
	STUDENT *sp;
	sp=(STUDENT *)malloc(sizeof(STUDENT)*MAXSIZE);
	for(int i=0;i<MAXSIZE;i++)
	{
		sp[i].flag='E';
	}
	return sp;
}

void destroyDataSet(STUDENT * sp)
{
	free(sp);
}

static int hash(STUDENT *sp,int val)
{
	int i=0;
	int h;
	h=(val+i)%MAXSIZE;
	return h;
}

void searchID(STUDENT *sp,int targetID)
{
	int location=hash(sp,targetID);
	while(sp[location].flag=='F')
	{
		if(sp[location].id==targetID)
		{
			printf("Successfully found ID:%d age:%d\n",sp[location].id,sp[location].age);	
			return;
		}
		location++;
	}
	printf("ID not found\n");
}

void insertion(STUDENT *sp,int age, int ID)
{
	int location=hash(sp,ID);
	while(sp[location].flag=='F')
	{
		location++;
	}
	sp[location].id=ID;
	sp[location].age=age;
	sp[location].flag='F';
}

void deletion(STUDENT *sp,int targetID)
{
	int location=hash(sp,targetID);
	while(sp[location].flag=='F')
        {
                if(sp[location].id==targetID)
                {
                        printf("Successfully deleted ID:%d age:%d\n",sp[location].id,sp[location].age);
                        sp[location].flag='D';
			return;
                }
                location++;
        }
	printf("Cannot delete this ID\n");
}
