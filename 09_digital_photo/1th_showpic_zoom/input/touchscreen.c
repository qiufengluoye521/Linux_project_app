#include <input_manager.h>
#include <disp_manager.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <tslib.h>
#include <config.h>


/* 参考tslib里的ts_print.c */

static struct tsdev *g_tTSDev;
static int giXres;
static int giYres;
static int giBpp;

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
        g_tTSDev = ts_open(pcTSName, 0);    /* 阻塞方式打开 */
    }
    else
    {
        g_tTSDev = ts_open("/dev/event0", 1);
    }
    
    if (!g_tTSDev) {
        DebugPrint("ts_open error!\n");
        return -1;
    }

    if (ts_config(g_tTSDev)) {
        DebugPrint("ts_config error!\n");
        return -1;
    }

    if (GetDispResolution(&giXres, &giYres,&giBpp))
    {
        return -1;
    }

    return 0;
}

static int TouchScreenDeviceExit(void)
{
    return 0;
}


static int TouchScreenGetInputEvent(PT_InputEvent ptInputEvent)
{
    struct ts_sample tSamp;
    int iRet;


    iRet = ts_read(g_tTSDev, &tSamp, 1);


    /* 处理数据 */
    if (iRet == 1)
    {
        ptInputEvent->tTime     = tSamp.tv;
        ptInputEvent->iType     = INPUT_TYPE_TOUCHSCREEN;
        ptInputEvent->iX        = tSamp.x;
        ptInputEvent->iY        = tSamp.y;
		ptInputEvent->iPressure = tSamp.pressure;

        
        return 0;
        
    }
    else
    {
        return -1;
    }


    return 0;    
    
}

// TouchScreenInit函数只注册(结构体压入链表)，没有具体执行，执行初始化是在 StdinDevInit 
// 函数中完成
int TouchScreenInit(void)
{
    int iError = 0;
    iError = RegesiterInputOpr(&g_tTouchScreenOpr);
    //DebugPrint("TouchScreenInit is %d\r\n",iError);
    return iError;
}

