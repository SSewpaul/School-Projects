#ifndef DATASET_H_
#define DATASET_H_

typedef struct student STUDENT;

typedef struct list LIST;

typedef struct node NODE;

STUDENT *createDataSet();

void destroyDataSet(STUDENT *sp);

void searchAge(STUDENT *sp,int age);

void maxAgeGap(STUDENT *sp);

void insertion(STUDENT *sp,int age, int ID);

void deletion(STUDENT *sp,int ID);
#endif

