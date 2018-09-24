#include <input_manager.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <tslib.h>
#include <config.h>

/* �ο�tslib���ts_print.c */

static struct tsdev *g_tTSDev;
static int giXres;
static int giYres;

static int TouchScreenDeviceInit(void);
static int TouchScreenDeviceExit(void);
static int TouchScreenGetInputEvent(PT_InputEvent ptInputEvent);



static T_InputOpr g_tTouchScreenOpr = {
    .name           = "touchscreen",
    .DeviceInit     = TouchScreenDeviceInit,
    .DeviceExit     = TouchScreenDeviceExit,
    .GetInputEvent  = TouchScreenGetInputEvent,
};

static int TouchScreenDeviceInit(void)
{
    char *pcTSName = NULL;
    
    if ((pcTSName = getenv("TSLIB_TSDEVICE")) != NULL ) 
    {
        g_tTSDev = ts_open(pcTSName, 1);
    }
    else
    {
        g_tTSDev = ts_open("/dev/event0", 1);
    }
    
    if (!g_tTSDev) {
        DBG_PRINTF("ts_open error!\n");
        return -1;
    }

    if (ts_config(g_tTSDev)) {
        DBG_PRINTF("ts_config error!\n");
        return -1;
    }

    if (GetDispResolution(&giXres, &giYres))
    {
        return -1;
    }

    g_tTouchScreenOpr.iFd = ts_fd(g_tTSDev);

    return 0;
}

static int TouchScreenDeviceExit(void)
{
    return 0;
}

static int isOutOf500ms(struct timeval *ptPreTime, struct timeval *ptNowTime)
{
    int iPreMs;
    int iNowMs;

    iPreMs = ptPreTime->tv_sec * 1000 + ptPreTime->tv_usec / 1000;
    iNowMs = ptNowTime->tv_sec * 1000 + ptNowTime->tv_usec / 1000;

    return (iNowMs > iPreMs + 500);
}

static int TouchScreenGetInputEvent(PT_InputEvent ptInputEvent)
{
    struct ts_sample tSamp;
    int iRet;

//    DBG_PRINTF("enter TouchScreenGetInputEvent fun\r\n");

    static struct timeval tPreTime;
    struct timeval tv_tmp;

    iRet = ts_read(g_tTSDev, &tSamp, 1);

    if (iRet < 0) {
//        DBG_PRINTF("TouchScreenGetInputEvent err\r\n");
        return -1;
    }

    gettimeofday(&tv_tmp,NULL);

    
//    DBG_PRINTF("pre time is : %d:%d\r\n",tPreTime.tv_sec,tPreTime.tv_usec);
//    DBG_PRINTF("tv_tmp is : %d:%d\r\n",tv_tmp.tv_sec,tv_tmp.tv_usec);

    /* �������� */
    if (isOutOf500ms(&tPreTime, &tv_tmp))
    {
        /* ����˴δ����¼�������ʱ��, ���ϴ��¼�������500ms */
        tPreTime = tv_tmp;
        ptInputEvent->tTime = tv_tmp;
        ptInputEvent->iType = INPUT_TYPE_TOUCHSCREEN;

        if (tSamp.y < giYres/3)
        {
            ptInputEvent->iVal = INPUT_VALUE_UP;
        }
        else if (tSamp.y > 2*giYres/3)
        {
            ptInputEvent->iVal = INPUT_VALUE_DOWN;
        }
        else
        {
            ptInputEvent->iVal = INPUT_VALUE_UNKNOWN;
        }
        return 0;
        
    }
    else
    {
//        DBG_PRINTF("with in 500ms err\r\n");
        return -1;
    }


    return 0;    
    
}

// TouchScreenInit����ֻע��(�ṹ��ѹ������)��û�о���ִ�У�ִ�г�ʼ������ StdinDevInit 
// ���������
int TouchScreenInit(void)
{
    int iError = 0;
    iError = RegesiterInputOpr(&g_tTouchScreenOpr);
    DBG_PRINTF("TouchScreenInit is %d\r\n",iError);
    return iError;
}

