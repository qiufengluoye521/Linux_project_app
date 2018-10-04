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

#define SERVER_PORT 5678

static int iSocketServer;
static struct sockaddr_in g_tSocketServerAddr;
static struct sockaddr_in g_tSocketClientAddr;

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

static int NetDebugOprInit(void)
{
    int iRet;
    int iRecvLen;
    char cRecvBuf[1000];
    struct sockaddr_in tSocketClientAddr;
    socklen_t iAddrLen;
    
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

    iRet = bind(iSocketServer, (const struct sockaddr *)&g_tSocketServerAddr, sizeof(struct sockaddr));
    if (-1 == iRet)
    {
        DBG_PRINTF("bind error!\n");
        return -1;
    }

    while(1)
    {
        iRecvLen = recvfrom(iSocketServer, cRecvBuf, 999, 0, (struct sockaddr *)&tSocketClientAddr, &iAddrLen);

        if (iRecvLen > 0)
        {
            cRecvBuf[iRecvLen] = '\0';
            DBG_PRINTF("netprint.c get msg: %s\n", cRecvBuf);
            if (strcmp(cRecvBuf, "setclient")  == 0)
            {
                g_tSocketClientAddr = tSocketClientAddr;
                return 0;
                //g_iHaveConnected = 1;
            }
        }
    }

    
}
static int NetDebugOprExit(void)
{
    close(iSocketServer);
}
static int NetDebugOprPrint(char* dbg_buf)
{
    int iAddrLen;
    int iSendLen;
    char cRecvBuf[1000];
    int str_len = 0;

    iAddrLen = sizeof(struct sockaddr);
    str_len  = strlen(dbg_buf);
    printf("start print from net\n");
    iSendLen = sendto(iSocketServer, cRecvBuf, str_len, 0,
                      (const struct sockaddr *)&g_tSocketClientAddr, iAddrLen);
}

int NetPrintInit(void)
{
    return RegisterDebugOpr(&g_tNetPrintDebugOpr);
}


