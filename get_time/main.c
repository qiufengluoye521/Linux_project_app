#include <stdio.h>
#include <sys/time.h>

int gettimeofday(struct timeval *tv, struct timezone *tz);

int main(int argc,char * argv[]){
    struct timeval tv;
    while(1){
        gettimeofday(&tv,NULL);
        printf("time %ld:%ld\n",tv.tv_sec,tv.tv_usec);
        sleep(2);
    }
    return 0;
}