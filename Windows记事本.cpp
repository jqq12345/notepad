// Windows���±�.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Windows���±�.h"
#include"resource.h"
#include<shellapi.h>
#include<commdlg.h>  //OPENFILENAME�ṹ��  choosecolor����
#include <windows.h>
#include<windowsx.h>
#include<cstring>
#include<time.h>
//#include <afxwin.h>
//#include <afxdlgs.h>//�����滻�Ի���
/*******************************************************************************/
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM ) ;
BOOL CALLBACK DlgProcGetFileName(HWND, UINT, WPARAM, LPARAM);    //��ȡ�ļ���
int OpenFile(void);                    //���ļ�
BOOL FileToEditBox(HWND, LPSTR);        //�ļ����༭��
BOOL EditBoxToFile(HWND, LPCTSTR);    //�༭���ļ�
BOOL EditBoxToClip1(HWND);            //�༭�򵽼����壨���ƣ�
BOOL EditBoxToClip2(HWND);            //�༭�򵽼����壨���У�
BOOL ClipToEditBox(HWND);            //�����嵽�༭��
void CenterWindow(HWND hWnd);         //������ʾ�Ի���
BOOL EditBoxToEditBox(HWND hwnd);    //�ı������Сʱ��д�༭����ı�
COLORREF DoSelectColour(HWND hwnd);  //�����ɫ
int Printfun(HINSTANCE hins);        //��ӡ����
int PagePrint(void);                 //ҳ�����ú���
void FINDtext(void);                //���ҹ��ú���
int DragFile(WPARAM wPa);//��ק�ļ�������
BOOL ShowFileInfo(HWND hWnd, HDC hDC, HDROP hDropInfo);
/*********************************************************************************/
HWND CMainWnd::hMainWnd = NULL;
char szFile[255];
char szFileTitle[255];
char szBuffer[30];
int shows;
char a[1000]="https://cn.bing.com/search?q=%E8%8E%B7%E5%8F%96%E6%9C%89%E5%85%B3+windows+10+%E4%B8%AD%E7%9A%84%E8%AE%B0%E4%BA%8B%E6%9C%AC%E7%9A%84%E5%B8%AE%E5%8A%A9&filters=guid:%224466414-zh-hans-dia%22%20lang:%22zh-hans%22&form=T00032&ocid=HelpPane-BingIA";
char b[100]="start ";
HWND hWnd; //���ھ��
HINSTANCE    hInst;        //Ӧ�ó���ʵ�����
HWND        hList;        //�ļ��б����
HANDLE        hFile;        //�ļ����
HWND        hWndEdit;    //�༭����
HWND        hStatic;    //״̬�����
static TCHAR szAppName[] = TEXT( "demo" ) ;
static TCHAR Edit[5]=TEXT("edit");
bool check(char*, char*);            //�ж������ַ����Ƿ����
UINT FindDialogID;//����Ĳ��ҶԻ�����Ϣ
TCHAR findwhat[80]={0};//�����ַ���
TCHAR findwith[80]={0};//�滻�ַ���
static FINDREPLACE findtext;//���ҽṹ��
/**************************************************************/
 int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow )
  {
     HWND        hwnd ;
     MSG            msg ;
	 WNDCLASS    wndclass;

     //ע�ᴰ��
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
         MessageBox( NULL, TEXT("�޷�ע�ᴰ����!"), TEXT("����"), MB_OK | MB_ICONERROR ) ;
         return 0 ;
     }
     //��������
     hwnd = CreateWindow( szAppName, TEXT("�½����±�"), CS_VREDRAW|CS_HREDRAW|WS_OVERLAPPEDWINDOW,
       300,100,800,700,
         NULL, LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1)),  
        hInstance,  
        NULL) ;
	 //SetWindowText(hwnd,TEXT("xinjian"));
	 hWnd=hwnd;//ָ������������ھ��
	  if( hwnd == NULL )  
        return 0; 
	  hInst = hInstance;
	  shows=iCmdShow;
	  //HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(ID_MICON));
	  //SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	  //SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
     //��ʾ����
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
 //��Ϣ������
 LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
 {
     RECT rect ;
	 RECT ClientRect={0,0,0,0};
	 HWND dWnd;
	 LPCTSTR lpTemp=" ";    // ���ַ���    
	 char* p = NULL;

	static OPENFILENAME ofn;
    static bool flag = false;            //��û�д��ļ���flag=flase
    static bool flag2 = false;        //��û���½��ļ���flag2=false
    static bool flag3 = false;        //�����ж��ļ����Ƿ���˺�׺����IDM_SAVE �н����õ�
    static bool flag4 = false;        //�����ж��Ƿ񱣴����ļ�
	time_t timep;
	struct tm *ti;
	//����������ʽ�ı���
	static CHOOSEFONT cf;
    static LOGFONT  lf;
    static HDC  hdc,edithdc;
	HFONT  hFont;
	//����������ɫ
    static CHOOSECOLOR	cc;
    static COLORREF crCustColors[16];
	HBRUSH hbrush;
	static DWORD rgbCurrent;        // initial color selection


     switch( message )
     {
     case WM_CREATE:
		 GetClientRect(hwnd, &ClientRect);        //��ȡ�ͻ�����С
         hWndEdit = CreateWindow( Edit, NULL,
             WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | 
             ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
			 0, 0, 0, 0,hwnd, (HMENU)1, ((LPCREATESTRUCT) lParam) -> hInstance, NULL);
		 SetFocus(hWndEdit);                    //�༭���ý���
		 hFont=CreateFont(-16,0,0,0,400,FALSE,FALSE,FALSE,DEFAULT_CHARSET,FALSE,FALSE,FALSE,FALSE,(LPCSTR)"����");//����Ĭ������
		 SendMessage(hWndEdit,WM_SETFONT,(WPARAM)hFont,MAKELPARAM(TRUE,0));
		 flag2 = true;
		 flag4=false;
        DragAcceptFiles(hwnd, TRUE);
		
		//
		hdc = GetDC(hWnd);
        lf.lfHeight = -GetDeviceCaps(hdc, LOGPIXELSY) / 6;  // 12 points
        lstrcpy(lf.lfFaceName, TEXT("Lucida Sans Unicode"));
        ReleaseDC(hWnd, hdc);
        cf.lStructSize = sizeof(CHOOSEFONT); //��ʼ���ṹ���С
        cf.hwndOwner = hWnd;
        cf.lpLogFont = &lf;  //�߼�����ѡ��
        cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS; //ѡ����Ļ�����־


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
 
     case WM_SIZE: //�����ڴ�С�ı��ʱ�򣬱༭����״̬�����Ÿı�
		 GetClientRect(hwnd, &rect) ;
         MoveWindow( hWndEdit, 0, 0, rect.right, rect.bottom, TRUE ) ;
         return 0 ;
	 case WM_CTLCOLOREDIT://��ɫ�ı���Ϣ
		 {
			 SetTextColor((HDC)wParam, rgbCurrent);
			 return (COLORREF)rgbCurrent;
		 }
	 case WM_COMMAND:  
        {  
            // ȡ����ԴIdֵ  
            // ���ж��û�ѡ�����ĸ��˵���  
            switch(LOWORD(wParam))  
            {  
            case ID_NEW:  				 //�����ļ����Ի���
				SendMessage(hWndEdit, EM_SETSEL, 0, -1);                     //ѡ�������ı�
				SendMessage(hWndEdit, EM_REPLACESEL, wParam, (DWORD)lpTemp); //�ÿմ�����
				EnableWindow(hWndEdit, 1);            //����༭������
				SetFocus(hWndEdit);                    //�༭���ý���
				flag2 = true;                            //�½����ļ���flag2=true
				flag4 = false;                        //�½����ļ�����δ���棬flag4=false
            return 0;  

            case ID_NEWVIEW: 
				 GetClientRect(hwnd, &ClientRect);        //��ȡ�ͻ�����С
				//MessageBox(hwnd,"�½����ڡ�","��ʾ",MB_OK); 
				     dWnd = CreateWindow( szAppName, TEXT("�ޱ���-���±�"), WS_OVERLAPPEDWINDOW,
						 CW_USEDEFAULT, CW_USEDEFAULT,CW_USEDEFAULT, CW_USEDEFAULT,
						 NULL, LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU1)),hInst,NULL) ;
					 if( hwnd == NULL )  
						 return 0; 
					 //��ʾ����
					 ShowWindow( dWnd, shows ) ;
					 UpdateWindow( dWnd ) ;
				return 0;

           case ID_OPEN:
			   OpenFile();flag=true;
			   return 0;
			   
		   case ID_SAVE:
			//���༭������������������ļ�
			   
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
					ofn.lpstrFilter = "�ı��ļ�\0*.txt";
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.lpstrTitle = "���Ϊ...";
					ofn.hwndOwner = hwnd;    //�����ھ��
                    ofn.hwndOwner = hwnd;                //�����ھ��
                    EnableWindow(hWndEdit, 0);        //ʹ�༭���ܽ�������
                    GetSaveFileName(&ofn);
                    EnableWindow(hWndEdit, 1);        //ʹ�༭����Խ�������
                    p = szFile;
                    while (*p != '\0')
                    {
                        if (*p == '.')            //�ַ�������"."�ţ�˵���ļ������˺�׺
                        {
                            flag3 = true;        //�ļ������˺�׺, flag3=true
                            if (check(p, ".txt") || check(p, ".TXT"))    //�����׺��".txt" ��".TXT"                      
                                break;                                //OK������ѭ��
                            else                    //�����׺����".txt" ��".TXT"
                                strcat(p, ".txt");    //����ĩβ����".txt"
                        }
                        p++;
                    }                    
                    if (flag3 == false)
                    {                        
                        strcat(p, ".txt");            //�ļ���û�д���׺�����Ǹ������Ϻ�׺
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
					ofn.lpstrFilter = "�ı��ļ�\0*.txt";
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.lpstrTitle = "���Ϊ...";
					ofn.hwndOwner = hwnd;    //�����ھ��
					EnableWindow(hWndEdit, 0);        //ʹ�༭���ܽ�������
					GetSaveFileName(&ofn);	
					EnableWindow(hWndEdit, 1);        //ʹ�༭����Խ�������
					p = szFile;
					while (*p != '\0')
					{
						if (*p == '.')            //�ַ�������"."�ţ�˵���ļ������˺�׺
						{
							flag3 = true;        //����flag=false;
							if (check(p, ".txt") || check(p, ".TXT"))    //�����׺��".txt" ��".TXT"
								break;                                //OK������
							else                    //�����׺����".txt" ��".TXT"
								strcat(p, ".txt");    //����ĩβ����".txt"
						}				
						else                        //���ַ�����û��"."�ţ�˵���ļ���û�д���׺
							flag3 = false;                //����flag=false;
						p++;
					}
					if (flag3 == false)                    //��flag=false��˵���ļ���û�д���׺
					{
						strcat(szFile, ".txt");    //���ļ������Ϻ�׺".txt"
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
			case ID_SETTING: //ҳ������
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
            DestroyWindow(hList);                //�����ļ��б��
            return 0;
        }
        return 0;

     case WM_DESTROY:
		 DWORD dwFileSize;
		 dwFileSize =SendMessage(hWndEdit, WM_GETTEXTLENGTH, 0, 0);
		 if ((flag == true || flag2 == true) && flag4 == false) //����½����ļ�������ļ�
		 {                                             //�����ļ�û�б������
			 if (dwFileSize != 0)            //����༭�����ݲ�Ϊ��
			 {
				 int t;
				 t = ::MessageBox(NULL, "�ļ���δ���棬�Ƿ�Ҫ����?", "��ʾ", MB_YESNOCANCEL | MB_ICONINFORMATION);
				 if (t == IDYES)
				 {
					 SendMessage(hwnd, WM_COMMAND,ID_SAVEAS, 0);        //�����ļ�					 
					 DestroyWindow(hwnd);                            //���𴰿�
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
	DWORD    dwFileSize, dwByteRead;            //�ļ���С���ļ������ֽ�
    char* lpBufPtr = NULL;                    //�ļ�������	
    //���ļ�
    hFile = CreateFile(lpszFileName,            //�ļ���
        GENERIC_READ,            //ֻ���ж��ļ�����
        FILE_SHARE_READ,        //����ʽ�������
        NULL,
        OPEN_EXISTING,            //��һ���ļ�������ļ������ڣ�����ʧ��
        FILE_ATTRIBUTE_NORMAL, //�����ļ�
        (HANDLE)NULL);

    if (hFile != INVALID_HANDLE_VALUE)            //�����ļ��ɹ�
    {
        dwFileSize = GetFileSize(hFile, NULL);        //��ȡ�ļ���С
        if (dwFileSize != 0xFFFFFFFF)                //����ȡ�ļ���С�ɹ�
        {
            lpBufPtr = (char*)malloc(dwFileSize);    //�����ļ�������			
			if (lpBufPtr != NULL)                    //�������ļ��������ɹ�
            {
                //��ȡ�ļ�
                ReadFile(hFile,                    //�ļ����
                    (LPVOID)lpBufPtr,        //�ļ�������ָ��					
					dwFileSize,            //�ļ���С
                    &dwByteRead,            //��¼�����˶����ֽ�
                    NULL);                    //���ļ���ǰλ�ö�ȡ
                if (dwByteRead != 0)                //����ȡ�ļ��ɹ�
                {
                    //���ļ�д��༭��
                    SendMessage(hWndEdit, WM_SETTEXT, 0, (LPARAM)lpBufPtr);
                }
                else
                {
                    MessageBox(NULL, "�����ֽ���Ϊ0 ", "", 0);
                    free(lpBufPtr);
                    return 0;
                }
            }
            else
            {
                MessageBox(hwnd, "�ڴ����ʧ��", "", 0);
                return 0;
            }
        }
        else
        {
            MessageBox(hwnd, "��ȡ�ļ���Сʧ��", "", 0);
            return 0;
        }
    }
    else
    {
        MessageBox(hwnd, "���ļ�ʧ��", "", 0);
        return 0;		
	}
    CloseHandle(hFile);
    return 1;
}
BOOL EditBoxToFile(HWND hwnd, LPCTSTR lpszFileName)
{
    DWORD dwFileSize, dwByteWrite;        //�ļ���С����д����ֽ���
    char* lpBufPtr = NULL;                //�ļ�������
    hFile = CreateFile(lpszFileName,
        GENERIC_READ | GENERIC_WRITE,    //��д�ļ�
        FILE_SHARE_WRITE,                //д��ʽ������д
        NULL,
        CREATE_ALWAYS,                    //����һ���ļ�������ļ��Ѵ��ڽ�����д      
		FILE_ATTRIBUTE_NORMAL,            //�����ļ�
        (HANDLE)NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        //��ȡ�༭���е��ı��Ĵ�С
        dwFileSize = SendMessage(hWndEdit, WM_GETTEXTLENGTH, 0, 0);
        dwFileSize += 1;        //�ı����ȣ�������ʧ���һ���ַ�
        if (dwFileSize != 0)    //����ȡ�ļ���С�ɹ�
        {
            lpBufPtr = (char*)malloc(dwFileSize);        //�����ļ�������
            if (lpBufPtr != NULL)                        //�������ļ��������ɹ�
            {   //��ȡ�༭���е�����
                GetWindowText(hWndEdit, lpBufPtr, dwFileSize);
				//���ļ�����������д���ļ�
                WriteFile(hFile, (LPVOID)lpBufPtr, dwFileSize, &dwByteWrite, NULL);
                free(lpBufPtr);                //�ͷŻ�����
            }
            else
            {
                //                MessageBox(NULL,"�ڴ����ʧ��","",0);
                return 0;
            }
        }
        else
        {
            //            MessageBox(NULL,"�༭������Ϊ��","",0);
            CloseHandle(hFile);
            return 0;
        }
    }
    else
    {
        //        MessageBox(NULL,"�����ļ�ʧ��","",0);
        return 0;
    }
    CloseHandle(hFile);
    return 1;
}
BOOL EditBoxToEditBox(HWND hwnd)
{
	 DWORD dwFileSize;
	 char* lpBufPtr = NULL;                //�ļ�������
	//��ȡ�༭���е��ı��Ĵ�С
        dwFileSize = SendMessage(hWndEdit, WM_GETTEXTLENGTH, 0, 0);
        dwFileSize += 1;        //�ı����ȣ�������ʧ���һ���ַ�
        if (dwFileSize != 0)    //����ȡ�ļ���С�ɹ�
        {
            lpBufPtr = (char*)malloc(dwFileSize);        //�����ļ�������
            if (lpBufPtr != NULL)                        //�������ļ��������ɹ�
            {   //��ȡ�༭���е�����
                GetWindowText(hWndEdit, lpBufPtr, dwFileSize);
				SendMessage(hWndEdit, WM_SETTEXT, 0, (LPARAM)lpBufPtr);
			}
            else
            {
                //                MessageBox(NULL,"�ڴ����ʧ��","",0);
                return 0;
            }
        }
}

int OpenFile(void)//�򿪺���
  {
	TCHAR szOpenFileNames[MAX_PATH];//�ļ����·��
	OPENFILENAME ofn;//�򿪽ṹ��
	HANDLE hFile;
	LPCWSTR LP=NULL;
	DWORD	dwFileSize,dwByteRead;			//�ļ���С���ļ������ֽ�
	char *lpBufPtr;	//�ļ�������
	ZeroMemory(&ofn,sizeof(ofn));//�ڴ�����
	
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWndEdit;//ӵ�жԻ���ľ��
	ofn.lpstrFilter = TEXT("�����ļ�\0*.*\0�ı��ļ���.txt��\0*.TXT\0");
	ofn.nFilterIndex = 2;//Ĭ��ѡ������������
	ofn.lpstrFile =szOpenFileNames;//�����ṹ��ofn ������ļ����ռ�ĵ�ַ
	ofn.lpstrFile[0] = TEXT('\0');
	ofn.nMaxFile = sizeof(szOpenFileNames);
	ofn.lpstrTitle=_T("��");
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.hwndOwner=hWnd;
	ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FILEMUSTEXIST |OFN_PATHMUSTEXIST;
	
     GetOpenFileName(&ofn);
	  hFile=CreateFile(szOpenFileNames,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,
	     FILE_ATTRIBUTE_NORMAL,NULL);
		SetWindowText(hWnd,szOpenFileNames +ofn.nFileOffset);//�ı䴰�ڱ������� 
	   hFile=CreateFile(szOpenFileNames,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,(HANDLE)NULL);//���ļ�

	   if(hFile!=INVALID_HANDLE_VALUE)			//�����ļ��ɹ�
	   {
		   dwFileSize=GetFileSize(hFile,NULL);		//��ȡ�ļ���С
		   if(dwFileSize!=0xFFFFFFFF)				//����ȡ�ļ���С�ɹ�
		   {
			   lpBufPtr=(char *)malloc(dwFileSize);	//�����ļ�������
			   if(lpBufPtr!=NULL)					//�������ļ��������ɹ�
			   {
				   
				   ReadFile(hFile,(LPVOID)lpBufPtr,dwFileSize,&dwByteRead,NULL);//��ȡ�ļ�				
				
				 if(dwByteRead!=0)				//����ȡ�ļ��ɹ�
					   {  
						   SendMessageA(hWndEdit,WM_SETTEXT,0,(LPARAM)lpBufPtr);//ָ����ASCLL�뽫�ļ�д��༭����Ϊһ����±�Ĭ�����õ�ASCLL��
						   }
					   else
					   {
						   MessageBox(NULL,_T("�����ֽ���Ϊ 0 "),_T("֪ͨ"),0);
						   free(lpBufPtr);
						   return 0;
					   }
				   }
				   else
				   {
					   MessageBox(NULL,_T("�ڴ����ʧ��"),_T("֪ͨ"),0);
					   return 0;
				   }
			   }
			   else
			   {
				   MessageBox(NULL,_T("��ȡ�ļ���Сʧ��"),_T("֪ͨ"),0);
				   return 0;
			   }
		   }
		   else
		   {
			   MessageBox(NULL,_T("���ļ�ʧ��"),_T("֪ͨ"),0);
			   return 0;
	        }
	   free(lpBufPtr);
	   CloseHandle(hFile);
       return 0;}
//�༭�򵽼����壨���Ʋ�����
BOOL EditBoxToClip1(HWND hWnd)

{

    LPTSTR lpCopyStr, lpText;

    HANDLE hText;                //ȫ���ڴ���

    DWORD dPos;

    WORD wStart, wEnd;

    char* lpBufPtr;

    DWORD dwFileSize;            //�ı���С



    //�õ�ѡ�е��ı�λ��

    dPos = SendMessage(hWndEdit, EM_GETSEL, NULL, NULL);

    wEnd = HIWORD(dPos);                                    //��ʼλ��

    wStart = LOWORD(dPos);                                //����λ��



    //����ı������ı�����

    dwFileSize = SendMessage(hWndEdit, WM_GETTEXTLENGTH, 0, 0L);

    lpBufPtr = (char*)malloc(dwFileSize + 2);                //�����ı�������

    if (lpBufPtr != NULL)

    {

        // ��ȡ�༭���ı����ݱ��浽lpBufPtr����ȡ���������ı�������������ѡ�е��ı���

        SendMessage(hWndEdit, WM_GETTEXT, dwFileSize, (LPARAM)lpBufPtr);



        // ��ȡ����ѡ�е��ı�

        lpCopyStr = lpBufPtr + wStart;

        *(lpCopyStr + (wEnd - wStart)) = '\0';

    }

    else

    {

        MessageBox(hWnd, "�ڴ����ʧ��", "", MB_OK | MB_ICONEXCLAMATION);

        return 0;

    }



    if (!(hText = GlobalAlloc(GHND, wEnd - wStart + 1)))

    {

        MessageBox(hWnd, "ȫ���ڴ����ʧ��", "", MB_OK);

        return 0;

    }



    //�����ı���ȫ���ڴ�

    lpText = (LPTSTR)GlobalLock(hText);        //����ȫ���ڴ沢����ȫ���ڴ�ָ��

    lstrcpy(lpText, lpCopyStr);                //�����ı���ȫ���ڴ�

    GlobalUnlock(hText);                    //����ȫ���ڴ�



    //����Ϊ���������

    if (!OpenClipboard(hWnd))                //�򿪼�����

    {

        MessageBox(hWnd, "�������ʧ��!", "", MB_OK);

        return 0;

    }

    EmptyClipboard();                        //��ռ�����

    SetClipboardData(CF_TEXT, hText);        //���ü���������

    CloseClipboard();                        //�رռ�����



    hText = NULL;

    return 1;

}
//�༭�򵽼����壨���в�����
BOOL EditBoxToClip2(HWND hWnd)

{

    EditBoxToClip1(hWnd);

    SendMessage(hWndEdit, EM_REPLACESEL, 1, (LPARAM)"");    //�Կո�ȡ���༭��ѡ�е��ı�

    return 1;

}
//�����嵽�༭��
BOOL ClipToEditBox(HWND hWnd)
{
    LPTSTR    lpPasteStr, lpText;
    HANDLE    hText;
    DWORD    dPos;
    WORD    wStart, wEnd;
    //�õ��ı�λ��
    dPos = SendMessage(hWndEdit, EM_GETSEL, NULL, NULL);
    wEnd = HIWORD(dPos);
    wStart = LOWORD(dPos);
    if ((wEnd - wStart) >= 0)            //�ı����ȱ�>=0
    {
        if (!(IsClipboardFormatAvailable(CF_TEXT)))
        {
            /*������������������API����С��ʿ��������������������������

            ����    IsClipboardFormatAvailable

            ԭ��    BOOL IsClipboardFormatAvailable (UINT uFormat)

            ����    ���������Ƿ�֧��ĳ����������

            ����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE

            ��֧������: CF_TEXT����Ŀǰֻ֧�ּ���ı�ģʽ

            ������������������������������������������������������������*/
            MessageBox(hWnd, "�������ı�ģʽ������", "", MB_OK);
            return 0;
        }
        if (!OpenClipboard(hWnd))
        {
            MessageBox(hWnd, "�������ʧ��", "", MB_OK);
            return 0;
        }
        if (!(hText = GetClipboardData(CF_TEXT)))
        {
            MessageBox(hWnd, "�޷���ȡ����������", "", MB_OK);
            return 0;
        }
        lpText = (LPTSTR)GlobalLock(hText);                //�����ڴ沢�����ڴ�ָ��
        lpPasteStr = (LPTSTR)malloc(GlobalSize(hText));    //���ݼ��������ݿ����ı�������
        lstrcpy(lpPasteStr, lpText);                        //�ı�����
        GlobalUnlock(hText);                            //����ȫ���ڴ�
        //�ڱ༭���еĹ��λ�ò���lpPasteStr
        SendMessage(hWndEdit, EM_REPLACESEL, 1, (LPARAM)lpPasteStr);
    }

    else

        MessageBox(hWnd, "�ı�����λ�ô���", "", MB_OK);

    return 1;

}
bool check(char* a, char* b) //�ж������ַ����Ƿ����

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



    /*��������������������API ����С��ʿ��������������������������������

    API����DragQueryFile()��

    UINT DragQueryFile(HDROP hDrop,

     UINT iFile,

     LPTSTR lpszFile,

     UINT cch )



    ---- ����hDrop��ָ���϶��ļ��ṹ�ľ����

    ---- iFile ���Կ�ʼ�ı��ж��ļ�����ţ���Ϊһ�ο����϶�����ļ������˲���

    ---- ��ΪxFFFFFFFF�����������϶��ļ���������

    ---- lpszFile ��ָ���ļ����Ļ�������

    ---- cch ���ļ����������Ĵ�С�����ļ������ַ�����

    ��������������������������������������������������������������������*/



    // ����������ļ���Ŀ

    cFiles = DragQueryFile(hDropInfo, (UINT)0xFFFFFFFF, (LPTSTR)NULL, (UINT)NULL);



    if (cFiles > 1)        //���ʩ����ļ���ֻһ������ʾ������Ϣ:

    {

        //        MessageBox(NULL,"��������ļ����࣬�����²���","��ʾ",0);

        return FALSE;

    }



    else        //���������ļ�ֻ��һ��

    {

        DragQueryFile(hDropInfo, 0, lpszFile, sizeof(lpszFile));

        //��hDropInfo ��ȡ�ļ���Ϣ��ŵ�lpszFile

        //ע��DragQueryFile()����ȡ�õ���Ϣ���������ļ�����

        //     �������ļ����⻹�������ļ�·����������MessageBox()��������



        EnableWindow(hWndEdit, 1);                //ʹ�༭����Խ�������

        FileToEditBox(hwnd, lpszFile);            //�ļ����༭��!!!!!!!!!!!!

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
{                    //�༭���ý���
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
int Printfun(HINSTANCE hins)//��ӡ����
{  
	PRINTDLG pd;
    SecureZeroMemory(&pd,sizeof(pd));//����
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
	pd.hDevNames = NULL; //��ʾ��ӡ�Ի������û����趨ֽ�Ŵ�С�ȡ� 
	PrintDlg(&pd);
	
 return 0;}
int PagePrint(void)//ҳ�����ú���
{
	PAGESETUPDLG Page;
	ZeroMemory(&Page,sizeof(Page));
	Page.lStructSize =sizeof(Page);
	Page.hwndOwner=hWnd;
    PageSetupDlg(&Page);

	 return 0;}

 void FINDtext(void)//���ҹ��ú���
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
	findtext.lpTemplateName=_T("����");
	FindDialogID=RegisterWindowMessage(FINDMSGSTRING);
   
 }

int DragFile(WPARAM wPa)//��ק�ļ�������
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
		MessageBox(NULL,_T("��������ļ����࣬�����²���"),_T("֪ͨ"),0);
		return 0;
	 }
	else
	{
		DragQueryFile(hDrag,0,(LPSTR)lpszFile,sizeof(lpszFile));//�ļ���ק�ɹ����ȡ�ļ���
		SetWindowText(hWnd,(LPSTR)lpszFile);//�ı䴰�ڱ�������
	    hDrag=(HDROP)CreateFile((LPCSTR)lpszFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,(HANDLE)NULL);//���ļ�
		if(hDrag!=INVALID_HANDLE_VALUE)//���ļ��ɹ�
		 {
           DragFileSize=GetFileSize(hDrag,NULL);//��ȡ�ļ���С
		    if(DragFileSize!=0xFFFFFFFF)//��ȡ��С�ɹ�
			 {
              lpSave=(char *)malloc(DragFileSize);//�����ڴ�
			    if(lpSave!=NULL)//�����ڴ�ɹ�
				 {
                  ReadFile(hDrag,(LPVOID)lpSave,DragFileSize,&DragByteRead,NULL);//��ȡ�ļ����ڴ�
				   SendMessageA(hWndEdit,WM_SETTEXT,0,(LPARAM)lpSave);//д��༭����,�ٴ����ѣ���Ϊ���±�Ĭ����ASCLL������Ҫ��SendMessageA������������
				  }
				 else
				 {
			       MessageBox(NULL,_T("�����ڴ�ʧ�ܣ�"),_T("֪ͨ��"),0);
				   free(lpSave);
				   return 0;
				  }
			  }
			 else
			   {MessageBox(NULL,_T("��ȡ�ļ���Сʧ�ܣ�"),_T("֪ͨ��"),0);
			    return 0;
			    }
		  }
		else
		{
		 MessageBox(NULL,_T("��ק�ļ���ʧ�ܣ�"),_T("֪ͨ��"),0);
		 return 0;
		}
	}
 free(lpSave);//�ͷ��ڴ�
 CloseHandle(hDrag);//�ر��ļ����
 return 0;
 }
/*****************************************************************************************/

