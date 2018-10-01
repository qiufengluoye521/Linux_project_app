#include <stdio.h>
#include <pthread.h>

int i = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;

void *threadfunc(void *pvoid)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        if(i < 200)
        {
            i++;
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        if(i >= 123)
        {
            pthread_cond_signal(&condvar);
        }
    }
    return NULL;
}



int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, &threadfunc, NULL);
    pthread_mutex_lock(&mutex);

    pthread_cond_wait(&condvar, &mutex);
    
    printf("i = %d\n", i);
    pthread_mutex_unlock(&mutex);
    pthread_join(tid, NULL);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condvar);
    return 0;
}
