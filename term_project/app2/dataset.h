#ifndef DATASET_H_
#define DATASET_H_

typedef struct student STUDENT;

STUDENT *createDataSet();

void destroyDataSet(STUDENT *sp);

void searchID(STUDENT * sp,int targetID);

void insertion(STUDENT *sp,int age,int ID);

void deletion(STUDENT *sp,int targetID);

#endif
