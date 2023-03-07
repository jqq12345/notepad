#pragma once

#include "resource.h"
#include<commdlg.h>
#ifndef _MAIN_WND_H
#define _MAIN_WND_H
#include <Windows.h>
#include<tchar.h>
#define _FIND_REPLACE_H

/*********子窗口对话框********/
class CMainWnd
{
public:
static BOOL WINAPI Main_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static BOOL Main_OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
static void Main_OnCommand(HWND hWnd, int id, HWND hWndCtl, LPARAM lParam);
static void Main_OnClose(HWND hWnd);
private:
static HWND hMainWnd;   // 主窗口句柄
};

#endif