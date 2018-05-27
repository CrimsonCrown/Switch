// Aluno: Arthur da Silveira Couto
// Matricula: 160002575


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define NMAC 5


int queries[NMAC];
pthread_mutex_t machine_lock[NMAC];
pthread_mutex_t global;
pthread_cond_t machine_cond[NMAC];


void * machine(void * meuid);
void singleconection(int i, int j);
void broadconection(int i);
void * broadsend(void * meuid);

int main(argc, argv)
int argc;
char *argv[];
{
  int erro;
  int i;
  int *id;

  pthread_t tid[NMAC];
  pthread_mutex_init(&global, 0);
  for (i = 0; i < NMAC; i++){
    pthread_cond_init(&machine_cond[i], 0);
    pthread_mutex_init(&machine_lock[i], 0);
  }

    
  for (i = 0; i < NMAC; i++)
  {
      id = (int *) malloc(sizeof(int));
      *id = i;
      erro = pthread_create(&tid[i], NULL, machine, (void *) (id));

      if(erro)
      {
        printf("erro na criacao do thread %d\n", i);
        exit(1);
      }
  }
 
  pthread_join(tid[0],NULL);
  return 0;
}

void * machine(void * pi)
{
  int pid = *(int*)pi;
  free(pi);
  time_t t;
  int contype;
  int trymac;
  srand((unsigned) time(&t));
  while(1){
    contype=rand() % (2*NMAC);
    if(contype < ((2*NMAC)-1)){
      trymac = rand() % (NMAC-1);
      if(trymac>=pid){
        trymac++;
      }
      singleconection(pid, trymac);
    }
    else{
      broadconection(pid);
    }
  }
  pthread_exit(0);
}

void broadconection(int pid)
{
  int erro;
  int *id;
  pthread_t tid[NMAC-1];
  int i,j;
  printf("%d wants to broadcast a package\n", pid);
  pthread_mutex_lock(&global);
  while(1){
    if (pthread_mutex_trylock(&machine_lock[pid]) != 0) {
      pthread_cond_wait(&machine_cond[pid], &global); 
    }
    else{
      break;
    }
  }
  pthread_mutex_unlock(&global);
  printf("%d uploading package to switch\n",pid);
  sleep(1);
  pthread_mutex_lock(&global);
  pthread_mutex_unlock(&machine_lock[pid]);
  pthread_cond_signal(&machine_cond[pid]); 
  printf("%d upload done\n",pid);
  pthread_mutex_unlock(&global);
  j=0;
  for (i=0;i<(NMAC-1);i++){
    if(i==pid){
      j++;
    }
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, broadsend, (void *) (id));
    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
    pthread_join(tid[0],NULL);
    j++;
  }
}

void * broadsend(void * pi){
  int pid = *(int*)pi;
  free(pi);
  pthread_mutex_lock(&global);
  while(1){
    if (pthread_mutex_trylock(&machine_lock[pid]) != 0) {
      pthread_cond_wait(&machine_cond[pid], &global); 
    }
    else{
      break;
    }
  }
  pthread_mutex_unlock(&global);
  printf("package going to %d\n", pid);
  sleep(1);
  pthread_mutex_lock(&global);
  pthread_mutex_unlock(&machine_lock[pid]);
  pthread_cond_signal(&machine_cond[pid]); 
  printf("%d received broadcast\n",pid);
  pthread_mutex_unlock(&global);
  pthread_exit(0);
}

void singleconection(int pid, int pid2)
{
  printf("%d wants to send %d a package\n", pid, pid2);
  pthread_mutex_lock(&global);
  while(1){
    while(1){
      if (pthread_mutex_trylock(&machine_lock[pid]) != 0) {
        pthread_cond_wait(&machine_cond[pid], &global); 
      }
      else{
        break;
      }
    }
    if (pthread_mutex_trylock(&machine_lock[pid2]) != 0) {
      pthread_mutex_unlock(&machine_lock[pid]);
      pthread_cond_wait(&machine_cond[pid2], &global); 
    }
    else{
      break;
    }
  }
  pthread_mutex_unlock(&global);
  printf("package conection between %d and %d started\n",pid, pid2);
  sleep(5);
  pthread_mutex_lock(&global);
  pthread_mutex_unlock(&machine_lock[pid]);
  pthread_mutex_unlock(&machine_lock[pid2]);
  pthread_cond_signal(&machine_cond[pid]); 
  pthread_cond_signal(&machine_cond[pid2]); 
  printf("package conection between %d and %d finished\n",pid, pid2);
  pthread_mutex_unlock(&global);
}