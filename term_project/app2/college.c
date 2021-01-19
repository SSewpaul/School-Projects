#include <stdio.h>
#include "dataset.h"
#include <time.h>
#include <stdlib.h>

typedef struct student
{
        int id;
        int age;
        char flag;
}STUDENT;

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
        insertion(sp,age,id);
        for(int i=0;i<999;i++)
        {
                id=numgen(id);
                age=rand()%13+18;
                insertion(sp,age,id);
        }
	for(int i=0;i<10;i++)
	{
		int target=rand()%2000+1;
        	searchID(sp,target);
        }
	for(int i=0;i<10;i++)
        {
                int target=rand()%2000+1;
                deletion(sp,target);
        }
        for(int i=0;i<10;i++)
        {
                int target=rand()%2000+1;
                searchID(sp,target);
        }
        destroyDataSet(sp);
}

	
