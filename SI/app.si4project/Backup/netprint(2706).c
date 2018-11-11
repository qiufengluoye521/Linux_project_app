#include "debug_manager.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "config.h"
#include <pthread.h>
#include <stdlib.h>

#define SERVER_PORT 5678
#define PRINT_BUF_SIZE   (16*1024)

static int iSocketServer;
static struct sockaddr_in g_tSocketServerAddr;
static struct sockaddr_in g_tSocketClientAddr;
static pthread_t g_trecive_tid;
static pthread_t g_tsend_tid;
static pthread_mutex_t g_tNetDbgMutex   = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_tNetDbgCondvar  = PTHREAD_COND_INITIALIZER;
static int g_iHaveConnected             = 0;
static int g_iReadbufferIndex           = 0;
static int g_iWritebufferIndex          = 0;
static char * g_pcNetDbgBuffer;

static int NetDebugOprInit(void);
static int NetDebugOprExit(void);
static int NetDebugOprPrint(char* dbg_buf);


static T_DebugOpr g_tNetPrintDebugOpr = {
    .name           = "netprint",
    .canUsed        = 1,
    .DebugOprInit   = NetDebugOprInit,
    .DebugOprExit   = NetDebugOprExit,
    .Debug_PRINT    = NetDebugOprPrint,
};

/* 使用环形缓冲区用于打印，环形缓冲区基本的四个函数�?
 * 1. isFull        judge if the buffer is full
 * 2. isEmpty       judge if the buffer is empty
 * 3. PutData       put data into buffer
 * 4. GetData       get data frome buffer
 */
static int isFull(void)
{
    if(((g_iWritebufferIndex + 1) % PRINT_BUF_SIZE) == g_iReadbufferIndex)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int isEmpty(void)
{
    return (g_iWritebufferIndex == g_iReadbufferIndex);
}

static int PutData(char cVal)
{
    if(isFull())
        return -1;
    g_pcNetDbgBuffer[g_iWritebufferIndex] = cVal;
    g_iWritebufferIndex = (g_iWritebufferIndex + 1) % PRINT_BUF_SIZE;
    
    return 0;
}

static int GetData(char *pcVal)
{
    if(isEmpty())
        return -1;
    *pcVal = g_pcNetDbgBuffer[g_iReadbufferIndex];
    g_iReadbufferIndex = (g_iReadbufferIndex + 1) % PRINT_BUF_SIZE;
    
    return 0;
}

static void *recive_threadfunc(void *pvoid)
{
    socklen_t iAddrLen;
    char cRecvBuf[1000];
    int iRecvLen;
    int dbg_level = 0;
    
    while(1)
    {
        iAddrLen = sizeof(struct sockaddr);
        iRecvLen = recvfrom(iSocketServer, cRecvBuf, 999, 0, (struct sockaddr *)&g_tSocketServerAddr, &iAddrLen);

        if (iRecvLen > 0)
        {
            cRecvBuf[iRecvLen] = '\0';
            DBG_PRINTF("netprint.c get msg: %s\n", cRecvBuf);
            if (strcmp(cRecvBuf, "setclient")  == 0)
            {
                g_tSocketClientAddr = g_tSocketServerAddr;
                g_iHaveConnected = 1;
            }
            else if(strncmp(cRecvBuf,"dbglevel=",9) == 0)
            {
                dbg_level   = cRecvBuf[9] - '0';
                Set_Dbg_Level(dbg_level);
            }
            else 
            {
                Set_Dbg_Channel(cRecvBuf);
            }
        }
    }
    
    return NULL;
}

static void *send_threadfunc(void *pvoid) 
{
    char strSendTmp[512];
    char get_char;
    int i = 0;
    int iSendLen;
    int iAddrLen;

    while(1)
    {
        pthread_mutex_lock(&g_tNetDbgMutex);
        pthread_cond_wait(&g_tNetDbgCondvar, &g_tNetDbgMutex);	
        pthread_mutex_unlock(&g_tNetDbgMutex);
        
        while(g_iHaveConnected && (!isEmpty()))
        {
            i = 0;
            while((i < 512) && (0 ==  GetData(&get_char)))
            {
                strSendTmp[i]   = get_char;
                i++;
            }
            
            iAddrLen = sizeof(struct sockaddr);
            iSendLen = sendto(iSocketServer, strSendTmp, i, 0,
                      (const struct sockaddr *)&g_tSocketClientAddr, iAddrLen);
            
        }
        
    }
}

static int NetDebugOprInit(void)
{
    int iRet;

    
    g_pcNetDbgBuffer = malloc(PRINT_BUF_SIZE);

    
    iSocketServer = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == iSocketServer)
    {
        DBG_PRINTF("socket error!\n");
        return -1;
    }

    g_tSocketServerAddr.sin_family      = AF_INET;
    g_tSocketServerAddr.sin_port        = htons(SERVER_PORT);  /* host to net, short */
    g_tSocketServerAddr.sin_addr.s_addr = INADDR_ANY;
    memset(g_tSocketServerAddr.sin_zero, 0, 8);

    iRet = bind(iSocketServer, (const struct sockaddr *)&g_tSocketServerAddr, sizeof(g_tSocketServerAddr));
    if (-1 == iRet)
    {
        DBG_PRINTF("bind error!\n");
        return -1;
    }
    
    /* 创建接收线程和发送线�?*/
    pthread_create(&g_trecive_tid, NULL, &recive_threadfunc, NULL);
    pthread_create(&g_tsend_tid, NULL, &send_threadfunc, NULL);
    
    return 0;
}
static int NetDebugOprExit(void)
{
    close(iSocketServer);
    free(g_pcNetDbgBuffer);
}
static int NetDebugOprPrint(char* dbg_buf)
{
    int str_len = 0;
    int i;
    
    str_len  = strlen(dbg_buf);
    
    for(i = 0;i < str_len;i++)
    {
        PutData(dbg_buf[i]);
    }
    
    
    pthread_cond_signal(&g_tNetDbgCondvar);

    // iAddrLen = sizeof(struct sockaddr);
    // str_len  = strlen(dbg_buf);
    // strcpy(cRecvBuf,dbg_buf);

    // // iSendLen = sendto(iSocketServer, cRecvBuf, 6, 0,
                      // // (const struct sockaddr *)&g_tSocketClientAddr, iAddrLen);
    // iSendLen = sendto(iSocketServer, cRecvBuf, str_len, 0,
                      // (const struct sockaddr *)&g_tSocketClientAddr, iAddrLen);
}

int NetPrintInit(void)
{
    return RegisterDebugOpr(&g_tNetPrintDebugOpr);
}


