// Windows记事本.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Windows记事本.h"
#include"resource.h"
#include<shellapi.h>
#include<commdlg.h>  //OPENFILENAME结构体  choosecolor函数
#include <windows.h>
#include<windowsx.h>
#include<cstring>
#include<time.h>
//#include <afxwin.h>
//#include <afxdlgs.h>//查找替换对话框
/*******************************************************************************/
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM ) ;
BOOL CALLBACK DlgProcGetFileName(HWND, UINT, WPARAM, LPARAM);    //获取文件名
int OpenFile(void);                    //打开文件
BOOL FileToEditBox(HWND, LPSTR);        //文件到编辑框
BOOL EditBoxToFile(HWND, LPCTSTR);    //编辑框到文件
BOOL EditBoxToClip1(HWND);            //编辑框到剪贴板（复制）
BOOL EditBoxToClip2(HWND);            //编辑框到剪贴板（剪切）
BOOL ClipToEditBox(HWND);            //剪贴板到编辑框
void CenterWindow(HWND hWnd);         //居中显示对话框
BOOL EditBoxToEditBox(HWND hwnd);    //改变字体大小时重写编辑框的文本
COLORREF DoSelectColour(HWND hwnd);  //获得颜色
int Printfun(HINSTANCE hins);        //打印函数
int PagePrint(void);                 //页面设置函数
void FINDtext(void);                //查找公用函数
int DragFile(WPARAM wPa);//拖拽文件处理函数
BOOL ShowFileInfo(HWND hWnd, HDC hDC, HDROP hDropInfo);
/*********************************************************************************/
HWND CMainWnd::hMainWnd = NULL;
char szFile[255];
char szFileTitle[255];
char szBuffer[30];
int shows;
char a[1000]="https://cn.bing.com/search?q=%E8%8E%B7%E5%8F%96%E6%9C%89%E5%85%B3+windows+10+%E4%B8%AD%E7%9A%84%E8%AE%B0%E4%BA%8B%E6%9C%AC%E7%9A%84%E5%B8%AE%E5%8A%A9&filters=guid:%224466414-zh-hans-dia%22%20lang:%22zh-hans%22&form=T00032&ocid=HelpPane-BingIA";
char b[100]="start ";
HWND hWnd; //窗口句柄
HINSTANCE    hInst;        //应用程序实例句柄
HWND        hList;        //文件列表框句柄
HANDLE        hFile;        //文件句柄
HWND        hWndEdit;    //编辑框句柄
HWND        hStatic;    //状态栏句柄
static TCHAR szAppName[] = TEXT( "demo" ) ;
static TCHAR Edit[5]=TEXT("edit");
bool check(char*, char*);            //判断两个字符串是否相等
UINT FindDialogID;//定义的查找对话框消息
TCHAR findwhat[80]={0};//搜索字符串
TCHAR findwith[80]={0};//替换字符串
static FINDREPLACE findtext;//查找结构体
/**************************************************************/
 int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow )
  {
     HWND        hwnd ;
     MSG            msg ;
	 WNDCLASS    wndclass;

     //注册窗口
     wndclass.lpfnWndProc    = WndProc ;
     wndclass.style          = CS_HREDRAW;//CS_HREDRAW | CS_VREDRAW| DS_CENTER;
     wndclass.hInstance        = hInstance ;
     wndclass.cbClsExtra        = 0 ;
     wndclass.cbWndExtra        = 0 ;
     wndclass.hbrBackground    = (HBRUSH) GetStockObject(WHITE_BRUSH) ;
     wndclass.hCursor        = LoadCursor( NULL, IDC_ARROW ) ;
     wndclass.hIcon          = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON2));
     wndclass.lpszClassName    = szAppName ;
     wndclass.lpszMenuName    = NULL ;
     if( !RegisterClass(&wndclass) )
     {
         MessageBox( NULL, TEXT("无法注册窗口类!"), TEXT("错误"), MB_OK | MB_ICONERROR ) ;
         return 0 ;
     }
     //创建窗口
     hwnd = CreateWindow( szAppName, TEXT("新建记事本"), CS_VREDRAW|CS_HREDRAW|WS_OVERLAPPEDWINDOW,
       300,100,800,700,
         NULL, LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1)),  
        hInstance,  
        NULL) ;
	 //SetWindowText(hwnd,TEXT("xinjian"));
	 hWnd=hwnd;//指定程序的主窗口句柄
	  if( hwnd == NULL )  
        return 0; 
	  hInst = hInstance;
	  shows=iCmdShow;
	  //HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ID_MICON));
	  //SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	  //SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
     //显示窗口
     ShowWindow( hwnd, iCmdShow ) ;
     UpdateWindow( hwnd ) ;
 
     while( GetMessage(&msg, NULL, 0, 0) )
     {
         TranslateMessage( &msg ) ;
         DispatchMessage( &msg ) ;
     }
	 SendMessage(hWnd, WM_COMMAND, ID_NEW, 0L);
     return msg.wParam ;
 }
 //消息处理函数
 LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
 {
     RECT rect ;
	 RECT ClientRect={0,0,0,0};
	 HWND dWnd;
	 LPCTSTR lpTemp=" ";    // 空字符串    
	 char* p = NULL;

	static OPENFILENAME ofn;
    static bool flag = false;            //若没有打开文件，flag=flase
    static bool flag2 = false;        //若没有新建文件，flag2=false
    static bool flag3 = false;        //用来判断文件名是否带了后缀，在IDM_SAVE 中将会用到
    static bool flag4 = false;        //用来判断是否保存了文件
	time_t timep;
	struct tm *ti;
	//设置字体样式的变量
	static CHOOSEFONT cf;
    static LOGFONT  lf;
    static HDC  hdc,edithdc;
	HFONT  hFont;
	//设置字体颜色
    static CHOOSECOLOR	cc;
    static COLORREF crCustColors[16];
	HBRUSH hbrush;
	static DWORD rgbCurrent;        // initial color selection


     switch( message )
     {
     case WM_CREATE:
		 GetClientRect(hwnd, &ClientRect);        //获取客户区大小
         hWndEdit = CreateWindow( Edit, NULL,
             WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | 
             ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
			 0, 0, 0, 0,hwnd, (HMENU)1, ((LPCREATESTRUCT) lParam) -> hInstance, NULL);
		 SetFocus(hWndEdit);                    //编辑框获得焦点
		 hFont=CreateFont(-16,0,0,0,400,FALSE,FALSE,FALSE,DEFAULT_CHARSET,FALSE,FALSE,FALSE,FALSE,(LPCSTR)"宋体");//创建默认字体
		 SendMessage(hWndEdit,WM_SETFONT,(WPARAM)hFont,MAKELPARAM(TRUE,0));
		 flag2 = true;
		 flag4=false;
        DragAcceptFiles(hwnd, TRUE);
		
		//
		hdc = GetDC(hWnd);
        lf.lfHeight = -GetDeviceCaps(hdc, LOGPIXELSY) / 6;  // 12 points
        lstrcpy(lf.lfFaceName, TEXT("Lucida Sans Unicode"));
        ReleaseDC(hWnd, hdc);
        cf.lStructSize = sizeof(CHOOSEFONT); //初始化结构体大小
        cf.hwndOwner = hWnd;
        cf.lpLogFont = &lf;  //逻辑字体选择
        cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS; //选择屏幕字体标志


		edithdc = GetDC(hWndEdit);
		cc.lStructSize = sizeof(CHOOSECOLOR);
		cc.hwndOwner = NULL;
		cc.hInstance = NULL;
		cc.rgbResult = RGB(0x80, 0x80, 0x80);
		cc.lpCustColors = crCustColors;
		cc.Flags = CC_RGBINIT | CC_FULLOPEN;
		cc.lCustData = 0;
		cc.lpfnHook = NULL;
		cc.lpTemplateName = NULL;

         return 0 ;
 
     case WM_SIZE: //当窗口大小改变的时候，编辑框与状态栏跟着改变
		 GetClientRect(hwnd, &rect) ;
         MoveWindow( hWndEdit, 0, 0, rect.right, rect.bottom, TRUE ) ;
         return 0 ;
	 case WM_CTLCOLOREDIT://颜色改变消息
		 {
			 SetTextColor((HDC)wParam, rgbCurrent);
			 return (COLORREF)rgbCurrent;
		 }
	 case WM_COMMAND:  
        {  
            // 取出资源Id值  
            // 并判断用户选择了哪个菜单项  
            switch(LOWORD(wParam))  
            {  
            case ID_NEW:  				 //弹出文件名对话框
				SendMessage(hWndEdit, EM_SETSEL, 0, -1);                     //选中所有文本
				SendMessage(hWndEdit, EM_REPLACESEL, wParam, (DWORD)lpTemp); //用空串代码
				EnableWindow(hWndEdit, 1);            //允许编辑框输入
				SetFocus(hWndEdit);                    //编辑框获得焦点
				flag2 = true;                            //新建了文件，flag2=true
				flag4 = false;                        //新建了文件，还未保存，flag4=false
            return 0;  

            case ID_NEWVIEW: 
				 GetClientRect(hwnd, &ClientRect);        //获取客户区大小
				//MessageBox(hwnd,"新建窗口。","提示",MB_OK); 
				     dWnd = CreateWindow( szAppName, TEXT("无标题-记事本"), WS_OVERLAPPEDWINDOW,
						 CW_USEDEFAULT, CW_USEDEFAULT,CW_USEDEFAULT, CW_USEDEFAULT,
						 NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU1)),hInst,NULL) ;
					 if( hwnd == NULL )  
						 return 0; 
					 //显示窗口
					 ShowWindow( dWnd, shows ) ;
					 UpdateWindow( dWnd ) ;
				return 0;

           case ID_OPEN:
			   OpenFile();flag=true;
			   return 0;
			   
		   case ID_SAVE:
			//将编辑框中所有内容输出到文件
			   
            if (flag == true || flag2 == true)
            {
                if (flag4 == false)
                {
					ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
					ofn.lpstrFile = szFile;
					ofn.nMaxFile = sizeof(szFile);
					ofn.lpstrFileTitle = NULL;	
					ofn.lpstrInitialDir = NULL;
					ofn.nMaxFileTitle = NULL;
					ofn.lpstrFilter = "文本文件\0*.txt";
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.lpstrTitle = "另存为...";
					ofn.hwndOwner = hwnd;    //父窗口句柄
                    ofn.hwndOwner = hwnd;                //父窗口句柄
                    EnableWindow(hWndEdit, 0);        //使编辑框不能接受输入
                    GetSaveFileName(&ofn);
                    EnableWindow(hWndEdit, 1);        //使编辑框可以接受输入
                    p = szFile;
                    while (*p != '\0')
                    {
                        if (*p == '.')            //字符串里有"."号，说明文件名带了后缀
                        {
                            flag3 = true;        //文件名带了后缀, flag3=true
                            if (check(p, ".txt") || check(p, ".TXT"))    //如果后缀是".txt" 或".TXT"                      
                                break;                                //OK，跳出循环
                            else                    //如果后缀不是".txt" 或".TXT"
                                strcat(p, ".txt");    //在其末尾加上".txt"
                        }
                        p++;
                    }                    
                    if (flag3 == false)
                    {                        
                        strcat(p, ".txt");            //文件名没有带后缀，我们给它加上后缀
                    }                    
                }
                EditBoxToFile(hwnd, szFile);
                flag4 = true;
            }
			flag3 = false;
			return 0;


			case ID_SAVEAS:  
				if (flag == true || flag2 == true)
				{             
					ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
					ofn.lpstrFile = szFile;
					ofn.nMaxFile = sizeof(szFile);
					ofn.lpstrFileTitle = szFileTitle;	
					ofn.lpstrInitialDir = NULL;
					ofn.nMaxFileTitle = NULL;
					ofn.lpstrFilter = "文本文件\0*.txt";
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.lpstrTitle = "另存为...";
					ofn.hwndOwner = hwnd;    //父窗口句柄
					EnableWindow(hWndEdit, 0);        //使编辑框不能接受输入
					GetSaveFileName(&ofn);	
					EnableWindow(hWndEdit, 1);        //使编辑框可以接受输入
					p = szFile;
					while (*p != '\0')
					{
						if (*p == '.')            //字符串里有"."号，说明文件名带了后缀
						{
							flag3 = true;        //设置flag=false;
							if (check(p, ".txt") || check(p, ".TXT"))    //如果后缀是".txt" 或".TXT"
								break;                                //OK，返回
							else                    //如果后缀不是".txt" 或".TXT"
								strcat(p, ".txt");    //在其末尾加上".txt"
						}				
						else                        //若字符串里没有"."号，说明文件名没有带后缀
							flag3 = false;                //设置flag=false;
						p++;
					}
					if (flag3 == false)                    //若flag=false，说明文件名没有带后缀
					{
						strcat(szFile, ".txt");    //给文件名加上后缀".txt"
						flag3 = true;
					}
					if (flag3 == true)
					{					
						EditBoxToFile(hwnd, szFile);
						flag4 = true;
					}
				}
				return 0;

			case ID_EXIT:
				 PostQuitMessage(0) ;
				break;
			case ID_SETTING: //页面设置
				PagePrint();
				return 0;
			case ID_PRINT:
				Printfun(hInst);
				return 0;
			case ID_UNDO:
				SendMessage(hWndEdit, WM_UNDO, 0, 0);
				return 0;
			case ID_COPY:
				EditBoxToClip1(hwnd);
				return 0;
			case ID_CUT:
				EditBoxToClip2(hwnd);
				return 0;
			case ID_STICK:
				ClipToEditBox(hwnd);
				return 0;
			case ID_DELE:
				SendMessage(hWndEdit,WM_CLEAR , 0, 0);
				return 0;
			case ID_SELALL:
				SendMessage(hWndEdit, EM_SETSEL, 0, LPARAM(GetWindowTextLength(hWndEdit)));		
			case ID_REPLACE:
				return 0;
			case ID_TIME: // Problems!
				time(&timep);
				ti=localtime(&timep);
				wsprintf(szBuffer, TEXT("%d/%d/%d %02d:%02d:%02d"), 1900+ti->tm_year, 1+ti->tm_mon,ti->tm_mday, ti->tm_min, ti->tm_sec);
				SendMessage(hWndEdit, EM_REPLACESEL, 1, (LPARAM)szBuffer);
				return 0;
			case ID_FONT:
				 if (!ChooseFont(&cf)) return 0;				   
				 hFont = CreateFontIndirect(&lf);
				 ::SendMessage(hWndEdit, WM_SETFONT, (WPARAM)hFont, 0);
				 EditBoxToEditBox(hWndEdit);
				 return 0;
			case ID_SETCOLOR:
				if (!ChooseColor(&cc)) return 0;	 
				  hbrush=CreateSolidBrush (cc.rgbResult);
                  rgbCurrent = cc.rgbResult; 
				  return 0;

				 

			case ID_HEPL:
				strcat(b,a);
				system(b);
				return 0;

			case ID_RELATIVE:
				::DialogBox(NULL, MAKEINTRESOURCE(IDD_ABOUTBOX ), hWnd, CMainWnd::Main_Proc);			
				return 0;
			default: 
				return 0;
            }  
		}
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            DestroyWindow(hList);                //消毁文件列表框
            return 0;
        }
        return 0;

     case WM_DESTROY:
		 DWORD dwFileSize;
		 dwFileSize =SendMessage(hWndEdit, WM_GETTEXTLENGTH, 0, 0);
		 if ((flag == true || flag2 == true) && flag4 == false) //如果新建了文件或打开了文件
		 {                                             //并且文件没有被保存过
			 if (dwFileSize != 0)            //如果编辑框内容不为空
			 {
				 int t;
				 t = ::MessageBox(NULL, "文件还未保存，是否要保存?", "提示", MB_YESNOCANCEL | MB_ICONINFORMATION);
				 if (t == IDYES)
				 {
					 SendMessage(hwnd, WM_COMMAND,ID_SAVEAS, 0);        //保存文件					 
					 DestroyWindow(hwnd);                            //毁灭窗口
					 return 0;
				 }
                else if (t == IDCANCEL)
                {
                    return 0;
                }
            }
        }
		 PostQuitMessage(0) ;
         return 0 ;
     }
     return DefWindowProc( hwnd, message, wParam, lParam ) ;
 }
 BOOL FileToEditBox(HWND hwnd, LPSTR lpszFileName)
{
	DWORD    dwFileSize, dwByteRead;            //文件大小，文件读入字节
    char* lpBufPtr = NULL;                    //文件缓冲区	
    //打开文件
    hFile = CreateFile(lpszFileName,            //文件名
        GENERIC_READ,            //只进行读文件操作
        FILE_SHARE_READ,        //读方式：共享读
        NULL,
        OPEN_EXISTING,            //打开一个文件，如果文件不存在，函数失败
        FILE_ATTRIBUTE_NORMAL, //常规文件
        (HANDLE)NULL);

    if (hFile != INVALID_HANDLE_VALUE)            //若打开文件成功
    {
        dwFileSize = GetFileSize(hFile, NULL);        //读取文件大小
        if (dwFileSize != 0xFFFFFFFF)                //若读取文件大小成功
        {
            lpBufPtr = (char*)malloc(dwFileSize);    //开辟文件缓冲区			
			if (lpBufPtr != NULL)                    //若开辟文件缓冲区成功
            {
                //读取文件
                ReadFile(hFile,                    //文件句柄
                    (LPVOID)lpBufPtr,        //文件缓冲区指针					
					dwFileSize,            //文件大小
                    &dwByteRead,            //记录读入了多少字节
                    NULL);                    //从文件当前位置读取
                if (dwByteRead != 0)                //若读取文件成功
                {
                    //将文件写入编辑框
                    SendMessage(hWndEdit, WM_SETTEXT, 0, (LPARAM)lpBufPtr);
                }
                else
                {
                    MessageBox(NULL, "读入字节数为0 ", "", 0);
                    free(lpBufPtr);
                    return 0;
                }
            }
            else
            {
                MessageBox(hwnd, "内存分配失败", "", 0);
                return 0;
            }
        }
        else
        {
            MessageBox(hwnd, "获取文件大小失败", "", 0);
            return 0;
        }
    }
    else
    {
        MessageBox(hwnd, "打开文件失败", "", 0);
        return 0;		
	}
    CloseHandle(hFile);
    return 1;
}
BOOL EditBoxToFile(HWND hwnd, LPCTSTR lpszFileName)
{
    DWORD dwFileSize, dwByteWrite;        //文件大小，已写入的字节数
    char* lpBufPtr = NULL;                //文件缓冲区
    hFile = CreateFile(lpszFileName,
        GENERIC_READ | GENERIC_WRITE,    //读写文件
        FILE_SHARE_WRITE,                //写方式：共享写
        NULL,
        CREATE_ALWAYS,                    //创建一个文件，如果文件已存在将被重写      
		FILE_ATTRIBUTE_NORMAL,            //常规文件
        (HANDLE)NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        //读取编辑框中的文本的大小
        dwFileSize = SendMessage(hWndEdit, WM_GETTEXTLENGTH, 0, 0);
        dwFileSize += 1;        //文本长度＋，否则丢失最后一个字符
        if (dwFileSize != 0)    //若读取文件大小成功
        {
            lpBufPtr = (char*)malloc(dwFileSize);        //开辟文件缓冲区
            if (lpBufPtr != NULL)                        //若开辟文件缓冲区成功
            {   //读取编辑框中的内容
                GetWindowText(hWndEdit, lpBufPtr, dwFileSize);
				//将文件缓冲区内容写入文件
                WriteFile(hFile, (LPVOID)lpBufPtr, dwFileSize, &dwByteWrite, NULL);
                free(lpBufPtr);                //释放缓冲区
            }
            else
            {
                //                MessageBox(NULL,"内存分配失败","",0);
                return 0;
            }
        }
        else
        {
            //            MessageBox(NULL,"编辑框内容为空","",0);
            CloseHandle(hFile);
            return 0;
        }
    }
    else
    {
        //        MessageBox(NULL,"创建文件失败","",0);
        return 0;
    }
    CloseHandle(hFile);
    return 1;
}
BOOL EditBoxToEditBox(HWND hwnd)
{
	 DWORD dwFileSize;
	 char* lpBufPtr = NULL;                //文件缓冲区
	//读取编辑框中的文本的大小
        dwFileSize = SendMessage(hWndEdit, WM_GETTEXTLENGTH, 0, 0);
        dwFileSize += 1;        //文本长度＋，否则丢失最后一个字符
        if (dwFileSize != 0)    //若读取文件大小成功
        {
            lpBufPtr = (char*)malloc(dwFileSize);        //开辟文件缓冲区
            if (lpBufPtr != NULL)                        //若开辟文件缓冲区成功
            {   //读取编辑框中的内容
                GetWindowText(hWndEdit, lpBufPtr, dwFileSize);
				SendMessage(hWndEdit, WM_SETTEXT, 0, (LPARAM)lpBufPtr);
			}
            else
            {
                //                MessageBox(NULL,"内存分配失败","",0);
                return 0;
            }
        }
}

int OpenFile(void)//打开函数
  {
	TCHAR szOpenFileNames[MAX_PATH];//文件最大路径
	OPENFILENAME ofn;//打开结构体
	HANDLE hFile;
	LPCWSTR LP=NULL;
	DWORD	dwFileSize,dwByteRead;			//文件大小，文件读入字节
	char *lpBufPtr;	//文件缓冲区
	ZeroMemory(&ofn,sizeof(ofn));//内存置零
	
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWndEdit;//拥有对话框的句柄
	ofn.lpstrFilter = TEXT("所有文件\0*.*\0文本文件（.txt）\0*.TXT\0");
	ofn.nFilterIndex = 2;//默认选择过滤器的序号
	ofn.lpstrFile =szOpenFileNames;//传给结构体ofn 来存放文件名空间的地址
	ofn.lpstrFile[0] = TEXT('\0');
	ofn.nMaxFile = sizeof(szOpenFileNames);
	ofn.lpstrTitle=_T("打开");
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.hwndOwner=hWnd;
	ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FILEMUSTEXIST |OFN_PATHMUSTEXIST;
	
     GetOpenFileName(&ofn);
	  hFile=CreateFile(szOpenFileNames,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
	     FILE_ATTRIBUTE_NORMAL,NULL);
		SetWindowText(hWnd,szOpenFileNames +ofn.nFileOffset);//改变窗口标题内容 
	   hFile=CreateFile(szOpenFileNames,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,(HANDLE)NULL);//打开文件

	   if(hFile!=INVALID_HANDLE_VALUE)			//若打开文件成功
	   {
		   dwFileSize=GetFileSize(hFile,NULL);		//读取文件大小
		   if(dwFileSize!=0xFFFFFFFF)				//若读取文件大小成功
		   {
			   lpBufPtr=(char *)malloc(dwFileSize);	//开辟文件缓冲区
			   if(lpBufPtr!=NULL)					//若开辟文件缓冲区成功
			   {
				   
				   ReadFile(hFile,(LPVOID)lpBufPtr,dwFileSize,&dwByteRead,NULL);//读取文件				
				
				 if(dwByteRead!=0)				//若读取文件成功
					   {  
						   SendMessageA(hWndEdit,WM_SETTEXT,0,(LPARAM)lpBufPtr);//指定用ASCLL码将文件写入编辑框，因为一般记事本默认是用的ASCLL码
						   }
					   else
					   {
						   MessageBox(NULL,_T("读入字节数为 0 "),_T("通知"),0);
						   free(lpBufPtr);
						   return 0;
					   }
				   }
				   else
				   {
					   MessageBox(NULL,_T("内存分配失败"),_T("通知"),0);
					   return 0;
				   }
			   }
			   else
			   {
				   MessageBox(NULL,_T("获取文件大小失败"),_T("通知"),0);
				   return 0;
			   }
		   }
		   else
		   {
			   MessageBox(NULL,_T("打开文件失败"),_T("通知"),0);
			   return 0;
	        }
	   free(lpBufPtr);
	   CloseHandle(hFile);
       return 0;}
//编辑框到剪贴板（复制操作）
BOOL EditBoxToClip1(HWND hWnd)

{

    LPTSTR lpCopyStr, lpText;

    HANDLE hText;                //全局内存句柄

    DWORD dPos;

    WORD wStart, wEnd;

    char* lpBufPtr;

    DWORD dwFileSize;            //文本大小



    //得到选中的文本位置

    dPos = SendMessage(hWndEdit, EM_GETSEL, NULL, NULL);

    wEnd = HIWORD(dPos);                                    //开始位置

    wStart = LOWORD(dPos);                                //结束位置



    //获得文本框中文本长度

    dwFileSize = SendMessage(hWndEdit, WM_GETTEXTLENGTH, 0, 0L);

    lpBufPtr = (char*)malloc(dwFileSize + 2);                //开辟文本缓冲区

    if (lpBufPtr != NULL)

    {

        // 读取编辑框文本内容保存到lpBufPtr（读取的是所有文本，而不仅仅是选中的文本）

        SendMessage(hWndEdit, WM_GETTEXT, dwFileSize, (LPARAM)lpBufPtr);



        // 截取我们选中的文本

        lpCopyStr = lpBufPtr + wStart;

        *(lpCopyStr + (wEnd - wStart)) = '\0';

    }

    else

    {

        MessageBox(hWnd, "内存分配失败", "", MB_OK | MB_ICONEXCLAMATION);

        return 0;

    }



    if (!(hText = GlobalAlloc(GHND, wEnd - wStart + 1)))

    {

        MessageBox(hWnd, "全局内存分配失败", "", MB_OK);

        return 0;

    }



    //复制文本到全局内存

    lpText = (LPTSTR)GlobalLock(hText);        //锁定全局内存并返回全局内存指针

    lstrcpy(lpText, lpCopyStr);                //复制文本到全局内存

    GlobalUnlock(hText);                    //解锁全局内存



    //以下为剪贴板操作

    if (!OpenClipboard(hWnd))                //打开剪贴板

    {

        MessageBox(hWnd, "剪贴板打开失败!", "", MB_OK);

        return 0;

    }

    EmptyClipboard();                        //清空剪贴板

    SetClipboardData(CF_TEXT, hText);        //设置剪贴板内容

    CloseClipboard();                        //关闭剪贴板



    hText = NULL;

    return 1;

}
//编辑框到剪贴板（剪切操作）
BOOL EditBoxToClip2(HWND hWnd)

{

    EditBoxToClip1(hWnd);

    SendMessage(hWndEdit, EM_REPLACESEL, 1, (LPARAM)"");    //以空格取代编辑框被选中的文本

    return 1;

}
//剪贴板到编辑框
BOOL ClipToEditBox(HWND hWnd)
{
    LPTSTR    lpPasteStr, lpText;
    HANDLE    hText;
    DWORD    dPos;
    WORD    wStart, wEnd;
    //得到文本位置
    dPos = SendMessage(hWndEdit, EM_GETSEL, NULL, NULL);
    wEnd = HIWORD(dPos);
    wStart = LOWORD(dPos);
    if ((wEnd - wStart) >= 0)            //文本长度必>=0
    {
        if (!(IsClipboardFormatAvailable(CF_TEXT)))
        {
            /*－－－－－－－－－API函数小帖士－－－－－－－－－－－－－

            名称    IsClipboardFormatAvailable

            原型    BOOL IsClipboardFormatAvailable (UINT uFormat)

            描述    检测剪贴板是否支持某种数据类型

            返回值：成功返回TRUE，失败返回FALSE

            现支持类型: CF_TEXT，即目前只支持检测文本模式

            －－－－－－－－－－－－－－－－－－－－－－－－－－－－－－*/
            MessageBox(hWnd, "剪贴板文本模式不可用", "", MB_OK);
            return 0;
        }
        if (!OpenClipboard(hWnd))
        {
            MessageBox(hWnd, "剪贴板打开失败", "", MB_OK);
            return 0;
        }
        if (!(hText = GetClipboardData(CF_TEXT)))
        {
            MessageBox(hWnd, "无法读取剪贴板数据", "", MB_OK);
            return 0;
        }
        lpText = (LPTSTR)GlobalLock(hText);                //锁定内存并返回内存指针
        lpPasteStr = (LPTSTR)malloc(GlobalSize(hText));    //根据剪贴板内容开辟文本缓冲区
        lstrcpy(lpPasteStr, lpText);                        //文本复制
        GlobalUnlock(hText);                            //解锁全局内存
        //在编辑框中的光标位置插入lpPasteStr
        SendMessage(hWndEdit, EM_REPLACESEL, 1, (LPARAM)lpPasteStr);
    }

    else

        MessageBox(hWnd, "文本插入位置错误", "", MB_OK);

    return 1;

}
bool check(char* a, char* b) //判断两个字符串是否相等

{

    int i;

    int j = strlen(b);

    for (i = 0; i < j; i++)

    {

        if (*a == *b)

        {

            a++;

            b++;

        }

        else

            return 0;

    }

    return 1;

}

BOOL ShowFileInfo(HWND hwnd, HDC hDC, HDROP hDropInfo)

{

    WORD cFiles;

    char lpszFile[255];



    /*－－－－－－－－－－API 函数小帖士－－－－－－－－－－－－－－－－

    API函数DragQueryFile()：

    UINT DragQueryFile(HDROP hDrop,

     UINT iFile,

     LPTSTR lpszFile,

     UINT cch )



    ---- 其中hDrop是指向拖动文件结构的句柄；

    ---- iFile 是以开始的被托动文件的序号，因为一次可能拖动多个文件。当此参数

    ---- 设为xFFFFFFFF，则函数返回拖动文件的数量；

    ---- lpszFile 是指向文件名的缓冲区；

    ---- cch 是文件名缓冲区的大小，即文件名的字符数。

    －－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－－*/



    // 返回拖入的文件数目

    cFiles = DragQueryFile(hDropInfo, (UINT)0xFFFFFFFF, (LPTSTR)NULL, (UINT)NULL);



    if (cFiles > 1)        //如果施入的文件不只一个，显示如下信息:

    {

        //        MessageBox(NULL,"你拖入的文件过多，请重新操作","提示",0);

        return FALSE;

    }



    else        //如果拖入的文件只有一个

    {

        DragQueryFile(hDropInfo, 0, lpszFile, sizeof(lpszFile));

        //从hDropInfo 获取文件信息存放到lpszFile

        //注：DragQueryFile()函数取得的信息是完整的文件名，

        //     即除了文件名外还包含了文件路径，可以用MessageBox()函数测试



        EnableWindow(hWndEdit, 1);                //使编辑框可以接受输入

        FileToEditBox(hwnd, lpszFile);            //文件到编辑框!!!!!!!!!!!!

    }

    return TRUE;

}
BOOL WINAPI CMainWnd::Main_Proc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )

{
switch (uMsg)
{
HANDLE_MSG(hWnd, WM_INITDIALOG, Main_OnInitDialog);

HANDLE_MSG(hWnd, WM_COMMAND, Main_OnCommand);

HANDLE_MSG(hWnd, WM_CLOSE, Main_OnClose);
}
return false;
}
BOOL CMainWnd::Main_OnInitDialog( HWND hWnd, HWND hWndFocus, LPARAM lParam )
{
	CenterWindow(hWnd);
	return TRUE;
}
void CMainWnd::Main_OnCommand( HWND hWnd, int id, HWND hWndCtl, LPARAM lParam )
{                    //编辑框获得焦点
	::EndDialog(hWnd, NULL);
}
void CMainWnd::Main_OnClose( HWND hWnd )
{
::EndDialog(hWnd, NULL);
}
void CenterWindow(HWND hWnd)
{
    HWND hParentOrOwner;
    RECT rc, rc2;
    int    x,y;
    if((hParentOrOwner=GetParent(hWnd))==NULL)
    {
        SystemParametersInfo(SPI_GETWORKAREA,0,&rc,0);
    }
    else
    {
        GetClientRect(hParentOrOwner, &rc);
    }
    GetWindowRect(hWnd, &rc2);
    x = ((rc.right-rc.left) - (rc2.right-rc2.left)) / 2 +rc.left;
    y = ((rc.bottom-rc.top) - (rc2.bottom-rc2.top)) / 2 +rc.top;
    SetWindowPos(hWnd,HWND_TOP,x, y,0, 0,SWP_NOSIZE);
}
int Printfun(HINSTANCE hins)//打印函数
{  
	PRINTDLG pd;
    SecureZeroMemory(&pd,sizeof(pd));//清零
	pd.lStructSize = sizeof(PRINTDLG); 
	pd.hwndOwner=hWnd;
	pd.hDevNames=NULL;
	pd.Flags = PD_RETURNDC; 
	pd.hDC = NULL; 
	pd.hInstance=hins ;
	pd.nMaxPage = 10; 
	pd.nMinPage = 1; 
	pd.nFromPage = 1; 
	pd.nToPage = 1; 
	pd.nCopies = 1; 
	pd.hDevMode = NULL; 
	pd.hDevNames = NULL; //显示打印对话框，由用户来设定纸张大小等。 
	PrintDlg(&pd);
	
 return 0;}
int PagePrint(void)//页面设置函数
{
	PAGESETUPDLG Page;
	ZeroMemory(&Page,sizeof(Page));
	Page.lStructSize =sizeof(Page);
	Page.hwndOwner=hWnd;
    PageSetupDlg(&Page);

	 return 0;}

 void FINDtext(void)//查找公用函数
 {
	
    ZeroMemory(&findtext,sizeof(findtext));
	findtext.lStructSize=sizeof(findtext);
	findtext.hwndOwner=hWnd;
	findtext.Flags=FR_DOWN|FR_HIDEWHOLEWORD|FR_ENABLEHOOK; 
	findtext.lpstrFindWhat=findwhat;
	findtext.lpstrReplaceWith=findwith;
	findtext.wFindWhatLen=80;
	findtext.wReplaceWithLen=80;
	findtext.lpfnHook=(LPFRHOOKPROC)NULL;
	findtext.lpTemplateName=_T("查找");
	FindDialogID=RegisterWindowMessage(FINDMSGSTRING);
   
 }

int DragFile(WPARAM wPa)//拖拽文件处理函数
{    
	 HDROP hDrag;
	 hDrag = (HDROP)wPa;
	 WORD cFiles;
	 char lpszFile[255];
	 DWORD DragFileSize,DragByteRead;
	 char *lpSave;
	 cFiles = DragQueryFile( hDrag,(UINT)0xFFFFFFFF,(LPTSTR)NULL,(UINT)NULL);
	if(cFiles>1)
	 {
		MessageBox(NULL,_T("你拖入的文件过多，请重新操作"),_T("通知"),0);
		return 0;
	 }
	else
	{
		DragQueryFile(hDrag,0,(LPSTR)lpszFile,sizeof(lpszFile));//文件拖拽成功后获取文件名
		SetWindowText(hWnd,(LPSTR)lpszFile);//改变窗口标题内容
	    hDrag=(HDROP)CreateFile((LPCSTR)lpszFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,(HANDLE)NULL);//打开文件
		if(hDrag!=INVALID_HANDLE_VALUE)//打开文件成功
		 {
           DragFileSize=GetFileSize(hDrag,NULL);//获取文件大小
		    if(DragFileSize!=0xFFFFFFFF)//获取大小成功
			 {
              lpSave=(char *)malloc(DragFileSize);//开辟内存
			    if(lpSave!=NULL)//开辟内存成功
				 {
                  ReadFile(hDrag,(LPVOID)lpSave,DragFileSize,&DragByteRead,NULL);//读取文件到内存
				   SendMessageA(hWndEdit,WM_SETTEXT,0,(LPARAM)lpSave);//写入编辑框内,再次提醒，因为记事本默认是ASCLL，所以要用SendMessageA，否则会出乱码
				  }
				 else
				 {
			       MessageBox(NULL,_T("申请内存失败！"),_T("通知！"),0);
				   free(lpSave);
				   return 0;
				  }
			  }
			 else
			   {MessageBox(NULL,_T("获取文件大小失败！"),_T("通知！"),0);
			    return 0;
			    }
		  }
		else
		{
		 MessageBox(NULL,_T("拖拽文件打开失败！"),_T("通知！"),0);
		 return 0;
		}
	}
 free(lpSave);//释放内存
 CloseHandle(hDrag);//关闭文件句柄
 return 0;
 }
/*****************************************************************************************/

