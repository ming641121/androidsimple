#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

 int o,ans=0;
void *thread_function(void *arg );
sem_t bin_sem1,bin_sem2;
pthread_mutex_t lock;
int main()
{
   int res,i=0,k=10;
   pthread_t a_thread; //a_thread is a sub-thread
   void *thread_result;
//init mutex
   pthread_mutex_init(&lock,NULL);

// Initialize semaphore
   res=sem_init(&bin_sem1, 0, 0);
   res=sem_init(&bin_sem2, 0, 0);
   if (res !=0 )
   {
     perror("Sem1 initialization failed ");
     exit(EXIT_FAILURE);
   }   

   res=sem_init(&bin_sem2, 0, 0);
   if (res !=0 )
   {
     perror("Sem2 initialization failed ");
     exit(EXIT_FAILURE);
   }

//Creat a thread
   res=pthread_create(&a_thread, NULL, thread_function, NULL);
   if (res!=0)
   {
        perror("Thread creation failed ");
        exit(EXIT_FAILURE);
   }


//Input data
   while (1) 
   {
      //sem_wait(&bin_sem1);
      printf("main will wait for ... \n");
      printf("I am inputing\n");
      scanf("%d", &i);
      sleep(1);
     sem_wait(&bin_sem1);
     // sleep(2);
      o=i;
      sem_post(&bin_sem2);

      if (i==88) break; 
   }
     sem_post(&bin_sem2);
   res=pthread_cancel(a_thread);
   res=pthread_join(a_thread, &thread_result);

//Canceling sub-thread when input data function terminated
   printf("Canceling sub-thread \n");   
  // res=pthread_cancel(a_thread);
   if (res!=0)
   {  
     perror("Fail to cancel sub-thread");
   }   
 
 
   
   printf("\nWaiting for sub-thread to finish..\n");
//   res=pthread_join(a_thread, &thread_result);
   if (res!=0)
   {
       perror("sub-thread join failed");
       exit(EXIT_FAILURE);
   }

   printf("sub-thread joined\n");
   sem_destroy(&bin_sem1); //clean sem1
   sem_destroy(&bin_sem2); 
   pthread_mutex_destroy(&lock);

   printf("\nTerminated!!!\n"); 
//   exit(0);
   return 0; 
} 
   // A sub-thread function
void *thread_function(void *arg)
{
      int *QQQ=NULL;
      if ( pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL) )
      {
          perror("Fail to set cancel-state");
          exit(EXIT_FAILURE);
      }
      if ( pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL) )
      {
          perror("Fail to set cancel-type");
          exit(EXIT_FAILURE);

      }
    //  sem_post(&bin_sem1);
   //Output data
      while (1)
      {
         printf("sub-thread will wait for ...\n");
         sem_post(&bin_sem1);    
         sem_wait(&bin_sem2);
//         pthread_mutex_lock(&lock);
//         sleep(2);     
         QQQ=&o;
       // sem_post(&bin_sem1);
         printf("QQQ=\n");
         printf("%d\n",*QQQ);
         if (o==88) break;
  //       pthread_mutex_unlock(&lock);
      }
      sleep(5);
      pthread_exit(0);
      
}





