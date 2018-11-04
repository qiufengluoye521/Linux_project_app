#include <input_manager.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <config.h>


static int stdintDeviceInit(void);
static int stdintDeviceExit(void);
static int stdintGetInputEvent(PT_InputEvent ptInputEvent);



static T_InputOpr g_tStdintInputOpr = {
    .name           = "stdint",
    .DeviceInit     = stdintDeviceInit,
    .DeviceExit     = stdintDeviceExit,
    .GetInputEvent  = stdintGetInputEvent,
};

static int stdintDeviceInit(void)
{
//    struct timeval t_timeval;
//    fd_set fds;
    struct termios t_termios;

//    t_timeval.tv_sec    = 0;
//    t_timeval.tv_usec   = 0;
//    FD_ZERO(&fds);
//    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
//    select(STDIN_FILENO+1, &fds, NULL, NULL, &t_timeval);
//    FD_ISSET(STDIN_FILENO, &fds);

    //get the terminal state
    tcgetattr(STDIN_FILENO, &t_termios);
    
    //turn off canonical mode
    t_termios.c_lflag &= ~ICANON;
    //minimum of number input read.
    t_termios.c_cc[VMIN] = 1;

    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &t_termios);
}

static int stdintDeviceExit(void)
{
    struct termios t_termios;
    
    //get the terminal state
    tcgetattr(STDIN_FILENO, &t_termios);
    //turn on canonical mode
    t_termios.c_lflag |= ICANON;
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &t_termios);
}

static int stdintGetInputEvent(PT_InputEvent ptInputEvent)
{
    struct timeval tTV;
    fd_set tFDs;
    char c;

    tTV.tv_sec = 0;
    tTV.tv_usec = 0;
    FD_ZERO(&tFDs);
	
    FD_SET(STDIN_FILENO, &tFDs); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &tFDs, NULL, NULL, &tTV);

    ptInputEvent->iType     = INPUT_TYPE_STDIN;
    gettimeofday(&ptInputEvent->tTime, NULL);

    c=fgetc(stdin);
    if('n' == c) {
        ptInputEvent->iVal      = INPUT_VALUE_DOWN;
    } else if('u' == c) {
        ptInputEvent->iVal      = INPUT_VALUE_UP;
    } else if('q' == c) {
        ptInputEvent->iVal      = INPUT_VALUE_EXIT;
    } else {
        ptInputEvent->iVal      = INPUT_VALUE_UNKNOWN;
		DebugPrint("stdint input unknow return -1\r\n");
    }
	//DebugPrint("stdint input return 0\r\n");
    return 0;

}

// StdinInit函数只注册(结构体压入链表)，没有具体执行，执行初始化是在 StdinDevInit 
// 函数中完成
int StdinInit(void)
{
    return RegesiterInputOpr(&g_tStdintInputOpr);
}

