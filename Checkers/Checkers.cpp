/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*------------------------------------------------------------------------
   	Checkers.CPP -- Project: Checkers Game

	Polytechnic Institute of New York University
	Spring 2012 . CS 6613 : Artificial Intelligence
    Instructor: E. K. Wong
	Student: Yanbin Luo <yluo04@students.poly.edu>
 -----------------------------------------------------------------------*/

// Checkers.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Checkers.h"
#include <string>
#include <math.h>
#include <ctime>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CHECKERS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHECKERS));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHECKERS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CHECKERS);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   //hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   //   CW_USEDEFAULT, CW_USEDEFAULT, 600, 600, NULL, NULL, hInstance, NULL);
   
   hWnd = CreateWindow( szWindowClass,	// window class name
						szTitle,		// window caption
						WS_CAPTION|WS_SYSMENU|WS_VISIBLE|WS_MINIMIZEBOX, // window style
					    100,			// initial x position //CW_USEDEFAULT
					    50,				// initial y position //CW_USEDEFAULT
					    600,			// initial x size
					    645,			// initial y size
					    NULL,			// parent window handle
					    NULL,	        // window menu handle
					    hInstance,		// program instance handle
					    NULL) ; 		// creation parameters
   if (!hWnd)
   {
      return FALSE;
   }
   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HPEN oldPen, hPen;
	HBRUSH oldBrush, pieceBrush[2], boardBrush[3];
	RECT rect;

	static HWND msgbox;
	static bool show = true;

	static int step = 0;
	static PieceAct rc = { -1, -1, -1, -1, -1, -1, -1, -1};
	static enum State state = Start;
	static short role = BLACK;
	static short board[8][8] = {};

	short x_mouse = 0, y_mouse = 0;
	static CString s;
	const int ID_TIMER = 1;
	static time_t start, finish, elapsed;
	struct tm * ptm;

	switch (message)
	{
	case WM_CREATE: 
		   msgbox = CreateWindowEx(NULL, L"ListBox", L"Message",WS_VISIBLE,
								   750, 50, 250, 250, hWnd, NULL, hInst, NULL);
		   ShowWindow(msgbox, SW_SHOW);
		   UpdateWindow(msgbox);
		   
		   memcpy(board,initboard,sizeof(board));

		   start = time(0);
		   state = NextState(0,0,state,board,&role,&rc);
		   
		   hdc = GetWindowDC(msgbox);
		   GetClientRect (msgbox, &rect) ;

		   s.Format(L"Welcome to checker!");
		   TextOut(hdc, 20,125,s, s.GetLength());
		   
		   switch (state){
		   case MovePick: s.Format(L"Action: Pick a piece to move."); break;
		   case MoveDrop: s.Format(L"Action: Drop a piece to move."); break;
		   case JumpPick: s.Format(L"Action: Pick a piece to jump."); break;
		   case JumpDrop: s.Format(L"Action: Drop a piece to jump."); break;
		   case MultiJump: s.Format(L"Action: You can jump again."); break;
		   default: s.Format(L"Game Stop!"); break;
		   }
		   
		   TextOut(hdc, 20,200,s, s.GetLength());
		   SetFocus(hWnd);
		   
		   if (state == Animate)
		   {
			   step = STEP;
			   SetTimer(hWnd, ID_TIMER, 10 , NULL); //update every 10 milliseconds
		   }
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_RESET:
			step = 0;
			rc.x_pick = -1; rc.y_pick = -1; rc.x_drop = -1; rc.y_drop = -1;	
			rc.x_jump2 = -1; rc.y_jump2 = -1; rc.x_jump3 = -1; rc.y_jump3 = -1;
			state = Start;
			role = BLACK; //curent role

			memcpy(board,initboard,sizeof(board));

			start = time(0);
			state = NextState(0,0,state,board,&role,&rc);

			ShowWindow(msgbox, SW_HIDE);
			ShowWindow(msgbox, SW_SHOW);
			UpdateWindow(msgbox);
			hdc = GetWindowDC(msgbox);
			GetClientRect (msgbox, &rect) ;

			s.Format(L"Game Reset!");
			TextOut(hdc, 20,125,s, s.GetLength());
		   
			switch (state){
			case MovePick: s.Format(L"Action: Pick a piece to move."); break;
			case MoveDrop: s.Format(L"Action: Drop a piece to move."); break;
			case JumpPick: s.Format(L"Action: Pick a piece to jump."); break;
			case JumpDrop: s.Format(L"Action: Drop a piece to jump."); break;
			case MultiJump: s.Format(L"Action: You can jump again."); break;
			default: s.Format(L"Game Stop!"); break;
			}
		   
			TextOut(hdc, 20,200,s, s.GetLength());
			SetFocus(hWnd);
		   
			if (state == Animate)
			{
				step = STEP;
				SetTimer(hWnd, ID_TIMER, 10 , NULL); //update every 10 milliseconds
			}

			InvalidateRect (hWnd, NULL, TRUE) ;
			break;
		case ID_SHOW:
			show = !show;
			if (show) ShowWindow(msgbox, SW_SHOW);
			else ShowWindow(msgbox, SW_HIDE);
			
			UpdateWindow(msgbox);
			SetFocus(hWnd);
			break;
		case ID_DIFFICULTY_LEVEL1:
			maxDepth = 1;
			break;
		case ID_DIFFICULTY_LEVEL2:
			maxDepth = 4;
			break;
		case ID_DIFFICULTY_LEVEL3:
			maxDepth = 8;
			break;
		case ID_DIFFICULTY_LEVEL4:
			maxDepth = 15;
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
			
	case WM_LBUTTONDOWN:
		if(state == Animate)
			break; // do not react while moving

		x_mouse = short(HIWORD (lParam)/LENGTH);
		y_mouse = short(LOWORD (lParam)/LENGTH);
		   
		state = NextState(x_mouse,y_mouse,state,board,&role,&rc);

		if (state == Animate)
		{
			step = STEP;
			SetTimer(hWnd, ID_TIMER, 10 , NULL); //update every 10 milliseconds
			if (role == BLACK) start = time(0);
		}
		else if (state != GameStop)
		{
			hdc = GetWindowDC(msgbox);
			GetClientRect (msgbox, &rect) ;

			switch (state){
			case MovePick: s.Format(L"Next Action: Pick a piece to move."); break;
			case MoveDrop: s.Format(L"Next Action: Drop a piece to move."); break;
			case JumpPick: s.Format(L"Next Action: Pick a piece to jump."); break;
			case JumpDrop: s.Format(L"Next Action: Drop a piece to jump."); break;
			case MultiJump: s.Format(L"Next Action: You can jump again."); break;
			default: s.Format(L"Game Stop!"); break;
			}

		   TextOut(hdc, 20,200,s, s.GetLength());
		   SetFocus(hWnd);
		}

		InvalidateRect (hWnd, NULL, TRUE) ;
		break;
	
    case WM_TIMER: 
		if (step--==0)
		{
			state = NextState(0,0,state,board,&role,&rc);
			KillTimer(hWnd, ID_TIMER);
			
			if (state == GameStop && show!=false) 
			{
				// update the message box
				ShowWindow(msgbox, SW_HIDE);
				ShowWindow(msgbox, SW_SHOW);
				UpdateWindow(msgbox);
				hdc = GetWindowDC(msgbox);
				GetClientRect (msgbox, &rect) ;
				
				s.Format(L"Game finished!"); 
				TextOut(hdc, 20,110,s, s.GetLength());

				int b_score = 0, w_score = 0;
				for(int i = 0; i<8; i++)
				for(int j = 0; j<8; j++)
				{
					if(board[i][j] == BLACK) 
						if (i == 0) b_score += 2;
						else b_score ++;
					
					if(board[i][j] == WHITE) 
						if (i == 7) w_score += 2;
						else w_score ++;
				}
				s.Format(L"Black Score : %d",b_score);
				TextOut(hdc, 20,140,s, s.GetLength());
				s.Format(L"White Score : %d",w_score);
				TextOut(hdc, 20,170,s, s.GetLength());

				if (b_score>w_score) s.Format(L"You Win!");
				if (b_score==w_score) s.Format(L"Draw!");
				if (b_score<w_score) s.Format(L"You Lost!");
				
				TextOut(hdc, 20,200,s, s.GetLength());
				SetFocus(hWnd);
			}
			else if (role == BLACK && state != Animate) 
			{
				finish = time(0);
				elapsed = static_cast<time_t>(difftime(finish,start));
				ptm = gmtime (&elapsed);

				// update the message box
				if (show)
				{
					ShowWindow(msgbox, SW_HIDE);
					ShowWindow(msgbox, SW_SHOW);
					UpdateWindow(msgbox);
					hdc = GetWindowDC(msgbox);
					GetClientRect (msgbox, &rect) ;
			
					s.Format(L"Maximum tree depth: %d.",treeDepth);
					TextOut(hdc, 20,50,s, s.GetLength());
					s.Format(L"Node generated: %d.",genNode);
					TextOut(hdc, 20,80,s, s.GetLength());
					s.Format(L"MAX-VALUE pruned: %d.",pruneMax);
					TextOut(hdc, 20,110,s, s.GetLength());
					s.Format(L"MIN-VALUE pruned: %d.",pruneMin);
					TextOut(hdc, 20,140,s, s.GetLength());
					s.Format(L"Running Time: %2d:%2d.", ptm->tm_min, ptm->tm_sec);
					TextOut(hdc, 20, 170,s, s.GetLength());

					switch (state){
					case MovePick: s.Format(L"Next Action: Pick a piece to move."); break;
					case MoveDrop: s.Format(L"Next Action: Drop a piece to move."); break;
					case JumpPick: s.Format(L"Next Action: Pick a piece to jump."); break;
					case JumpDrop: s.Format(L"Next Action: Drop a piece to jump."); break;
					case MultiJump: s.Format(L"Next Action: You can jump again."); break;
					default: s.Format(L"BLACK move first!"); break;
					}

					TextOut(hdc, 20,200,s, s.GetLength());
					SetFocus(hWnd);
				}
			}
			else if (state == Animate)
			{
				step = STEP;
				SetTimer(hWnd, ID_TIMER, 10 , NULL); //update every 10 milliseconds
			}
		}
		InvalidateRect (hWnd, NULL, TRUE) ;
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...

		//create the pen & brush inside "case" and delete before leave!
		hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0)); // black

		pieceBrush[0] = CreateSolidBrush(RGB(255, 255, 255)); // white
		pieceBrush[1] = CreateSolidBrush(RGB(0, 0, 0)); // black

		boardBrush[0] = CreateSolidBrush(RGB(255, 255, 255)); // white
		boardBrush[1] = CreateSolidBrush(RGB(200, 200, 200)); // gray
		boardBrush[2] = CreateSolidBrush(RGB(0, 200, 0)); // green

		// Select our green pen into the device context and remember previous pen
		oldPen = (HPEN)SelectObject(hdc, hPen);
		oldBrush = (HBRUSH)SelectObject(hdc, pieceBrush[0]);

		// draw back ground board
		for (int i = 0 ; i < 8 ; i++)
		for (int j = 0 ; j < 8 ; j++)
		{
			rect.top = i * LENGTH; 
			rect.bottom = (i + 1) * LENGTH; 
			rect.left = j * LENGTH; 
			rect.right = (j + 1) * LENGTH;
			FillRect(hdc, &rect, boardBrush[(i+j)%2]);
		}
		
		// draw pick board
		if (state == MoveDrop || state == JumpDrop || state == MultiJump || state == Animate){
			rect.top = rc.x_pick * LENGTH; 
			rect.bottom = (rc.x_pick + 1) * LENGTH; 
			rect.left = rc.y_pick * LENGTH; 
			rect.right = (rc.y_pick + 1) * LENGTH;
			FillRect(hdc, &rect, boardBrush[2]);
		}
		
		// draw piece
		for (int i = 0 ; i < 8 ; i++)
		for (int j = 0 ; j < 8 ; j++)
		{
			if(board[i][j] != 0) // i & j is transpost in board
			{
				SelectObject(hdc, pieceBrush[(board[i][j]+1)%2]); // WHITE % 2 = 1, BLACK % 2 = 0;
				Ellipse (hdc, j * LENGTH + 10, i * LENGTH + 10, (j+1) * LENGTH - 10, (i+1) * LENGTH - 10) ;
				
				SelectObject(hdc, pieceBrush[board[i][j]%2]);
				Ellipse (hdc, j * LENGTH + 30, i * LENGTH + 30, (j+1) * LENGTH - 30, (i+1) * LENGTH - 30) ;
			}
		}

		// draw moving piece
		if (state == Animate){
			int xCurrent = (STEP - step) * (rc.x_drop * LENGTH - rc.x_pick * LENGTH)/STEP + rc.x_pick * LENGTH;
			int yCurrent = (STEP - step) * (rc.y_drop * LENGTH - rc.y_pick * LENGTH)/STEP + rc.y_pick * LENGTH;

			SelectObject(hdc, pieceBrush[(role+1)%2]);
			Ellipse (hdc, yCurrent + 10, xCurrent + 10, yCurrent + LENGTH - 10, xCurrent + LENGTH - 10) ;

			SelectObject(hdc, pieceBrush[role%2]);
			Ellipse (hdc, yCurrent + 30, xCurrent + 30, yCurrent + LENGTH - 30, xCurrent + LENGTH - 30) ;
		}

		// Do not forget to clean up.
		// Select the old pen back into the device context
		SelectObject(hdc, oldPen);
		DeleteObject(hPen);
		SelectObject(hdc, oldBrush);
		DeleteObject(pieceBrush[0]);
		DeleteObject(pieceBrush[1]);
		DeleteObject(boardBrush[0]);
		DeleteObject(boardBrush[1]);
		DeleteObject(boardBrush[2]);

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
