#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <pthread.h>

#define MAX 50
#define NOTHREADS 2

int array1[1000], array2[1000], array3[1000], array4[1000], array5[1000], array6[1000], n=1000, c, d, swap;

typedef struct node {
	int i;
	int j;
} NODE;



//------------------------------bubble sort------------------------------

bubblesort() { 
	int c,d,swap;
	
	for (c = 0 ; c < ( n - 1 ); c++)  {
	    for (d = 0 ; d < n - c - 1; d++) {
		      if (array1[d] > array1[d+1])      {
			        swap = array1[d];
			        array1[d] = array1[d+1];
			        array1[d+1] = swap;
      			}
    	    }
	}

	printf("\nBubble sort results\n");

	for ( c = 0 ; c < n ; c++ )
	     printf("%d ", array1[c]);

	printf ("\n");
}


//------------------------------parallel bubble------------------------------

void bubblep() {

        int i, tmp, changes;
        int chunk;
        chunk = n / 2;
	changes = 1;
	int nr = 0;

	    while(changes)    {

		    #pragma omp parallel private(tmp) 
		    {	

			    nr++;
			    changes = 0;

			    #pragma omp for reduction(+:changes)

			    for(i = 0; i < n - 1; i = i + 2) {
			            if(array4[i] > array4[i+1] ) {
			                    tmp = array4[i];
			                    array4[i] = array4[i+1];
			                    array4[i+1] = tmp;
			                    ++changes;
			            }
			    }
		
			    #pragma omp for reduction(+:changes)

			    for(i = 1; i < n - 1; i = i + 2) {
			            if( array4[i] > array4[i+1] ) {
			                    tmp = array4[i];
			                    array4[i] = array4[i+1];
			                    array4[i+1] = tmp;
			                    ++changes;
			            }
			    }
	    	    }
    	   }

	   printf("\nBubble Sort with threads results\n");

	   for ( i = 0 ; i < n ; i++ )
		     printf("%d ", array4[i]);
	   
	   printf ("\n");
	   
	   return;

}


//------------------------------merge with threads------------------------------

void pmerge(int i, int j) {

        int mid = (i+j)/2;
        int ai = i;
        int bi = mid+1;
        int newa[j-i+1], newai = 0;

        while(ai <= mid && bi <= j) {

                if (array5[ai] > array5[bi])
                        newa[newai++] = array5[bi++];
                else
                        newa[newai++] = array5[ai++];
        }

        while(ai <= mid) {
                newa[newai++] = array5[ai++];
        }

        while(bi <= j) {
                newa[newai++] = array5[bi++];
        }

        for (ai = 0; ai < (j-i+1) ; ai++)
                array5[i+ai] = newa[ai];

}

void * pmergesort(void *a)
{
                NODE *p = (NODE *)a;
                NODE n1, n2;
                int mid = (p->i+p->j)/2;
                pthread_t tid1, tid2;
                int ret;

                n1.i = p->i;
                n1.j = mid;

                n2.i = mid+1;
                n2.j = p->j;

                if (p->i >= p->j) return;

                ret = pthread_create(&tid1, NULL, pmergesort, &n1);

                if (ret) {
                        printf("%d %s - unable to create thread - ret - %d\n", __LINE__, __FUNCTION__, ret);
                        exit(1);
                }


                ret = pthread_create(&tid2, NULL, pmergesort, &n2);

                if (ret) {
                        printf("%d %s - unable to create thread - ret - %d\n", __LINE__, __FUNCTION__, ret);
                        exit(1);
                }

                pthread_join(tid1, NULL);
                pthread_join(tid2, NULL);

                pmerge(p->i, p->j);
                pthread_exit(NULL);

}


//------------------------------merge sort------------------------------

void mergeSort(int arr[],int low,int mid,int high) {

    int i,m,k,l,temp[1000];

    l=low;
    i=low;
    m=mid+1;

    while((l<=mid)&&(m<=high)) {

         if(arr[l]<=arr[m]) {
             temp[i]=arr[l];
             l++;
         } else {
             temp[i]=arr[m];
             m++;
         }

         i++;
    }

    if(l>mid) {
         for(k=m;k<=high;k++) {
             temp[i]=arr[k];
             i++;
         }
    } else {
         for(k=l;k<=mid;k++) {
             temp[i]=arr[k];
             i++;
         }
    }

    for(k=low;k<=high;k++) {
         arr[k]=temp[k];
    }
}

void partition(int arr[],int low,int high) {

    int mid;

    if(low<high) {
         mid=(low+high)/2;
         partition(arr,low,mid);
         partition(arr,mid+1,high);
         mergeSort(arr,low,mid,high);
    }
}


//------------------------------shell sort------------------------------

void shellSort(int numbers[], int array_size) {

	int i, j, increment, temp;
	increment = 3;

	while (increment > 0) {
		for (i=0; i < array_size; i++) {
			j = i;
			temp = numbers[i];

			while ((j >= increment) && (numbers[j-increment] > temp)) {
				numbers[j] = numbers[j - increment];
				j = j - increment;
			}

			numbers[j] = temp;
		}

		if (increment/2 != 0)
			increment = increment/2;
		else if (increment == 1)
			increment = 0;
		else
			increment = 1;
	}
}


//------------------------------shell sort using threads------------------------------

void Shsort(int a[], int n, int s) {
	int j,i,key;
	
	for (j=s; j<n; j+=s) {
		key = a[j];
		i = j - s;
	
		while (i >= 0 && a[i] > key) {
			a[i+s] = a[i];
			i-=s;
		}
		a[i+s] = key;
	}
}

void shellsortp(int a[],int n) {
	int i, m;

	for(m = n/2; m > 0; m /= 2)
	{
		#pragma omp parallel for shared(a,m,n) private (i) default(none)
			for(i = 0; i < m; i++)
				Shsort(&(a[i]), n-i, m);
	}

	return;
}

//------------------------------main program------------------------------

int main() {

	int i,j;
	NODE m;

	pthread_t tid;
	int ret;
	m.i = 0;
	m.j = n-1;

	clock_t start, end;
	double cpu_time_used;

	system("clear");

	for (c = 0; c < n; c++) {
		array1[c]=rand()%100;
		array2[c]=array1[c] ;
		array3[c]=array1[c];
		array4[c]=array1[c] ;
		array5[c]=array1[c];
		array6[c]=array1[c] ;
	}

	printf("\nBefore Sorting:\n\n");

	for(i=0;i<n;i++)
		printf("%d ",array1[i]);

	printf("\n");



//------------------------------Bubble sort without threads------------------------------

	start = clock();
	bubblesort();
	end = clock();

	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

	printf("\nTime taken: %lf\n",cpu_time_used);


//------------------------------Bubble sort with threads------------------------------

	start = clock();
	bubblep();
	end = clock();

	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

	printf("\nTime taken: %lf\n",cpu_time_used);

//------------------------------merge sort------------------------------

	start = clock();
	partition(array2,0,n-1);
	end = clock();

	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

	printf("\nMerge Sort results\n");

	for(i=0;i<n;i++){
		printf("%d ",array2[i]);
	}

	printf ("\n");

	printf("Time taken: %lf\n",cpu_time_used);

//------------------------------merge sort using threads------------------------------

	start = clock();
	ret=pthread_create(&tid, NULL, pmergesort, (void *)&m);

	if (ret) {
		printf("%d %s - unable to create thread - ret - %d\n", __LINE__, __FUNCTION__, ret);
		exit(1);
	}

	pthread_join(tid, NULL);

	end = clock();

	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

	printf("\nMerge Sort with threads results\n");

	for (i = 0; i < n; i++)
		printf ("%d ", array5[i]);

	printf ("\n");

	printf("Time taken: %lf\n",cpu_time_used);

//------------------------------shell sort------------------------------

	start = clock();
	shellSort(array3, n);
	end = clock();

	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

	printf("\nShell Sort results\n");

	for (i = 0; i < n; i++)
		printf("%d ", array3[i]);

	printf("\n");

	printf("Time taken: %lf\n",cpu_time_used);


//------------------------------shell sort using threads------------------------------

	start = clock();
	shellsortp(array6,n);
	end = clock();

	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

	printf("\nShell Sort with threads results\n");

	for (i = 0; i < n; i++)
		printf("%d ", array6[i]);

	printf("\n");

	printf("Time taken: %lf\n\n",cpu_time_used);

	return 0;
}
