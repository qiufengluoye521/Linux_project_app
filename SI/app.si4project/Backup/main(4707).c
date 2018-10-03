#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <config.h>
#include <draw.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <input_manager.h>
#include <string.h>


/* ./show_file [-s Size] [-f freetype_font_file] [-h HZK] <text_file> */
int main(int argc, char **argv)
{
	int iError;
	unsigned int dwFontSize = 16;
	char acHzkFile[128];
	char acFreetypeFile[128];
	char acTextFile[128];

	char acDisplay[128];

	char cOpr;
	int bList = 0;

	acHzkFile[0]  = '\0';
	acFreetypeFile[0] = '\0';
	acTextFile[0] = '\0';

    T_InputEvent tInputEventTmp;
    int pre_time    = 0;
    int time_dif    = 0;

    /* 先做DebugInit(仅压入链表，初始化操作调用int InitDebugChanel 否则调试信息无法打印 */
    iError = DebugInit();
    // if(0 != iError)
    // {
        // DBG_PRINTF("DebugInit err\n");
        // return -1;
    // }
    /* Debug 函数初始化 */
    iError = InitDebugChanel();
    if(0 != iError)
    {
        DBG_PRINTF("InitDebugChanel err\n");
        return -1;
    }

	strcpy(acDisplay, "fb");
	
	while ((iError = getopt(argc, argv, "ls:f:h:d:")) != -1)
	{
		switch(iError)
		{
			case 'l':
			{
				  bList = 1;
				  break;
			}
			case 's':
			{
				  dwFontSize = strtoul(optarg, NULL, 0);
				  break;
			}
			case 'f':
			{
				  strncpy(acFreetypeFile, optarg, 128);
				  acFreetypeFile[127] = '\0';
				  break;
			}			
			case 'h':
			{
					strncpy(acHzkFile, optarg, 128);
					acHzkFile[127] = '\0';
					break;
			}
			case 'd':
			{
				strncpy(acDisplay, optarg, 128);
				acDisplay[127] = '\0';
				break;
			}
			default:
			{
					DebugPrint("Usage: %s [-s Size] [-d display] [-f font_file] [-h HZK] <text_file>\n", argv[0]);
					DebugPrint("Usage: %s -l\n", argv[0]);
					return -1;
					break;
			}
		}
	}

	if (!bList && (optind >= argc))
	{
		DebugPrint("Usage: %s [-s Size] [-d display] [-f font_file] [-h HZK] <text_file>\n", argv[0]);
		DebugPrint("Usage: %s -l\n", argv[0]);
		return -1;
	}
		
	iError = DisplayInit();
	if (iError)
	{
		DebugPrint("DisplayInit error!\n");
		return -1;
	}

	iError = FontsInit();
	if (iError)
	{
		DebugPrint("FontsInit error!\n");
		return -1;
	}

	iError = EncodingInit();
	if (iError)
	{
		DebugPrint("EncodingInit error!\n");
		return -1;
	}

    iError = InputInit();
	if (iError)
	{
		DebugPrint("InputInit error!\n");
		return -1;
	}

	if (bList)
	{
		DebugPrint("supported display:\n");
		ShowDispOpr();

		DebugPrint("supported font:\n");
		ShowFontOpr();

		DebugPrint("supported encoding:\n");
		ShowEncodingOpr();
		DebugPrint("supported input:\n");
		ShowInputOpr();
		return 0;
	}

	strncpy(acTextFile, argv[optind], 128);
	acTextFile[127] = '\0';
		
	iError = OpenTextFile(acTextFile);
	if (iError)
	{
		DebugPrint("OpenTextFile error!\n");
		return -1;
	}

	iError = SetTextDetail(acHzkFile, acFreetypeFile, dwFontSize);
	if (iError)
	{
		DebugPrint("SetTextDetail error!\n");
		return -1;
	}

	DebugPrint("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

	iError = SelectAndInitDisplay(acDisplay);
	if (iError)
	{
		DebugPrint("SelectAndInitDisplay error!\n");
		return -1;
	}
	
	DebugPrint("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	iError = ShowNextPage();
	DebugPrint("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

    iError = AllInputDevicesInit();
	if (iError)
	{
		DebugPrint("AllInputDevicesInit error!\n");
		return -1;
	}

	DebugPrint("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    
	if (iError)
	{
		DebugPrint("Error to AllInputDevicesInit\n");
		return -1;
	}

	while (1)
	{
		DebugPrint("\r\nEnter 'n' to show next page, 'u' to show previous page, 'q' to exit: \n");
        while(GetInputEvent(&tInputEventTmp));
        //time_dif += tInputEventTmp.tTime

        if(tInputEventTmp.iVal == INPUT_VALUE_UP) {
            ShowPrePage();
        } else if(tInputEventTmp.iVal == INPUT_VALUE_DOWN) {
            ShowNextPage();
        } else if(tInputEventTmp.iVal == INPUT_VALUE_EXIT) {
            return 0;
        }
        
	}
	return 0;
}

