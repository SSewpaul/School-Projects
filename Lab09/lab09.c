#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

#define min(a,b) ((a) < (b) ? (a) : (b))
#define ceil(x,y) (((x) + (y) - 1) / (y))

typedef struct matrix{
    int nrows;
    int ncols;
    int * data;
    int **  rows;
}matrix;

typedef struct matrix_matrix_args{
   matrix * mat1;
   matrix * mat2;
   matrix ** result;
   int tid;
   int thr_num;
   pthread_mutex_t * mmutex;
}matrix_matrix_args;

typedef struct scalar_args{
    matrix ** mat;
    int tid;
    int thr_num;
    int val;
    pthread_mutex_t * smutex;
}scalar_args;

/**
 * Create an empty matrix with a given # of rows and columns.
 * @param  nrows Number of rows
 * @param  ncols Number of columns
 * @return       Pointer to structure or NULL
 */
matrix * matrix_create(const int nrows, const int ncols)
{
    if(nrows < 1 || ncols < 1){
        return NULL;
    }
    matrix * mat = (matrix *) malloc(sizeof(matrix));
    if(!mat){
        return NULL;
    }
    mat->nrows = nrows;
    mat->ncols = ncols;
    mat->data = (int *) malloc(sizeof(int) * nrows * ncols);
    if(!mat->data){
        // free structure memory since we cannot allocate space for data
        free(mat);
        return NULL;
    }
    mat->rows = (int **) malloc(sizeof(int *) * nrows);
    if(!mat->rows){
        // free structure memory since we cannot allocate space for rows
        free(mat->data);
        free(mat); 
        return NULL;
    }
    int i;
    for(i=0; i < nrows; ++i){
        mat->rows[i] = mat->data + i * ncols;
    }
    return mat;
}

/**
 * Initialize all elements of the matrix with a given value.
 * @param  mat   Matrix to be initialized
 * @param val Value to initialize elements with
 */
void matrix_init(matrix * mat){
    long i;
    for(i=0; i < (long) mat->nrows * mat->ncols; ++i){
        mat->data[i] = rand()%100;
    }
}
   
/**
 * Free matrix data structure
 * @param matp Reference to matrix
 */
void matrix_free(matrix ** matp){
    free((*matp)->rows);
    free((*matp)->data);
    free(*matp);
    *matp = NULL;
}

void print_matrix(matrix *p)
{
    int i,j;
    for(i=0;i<p->nrows;i++)
    {
	for(j=0; j<p->ncols;j++)
	{
       	    printf("%d ",p->data[i*p->ncols+j]);
	}
	printf("\n");
    }
}

//Operations

void *  matrix_matrix_add(void * args){
    matrix_matrix_args * mat=(matrix_matrix_args *)args;
    int bucket_size=ceil((mat->mat1->ncols*mat->mat1->nrows),mat->thr_num);
    int j;
    for(j=(mat->tid)*bucket_size; j<min((mat->tid+1)*bucket_size,(mat->mat1->ncols*mat->mat1->nrows));j++)
    {
	pthread_mutex_lock(mat->mmutex);
	(*mat->result)->data[j]=mat->mat1->data[j]+mat->mat2->data[j];
	pthread_mutex_unlock(mat->mmutex);
    }
    return NULL;
}

double matrix_matrix_op(int num_of_threads,matrix * a,matrix * b,matrix *c){
    if(num_of_threads>1)
    {

        if(a->ncols!=b->ncols||a->nrows!=b->nrows)
        {
       	    printf("Invalid matrices inputted cannot carry out addition\n");
       	    matrix_free(&a);
       	    matrix_free(&b);
    	    return 0;
        }
	pthread_t thr[num_of_threads];
	matrix_matrix_args mat[num_of_threads];
        pthread_mutex_t mmutex=PTHREAD_MUTEX_INITIALIZER;
        clock_t start=clock();
	int i;
        for(i=0;i<num_of_threads;i++)
	{
            mat[i].mat1=a;
            mat[i].mat2=b;
            mat[i].thr_num=num_of_threads;
            mat[i].result=&c;
	    mat[i].tid=i;
	    mat[i].mmutex=&mmutex;
            if(pthread_create(&thr[i],NULL,matrix_matrix_add,(void *)&mat[i])!=0)
	    {
	       printf("could not create thread\n");
               return 0;

            }
	}
        int j;
        for(j=0;j<num_of_threads;j++)
        {
            pthread_join(thr[j],NULL);
        }

	clock_t end=clock();
	matrix_free(&a);
        matrix_free(&b);

	return (double)(end-start)/CLOCKS_PER_SEC;
    }
    else
    {
	if(a->ncols!=b->ncols||a->nrows!=b->nrows)
        {
            printf("Invalid matrices inputted cannot carry out addition\n");
            matrix_free(&a);
            matrix_free(&b);
	    return 0;
        }
	int nrows = a->nrows;
        int ncols = a->ncols;
	clock_t start=clock();
	int i;
	int j;
        for(i=0; i < nrows; ++i)
	{
            for(j=0; j < ncols; ++j)
	    {
                c->data[i * ncols + j] = a->data[i * ncols + j] + b->data[i * ncols + j];
            }
	}
	clock_t end=clock();
	matrix_free(&a);
        matrix_free(&b);
	return (double)(end-start)/CLOCKS_PER_SEC;
    }
}

void * scalar_add(void * args){
    scalar_args * sc=(scalar_args *)args;
    int bucket_size=ceil((*sc->mat)->ncols*(*sc->mat)->nrows,sc->thr_num);
    int j;
    for(j=(sc->tid)*bucket_size; j<min((sc->tid+1)*bucket_size,((*sc->mat)->ncols*(*sc->mat)->nrows));j++)
    {   pthread_mutex_lock(sc->smutex);
        (*sc->mat)->data[j]+=sc->val;
	pthread_mutex_unlock(sc->smutex);
    }
    return NULL;
}

double scalar_op(int num_of_threads,matrix * a, const int val){
    if(num_of_threads>1)
    {
        pthread_t thr[num_of_threads];
        scalar_args sc[num_of_threads];
        pthread_mutex_t smutex=PTHREAD_MUTEX_INITIALIZER;
        clock_t start=clock();
        int i;
        for(i=0;i<num_of_threads;i++)
        {
	    sc[i].mat=&a;
            sc[i].thr_num=num_of_threads;
            sc[i].tid=i;
            sc[i].smutex=&smutex;
	    sc[i].val=val;
            if(pthread_create(&thr[i],NULL,scalar_add,(void *)&sc[i])!=0)
            {
               printf("could not create thread\n");
               return 0;

            }
        }
        int j;
        for(j=0;j<num_of_threads;j++)
        {
            pthread_join(thr[j],NULL);
        }

        clock_t end=clock();
        return (double)(end-start)/CLOCKS_PER_SEC;

    }
    else
    {
	clock_t start=clock();
	int i;
	int j;
	for(i=0;i<a->nrows;i++)
	{
	    for(j=0; j<a->ncols;j++)
       	    {
 	         a->data[i*a->ncols+j]+=val;
	    }
	}
	clock_t end=clock();

        return (double)(end-start)/CLOCKS_PER_SEC;
    }    
}

//Test Functions

char * scalar_test()
{
    bool isPass=false;
    matrix *p=matrix_create(10,10);
    matrix_init(p);
    matrix *q=matrix_create(10,10);
    int i;
    for(i=0;i<100;i++)
    {
	q->data[i]=p->data[i];
    }
    printf("First matrix:\n");
    int k;
    int j;
    for(k=0;k<10;k++)
    {
	for(j=0;j<10;j++)
	{
	    printf("%d ",p->data[k*10+j]);
	}
	printf("\n");
    }
     printf("\n\n\n");   
    int val=rand()%5;
    scalar_op(1,p,val);
    scalar_op(3,q,val);
    printf("Result of addition using one thread:\n");
    int l;
    int m;
    for(l=0;l<10;l++)
    {
        for( m=0;m<10;m++)
        {
            printf("%d ",p->data[l*10+m]);
        }
        printf("\n");
    }
    printf("\n\n\n");
    printf("result of addition using 3 threads:\n");
    int n;
    int o;
    for(n=0;n<10;n++)
    {
        for(o=0;o<10;o++)
        {
            printf("%d ",q->data[n*10+o]);
        }
        printf("\n");
    }
    printf("\n\n\n");
    int r;
    for(r=0;r<100;r++)
    {
	if(p->data[r]==q->data[r])
	{
	    isPass=true;
	}
	else
	{
	    isPass=false;
	}
    }
    matrix_free(&p);
    matrix_free(&q);
    if(isPass==true)
    {
        return "pass";
    }
    return "fail";
}

char * matrix_matrix_test()
{
    bool isPass=false;
    matrix *p=matrix_create(10,10);
    matrix *x=matrix_create(10,10);
    matrix_init(p);
    printf("first matrix:\n");
    int i,j;
    for(i=0;i<10;i++)
    {
        for(j=0;j<10;j++)
        {
            printf("%d ",p->data[i*10+j]);
        }
        printf("\n");
    }
     printf("\n\n\n");
    int k;
    for(k=0;k<100;k++)
    {
        x->data[k]=p->data[k];
    }
    matrix *q=matrix_create(10,10);
    matrix *y=matrix_create(10,10);
    matrix_init(q);
    printf("second matrix:\n");
    int l,m;
    for(l=0;l<10;l++)
    {
        for(m=0;m<10;m++)
        {
            printf("%d ",q->data[l*10+m]);
        }
        printf("\n");
    }
     printf("\n\n\n");
    int n;
    for(n=0;n<100;n++)
    {
        y->data[n]=q->data[n];
    }
    matrix *result1=matrix_create(10,10);
    matrix *result2=matrix_create(10,10);
    matrix_matrix_op(1,p,q,result1);
    matrix_matrix_op(3,x,y,result2);
    printf("result of using one thread:\n");
    int o,r;
    for(o=0;o<10;o++)
    {
        for(r=0;r<10;r++)
        {
            printf("%d ",result1->data[o*10+r]);
        }
        printf("\n");
    }
     printf("\n\n\n");
    printf("result of using 3 threads:\n");
    int s,t;
    for(s=0;s<10;s++)
    {
        for(t=0;t<10;t++)
        {
            printf("%d ",result2->data[s*10+t]);
        }
        printf("\n");
    }
     printf("\n\n\n");
    int u;
     for(u=0;u<100;u++)
     {
        if(result1->data[u]==result2->data[u])
        {
            isPass=true;
        }
        else
        {
            isPass=false;
        }
    }
    matrix_free(&result1);
    matrix_free(&result2);
    if(isPass==true)
    {
	return "pass";
    }
    return "fail";
}


	

int main(int argc,char *argv[])
{
    srand(time(NULL));
    switch(argc)
    {
	case 2:
	    if(strcmp(argv[1],"test")==0)
	    {
		printf("Testing that scalar addition works\n%s\n",scalar_test());
		printf("Testing that matrix matrix addition is working\n%s\n",matrix_matrix_test());
		break;
	    }
	case 5:
	    if(strcmp(argv[1],"scalar")==0)
	    {
		int nrows=atoi(argv[2]);
		int ncols=atoi(argv[3]);
		int num_of_threads=atoi(argv[4]);
		matrix *p=matrix_create(nrows,ncols);
		matrix_init(p);
		printf("The time taken by %d threads is %lf\n",num_of_threads,scalar_op(num_of_threads,p,rand()));
		matrix_free(&p);
		break;
	    }
	    else if(strcmp(argv[1],"matrix")==0)
	    {
		int nrows=atoi(argv[2]);
                int ncols=atoi(argv[3]);
                int num_of_threads=atoi(argv[4]);
                matrix *p=matrix_create(nrows,ncols);
		matrix_init(p);
		matrix *q=matrix_create(nrows,ncols);
		matrix_init(q);
		matrix *result=matrix_create(nrows,ncols);
                printf("The time taken by %d threads is %lf\n",num_of_threads,matrix_matrix_op(num_of_threads,p,q,result));
		matrix_free(&result);
                break;
	    }
	    else
	    {
		printf("Incorrect arguments. Cannot process these values\n");
		return 0;
	    }
	case 6:
	    if(strcmp(argv[1],"scalar")==0)
            {
	        int nrows=atoi(argv[2]);
                int ncols=atoi(argv[3]);
		int val=atoi(argv[4]);
                int num_of_threads=atoi(argv[5]);
                matrix *p=matrix_create(nrows,ncols);
		matrix_init(p);
                printf("The time taken by %d threads is %lf\n",num_of_threads,scalar_op(num_of_threads,p,val));
                matrix_free(&p);
		break;
	    }
	    else
            {
                printf("Incorrect arguments. Cannot process these values\n");
                return 0;
            }
	case 7:
	    if(strcmp(argv[1],"matrix")==0)
            {
                int nrows=atoi(argv[2]);
                int ncols=atoi(argv[3]);
                int num_of_threads=atoi(argv[6]);
                matrix *p=matrix_create(nrows,ncols);
		matrix_init(p);
		nrows=atoi(argv[4]);
		ncols=atoi(argv[5]);
                matrix *q=matrix_create(nrows,ncols);
		matrix_init(q);
                matrix *result=matrix_create(nrows,ncols);
                printf("The time taken by %d threads is %lf\n",num_of_threads,matrix_matrix_op(num_of_threads,p,q,result));
                matrix_free(&result);
		break;
            }
	    else
            {
                printf("Incorrect arguments. Cannot process these values\n");
                return 0;
            }
	default:
	    printf("Wrong number of arguments inputted\n");
	    break;
    }
	   
}
