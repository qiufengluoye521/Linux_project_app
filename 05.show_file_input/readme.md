输入方式：串口输入、触摸屏输入

获得输入状态的方式：

1. **查询法**：cpu一直循环查询输入状态，直到查询到有输入状态变化：

   缺点：占用CPU率非常高

   优点：实现简单

   这里需要用到“non-blocking user input”的概念，用户串口输入字符，不需要点回车，直接响应。相关内容可以参考 Non-blocking user input in loop without ncurses 文档。实验的一个小demo可以参考 non_blocking_user_input程序。源代码如下：

   ```c
   #include <termios.h>
   #include <unistd.h>
   #include <stdio.h>
   #include <sys/time.h>
   
   #define NB_ENABLE      1
   #define NB_DISABLE     0
   
   int kbhit()
   {
       struct timeval tv;
       fd_set fds;
       tv.tv_sec = 0;
       tv.tv_usec = 0;
       FD_ZERO(&fds);
       FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
       select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
       return FD_ISSET(STDIN_FILENO, &fds);
   }
   
   void nonblock(int state)
   {
       struct termios ttystate;
    
       //get the terminal state
       tcgetattr(STDIN_FILENO, &ttystate);
    
       if (state==NB_ENABLE)
       {
           //turn off canonical mode
           ttystate.c_lflag &= ~ICANON;
           //minimum of number input read.
           ttystate.c_cc[VMIN] = 1;
       }
       else if (state==NB_DISABLE)
       {
           //turn on canonical mode
           ttystate.c_lflag |= ICANON;
       }
       //set the terminal attributes.
       tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
    
   }
   
   int main()
   {
       char c;
       int i=0;
    
       nonblock(NB_ENABLE);
       while(!i)
       {
           usleep(1);
           i=kbhit();
           if (i!=0)
           {
               c=fgetc(stdin);
               if (c=='q')
                   i=1;
               else if (c == 'n')
                   i = 1;
               else
                   i=0;
           }
           //printf("kbhit :%d\r\n",i);
           //fprintf(stderr,"%d ",i);
       }
       printf("\n you hit %c. \n",c);
       nonblock(NB_DISABLE);
    
       return 0;
   }
   ```

   **测试方法：**

   a. insmod  s3c_ts.ko

   确定是哪个设备节点对应触摸屏

   b. export TSLIB_TSDEVICE=/dev/event0
   export TSLIB_CALIBFILE=/etc/pointercal
   export TSLIB_CONFFILE=/etc/ts.conf
   export TSLIB_PLUGINDIR=/lib/ts
   export TSLIB_CONSOLEDEVICE=none
   export TSLIB_FBDEVICE=/dev/fb0

   c. 较准
   ts_calibrate   

   d. telnetd -l /bin/sh  //启动telnet服务，为了登录进去观察CPU占用率（secureCRT telnet方式连接开发板）

   e. ./show_file -s 24 -d fb -f ./MSYH.TTF ./utf8_novel.txt

   f. telnet上开发板执行top命令观察

2. **使用select,poll**   // CPU占用率低，适用于很多简单场合

   参考：UNIX环境高级编程 I/O多路转接

   监测多个文件，只要有某一个文件可读/可写/异常或超时，即返回

   int select(int nfds,    					// 最大文件句柄+1

   ​		fd_set *readfds, 				// 被监测是否可读的文件

   ​		fd_set *writefds,				//  被监测是否可写的文件

   ​		fd_set *exceptfds, 			// 被监测是否有异常的文件

   ​		struct timeval *timeout);		//  超时时间
                 

   输入 man select可以看到select的用法，及一个例子，代码如 select_demo 所示： 

   ```c
   #include <stdio.h>
   #include <stdlib.h>
   #include <sys/time.h>
   #include <sys/types.h>
   #include <unistd.h>
   
   int main(void)
   {
       fd_set rfds;
       struct timeval tv;
       int retval;
       
       printf("enter from serial port and press Enter\r\n");
   
       /* Watch stdin (fd 0) to see when it has input. */
       FD_ZERO(&rfds);
       FD_SET(0, &rfds);
   
       /* Wait up to five seconds. */
       tv.tv_sec = 5;
       tv.tv_usec = 0;
   
       retval = select(1, &rfds, NULL, NULL, &tv);
       /* Don't rely on the value of tv now! */
   
       if (retval == -1)
           perror("select()");
       else if (retval)
           printf("Data is available now.\n");
           /* FD_ISSET(0, &rfds) will be true. */
       else
           printf("No data within five seconds.\n");
   
       exit(EXIT_SUCCESS);
   }
   ```

   测试方法如1.

   

   3. **thread 线程机制**

      线程机制需要包含pthread.h头文件，具体使用方法可参考 《Unix_Linux_Windows_OpenMP多线程编程》，内容较多，可先熟悉其基本用法。

      参考如下代码，代码改自文档中的demo。 

      ```c
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
                  /* 唤醒主线程 */
                  pthread_cond_signal(&condvar);
              }
          }
          return NULL;
      }
      
      
      
      int main()
      {
          pthread_t tid;
          // 创建一个线程，线程的执行函数是threadfunc
          pthread_create(&tid, NULL, &threadfunc, NULL);
          /* 访问临界资源前，先获得互斥量 */
          pthread_mutex_lock(&mutex);
      	/* 休眠，等待 pthread_cond_signal(&g_tConVar)函数来唤醒 */
          pthread_cond_wait(&condvar, &mutex);
          
          printf("i = %d\n", i);
          /* 释放互斥量 */
          pthread_mutex_unlock(&mutex);
          /* 等待一个线程完成后继续执行 */
          pthread_join(tid, NULL);
          /* 销毁互斥量 mutex */
          pthread_mutex_destroy(&mutex);
          /* 销毁条件变量 */
          pthread_cond_destroy(&condvar);
          return 0;
      }
      ```

      注：

      pthread_join：函数可以使调用这个函数的线程等待指定的线程运行完成再继续执行。它的 形式为：  

      int pthread_join(pthread_t thread, void **value_ptr);

      参数 thread 为要等待的线程的 ID，参数 value_ptr 为指向返回值的指针提供一个位置，这 个返回值是由目标线程传递给 pthread_exit 或 return 的。如果 value_ptr 为 NULL，调用 程序就不会对目标线程的返回状态进行检索了。如果函数调用成功，pthread_join 返回 0， 如果不成功，pthread_join 返回一个非零的错误码。下表列出了 pthread_join 的错误形式 及相应的错误码 

      | 错误   | 原因                              |
      | ------ | --------------------------------- |
      | EINVAL | thread 对应的不是一个可接合的线程 |
      | ESRCH  | 没有 ID 为 thread 的线程          |


   测试方法如1.

   

   

   

   

 