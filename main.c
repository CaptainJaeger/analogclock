#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "resource.h"

HINSTANCE Instanz;
const char g_szClassName[] = "analogClockClass";

const int ID_TIMER = 1;
static int radioChecked = 110;
static SYSTEMTIME lt = { 0 };
static int lt_hour = 0, lt_min = 0, lt_sec = 0;

LRESULT CALLBACK Fensterfunktion(HWND, UINT, WPARAM, LPARAM);
BOOL FAR PASCAL Dialogfunktion(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE dieseInstanz, HINSTANCE vorherigeInstanz, LPSTR lpszArgument, int FensterStil) {
	MSG Meldung;	
	HWND Hauptfenster;
	Instanz = dieseInstanz;

	if (!vorherigeInstanz) {
		WNDCLASSEX wincl;
		wincl.style = 0;
		wincl.lpfnWndProc = Fensterfunktion;
		wincl.cbClsExtra = 0;
		wincl.cbWndExtra = 0;
		wincl.hInstance = dieseInstanz;
		wincl.hIcon = LoadIcon(NULL, IDI_ASTERISK);//LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);//(HICON)LoadImage(GetModuleHandle(NULL),	MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0)
		wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
		wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
		wincl.lpszMenuName = MAKEINTRESOURCE(IDR_MYMENU);
		wincl.lpszClassName = g_szClassName;
		wincl.cbSize = sizeof(WNDCLASSEX);

		if (!RegisterClassEx(&wincl)) {
			MessageBox(NULL, TEXT("Window Registration Failed!"), "Error!",
				MB_ICONEXCLAMATION | MB_OK);
			return 0;
		}
	}

	Hauptfenster = CreateWindowEx(
		0,	/*sunken inner border around window, WS_EX_CLIENTEDGE*/
		g_szClassName, /*Class name*/
		TEXT("Analog Clock Application"),	/* App Title*/
		WS_OVERLAPPEDWINDOW,	/* win-Style paramter */
		CW_USEDEFAULT,	
		CW_USEDEFAULT,
		720,	/* x_fenster */
		800,	/* y-Fenster */
		NULL,
		NULL,
		dieseInstanz,
		NULL);

	if (!Hauptfenster) {
		MessageBox(NULL, TEXT("Window Creation Failed!"), "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	
	//Beim erst-aufruf initialiseren
	GetLocalTime(&lt);
	lt_hour = lt.wHour, lt_min = lt.wMinute, lt_sec = lt.wSecond;

	ShowWindow(Hauptfenster, FensterStil);
	UpdateWindow(Hauptfenster);

	//in windows immer Adressuebergabe statt Referenzuebergabe
	//kein Referenzuebergabe, sondern Adressuebergabe, um auf das Original zu zeigen und auf Meldung kann geschrieben werden
	//ueber dispatcher wird diese an callback-Methode uebergeben
	//mit dispatcher wird richtige message angesprochen
	//modales Fenster hier
	while (GetMessage(&Meldung, NULL, 0, 0)) {
		TranslateMessage(&Meldung);
		DispatchMessage(&Meldung);
	}
	return Meldung.wParam;

}// Ende WinMain
 //-----------------Callback Funktion des Hauptfensters ------------------------
LRESULT CALLBACK Fensterfunktion(HWND Fenster, UINT nachricht, WPARAM wParam, LPARAM lParam)  {

	HDC hdc;
	PAINTSTRUCT ps;
	int heigth=720, width= 720;
	double pi = 3.14;
	//SYSTEMTIME lt = { 0 };
	switch (nachricht)
	{
	case WM_PAINT:
	{		
		
		
		//Systemzeit wurde ausgewaehlt
		if (radioChecked == 110) {
			/*lt_hour = 2;
			lt_min = 3;
			lt_sec = 4;
			*/

			GetLocalTime(&lt);
			lt_hour = lt.wHour, lt_min = lt.wMinute, lt_sec = lt.wSecond;
		}

		hdc = BeginPaint(Fenster, &ps);
		//MoveToEx(hdc, width/2, heigth/2, NULL);
		HPEN hPenOld;
		HPEN hLinePen;
		COLORREF qLineColor;
		qLineColor = RGB(0, 0, 255);
		hLinePen = CreatePen(PS_SOLID, 1, qLineColor);
		hPenOld = (HPEN)SelectObject(hdc, hLinePen);
		//fractal
		for (int i = 0; i< 300; i++) {
			MoveToEx(hdc, floor((width / 2) + 260 * sin(i * pi / 150)), floor((heigth / 2) - 260 * cos(i * pi / 150)), NULL);
			LineTo(hdc, floor((width / 2) + 260 * sin(i * pi / 150 + ((lt_sec + lt_min * 60+3600*lt_hour)*0.033333333333333)*i*pi / 150)), floor((heigth / 2) - 260 * cos(i * pi / 150 + ((lt_sec + lt_min * 60+3600*lt_hour)*0.033333333333333)*i*pi / 150)));
		}
		qLineColor = RGB(255, 0, 0);
		hLinePen = CreatePen(PS_SOLID, 10, qLineColor);
		hPenOld = (HPEN)SelectObject(hdc, hLinePen);
		//zeiger
		MoveToEx(hdc, floor(width / 2), floor(heigth / 2), NULL);
		LineTo(hdc, floor(width / 2) + 220 * sin(lt_sec*pi / 30), floor(heigth / 2) - 220 * cos(lt_sec*pi / 30));

		MoveToEx(hdc, floor(width / 2), floor(heigth / 2), NULL);
		LineTo(hdc, floor(width / 2) + 180 * sin(lt_min*pi / 30), floor(heigth / 2) - 180 * cos(lt_min*pi / 30));

		MoveToEx(hdc, floor(width / 2), floor(heigth / 2), NULL);
		LineTo(hdc, floor(width / 2) + 140 * sin(lt_hour*pi / 6), floor(heigth / 2) - 140 * cos(lt_hour*pi / 6));

		qLineColor = RGB(255, 0, 0);
		hLinePen = CreatePen(PS_SOLID, 5, qLineColor);
		hPenOld = (HPEN)SelectObject(hdc, hLinePen);
		//Zahlen
		//XII
		MoveToEx(hdc, floor((width / 2) + 280 * sin(0 * pi / 6)), floor((heigth / 2) - 280 * cos(0 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(0 * pi / 6)), floor((heigth / 2) - 280 * cos(0 * pi / 6) - 20));
		MoveToEx(hdc, floor((width / 2) + 280 * sin(0 * pi / 6) + 10), floor((heigth / 2) - 280 * cos(0 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(0 * pi / 6) + 10), floor((heigth / 2) - 280 * cos(0 * pi / 6) - 20));
		MoveToEx(hdc, floor((width / 2) + 280 * sin(0 * pi / 6) - 10), floor((heigth / 2) - 280 * cos(0 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(0 * pi / 6) - 20), floor((heigth / 2) - 280 * cos(0 * pi / 6) - 20));
		MoveToEx(hdc, floor((width / 2) + 280 * sin(0 * pi / 6) - 20), floor((heigth / 2) - 280 * cos(0 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(0 * pi / 6) - 10), floor((heigth / 2) - 280 * cos(0 * pi / 6) - 20));
		//I
		MoveToEx(hdc, floor((width / 2) + 280 * sin(1 * pi / 6)), floor((heigth / 2) - 280 * cos(1 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(1 * pi / 6)), floor((heigth / 2) - 280 * cos(1 * pi / 6) - 20));
		//II
		MoveToEx(hdc, floor((width / 2) + 280 * sin(2 * pi / 6)), floor((heigth / 2) - 280 * cos(2 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(2 * pi / 6)), floor((heigth / 2) - 280 * cos(2 * pi / 6) - 20));
		MoveToEx(hdc, floor((width / 2) + 280 * sin(2 * pi / 6) + 10), floor((heigth / 2) - 280 * cos(2 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(2 * pi / 6) + 10), floor((heigth / 2) - 280 * cos(2 * pi / 6) - 20));
		//III
		MoveToEx(hdc, floor((width / 2) + 280 * sin(3 * pi / 6)), floor((heigth / 2) - 280 * cos(3 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(3 * pi / 6)), floor((heigth / 2) - 280 * cos(3 * pi / 6) - 20));
		MoveToEx(hdc, floor((width / 2) + 280 * sin(3 * pi / 6) + 10), floor((heigth / 2) - 280 * cos(3 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(3 * pi / 6) + 10), floor((heigth / 2) - 280 * cos(3 * pi / 6) - 20));
		MoveToEx(hdc, floor((width / 2) + 280 * sin(3 * pi / 6) + 20), floor((heigth / 2) - 280 * cos(3 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(3 * pi / 6) + 20), floor((heigth / 2) - 280 * cos(3 * pi / 6) - 20));
		//IV
		MoveToEx(hdc, floor((width / 2) + 280 * sin(4 * pi / 6) + 10), floor((heigth / 2) - 280 * cos(4 * pi / 6) + 10), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(4 * pi / 6) + 10), floor((heigth / 2) - 280 * cos(4 * pi / 6) - 20) + 10);
		MoveToEx(hdc, floor((width / 2) + 280 * sin(4 * pi / 6) + 25), floor((heigth / 2) - 280 * cos(4 * pi / 6) + 10), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(4 * pi / 6) + 20), floor((heigth / 2) - 280 * cos(4 * pi / 6) - 20) + 10);
		MoveToEx(hdc, floor((width / 2) + 280 * sin(4 * pi / 6) + 25), floor((heigth / 2) - 280 * cos(4 * pi / 6) + 10), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(4 * pi / 6) + 30), floor((heigth / 2) - 280 * cos(4 * pi / 6) - 20) + 10);
		//V
		MoveToEx(hdc, floor((width / 2) + 280 * sin(5 * pi / 6) + 15), floor((heigth / 2) - 280 * cos(5 * pi / 6) + 15), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(5 * pi / 6) + 10), floor((heigth / 2) - 280 * cos(5 * pi / 6) - 20) + 15);
		MoveToEx(hdc, floor((width / 2) + 280 * sin(5 * pi / 6) + 15), floor((heigth / 2) - 280 * cos(5 * pi / 6) + 15), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(5 * pi / 6) + 20), floor((heigth / 2) - 280 * cos(5 * pi / 6) - 20) + 15);
		//VI
		MoveToEx(hdc, floor((width / 2) + 280 * sin(6 * pi / 6)), floor((heigth / 2) - 280 * cos(6 * pi / 6) + 20), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(6 * pi / 6) - 5), floor((heigth / 2) - 280 * cos(6 * pi / 6) - 20) + 20);
		MoveToEx(hdc, floor((width / 2) + 280 * sin(6 * pi / 6)), floor((heigth / 2) - 280 * cos(6 * pi / 6) + 20), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(6 * pi / 6) + 5), floor((heigth / 2) - 280 * cos(6 * pi / 6) - 20) + 20);
		MoveToEx(hdc, floor((width / 2) + 280 * sin(6 * pi / 6) + 15), floor((heigth / 2) - 280 * cos(6 * pi / 6) + 20), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(6 * pi / 6) + 15), floor((heigth / 2) - 280 * cos(6 * pi / 6) - 20) + 20);
		//VII
		MoveToEx(hdc, floor((width / 2) + 280 * sin(7 * pi / 6) - 25), floor((heigth / 2) - 280 * cos(7 * pi / 6) + 25), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(7 * pi / 6) - 35), floor((heigth / 2) - 280 * cos(7 * pi / 6) - 20) + 25);
		MoveToEx(hdc, floor((width / 2) + 280 * sin(7 * pi / 6) - 25), floor((heigth / 2) - 280 * cos(7 * pi / 6) + 25), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(7 * pi / 6) - 20), floor((heigth / 2) - 280 * cos(7 * pi / 6) - 20) + 25);
		MoveToEx(hdc, floor((width / 2) + 280 * sin(7 * pi / 6) - 10), floor((heigth / 2) - 280 * cos(7 * pi / 6) + 25), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(7 * pi / 6) - 10), floor((heigth / 2) - 280 * cos(7 * pi / 6) - 20) + 25);
		MoveToEx(hdc, floor((width / 2) + 280 * sin(7 * pi / 6)), floor((heigth / 2) - 280 * cos(7 * pi / 6) + 25), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(7 * pi / 6)), floor((heigth / 2) - 280 * cos(7 * pi / 6) - 20) + 25);
		//VIII
		MoveToEx(hdc, floor((width / 2) + 280 * sin(8 * pi / 6) - 35), floor((heigth / 2) - 280 * cos(8 * pi / 6) + 15), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(8 * pi / 6) - 40), floor((heigth / 2) - 280 * cos(8 * pi / 6) - 20) + 15);
		MoveToEx(hdc, floor((width / 2) + 280 * sin(8 * pi / 6) - 35), floor((heigth / 2) - 280 * cos(8 * pi / 6) + 15), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(8 * pi / 6) - 30), floor((heigth / 2) - 280 * cos(8 * pi / 6) - 20) + 15);
		MoveToEx(hdc, floor((width / 2) + 280 * sin(8 * pi / 6) - 20), floor((heigth / 2) - 280 * cos(8 * pi / 6) + 15), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(8 * pi / 6) - 20), floor((heigth / 2) - 280 * cos(8 * pi / 6) - 20) + 15);
		MoveToEx(hdc, floor((width / 2) + 280 * sin(8 * pi / 6) - 10), floor((heigth / 2) - 280 * cos(8 * pi / 6) + 15), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(8 * pi / 6) - 10), floor((heigth / 2) - 280 * cos(8 * pi / 6) - 20) + 15);
		MoveToEx(hdc, floor((width / 2) + 280 * sin(8 * pi / 6)), floor((heigth / 2) - 280 * cos(8 * pi / 6) + 15), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(8 * pi / 6)), floor((heigth / 2) - 280 * cos(8 * pi / 6) - 20) + 15);
		//IX
		MoveToEx(hdc, floor((width / 2) + 280 * sin(9 * pi / 6) - 20), floor((heigth / 2) - 280 * cos(9 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(9 * pi / 6) - 20), floor((heigth / 2) - 280 * cos(9 * pi / 6) - 20));
		MoveToEx(hdc, floor((width / 2) + 280 * sin(9 * pi / 6) - 10), floor((heigth / 2) - 280 * cos(9 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(9 * pi / 6)), floor((heigth / 2) - 280 * cos(9 * pi / 6) - 20));
		MoveToEx(hdc, floor((width / 2) + 280 * sin(9 * pi / 6)), floor((heigth / 2) - 280 * cos(9 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(9 * pi / 6) - 10), floor((heigth / 2) - 280 * cos(9 * pi / 6) - 20));
		//X
		MoveToEx(hdc, floor((width / 2) + 280 * sin(10 * pi / 6) - 20), floor((heigth / 2) - 280 * cos(10 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(10 * pi / 6) - 10), floor((heigth / 2) - 280 * cos(10 * pi / 6) - 20));
		MoveToEx(hdc, floor((width / 2) + 280 * sin(10 * pi / 6) - 10), floor((heigth / 2) - 280 * cos(10 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(10 * pi / 6) - 20), floor((heigth / 2) - 280 * cos(10 * pi / 6) - 20));
		//XI
		MoveToEx(hdc, floor((width / 2) + 280 * sin(11 * pi / 6) - 30), floor((heigth / 2) - 280 * cos(11 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(11 * pi / 6) - 20), floor((heigth / 2) - 280 * cos(11 * pi / 6) - 20));
		MoveToEx(hdc, floor((width / 2) + 280 * sin(11 * pi / 6) - 20), floor((heigth / 2) - 280 * cos(11 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(11 * pi / 6) - 30), floor((heigth / 2) - 280 * cos(11 * pi / 6) - 20));
		MoveToEx(hdc, floor((width / 2) + 280 * sin(11 * pi / 6) - 10), floor((heigth / 2) - 280 * cos(11 * pi / 6)), NULL);
		LineTo(hdc, floor((width / 2) + 280 * sin(11 * pi / 6) - 10), floor((heigth / 2) - 280 * cos(11 * pi / 6) - 20));
		//Kreis
		Arc(hdc, 100, 100, width - 100, heigth - 100, 0, 0, 0, 0);

		EndPaint(Fenster, &ps);
	}
	break;

	//Menuauswahl
	case WM_COMMAND: 
		switch (wParam) {
			//IDs siehe resource.h
			case 1000:	//Beenden			
				PostQuitMessage(0);
				return 0;
					
			case 10:	//Uhrzeit waehlen
				DialogBox(Instanz, MAKEINTRESOURCE(IDD_DIALOGCLOCK), Fenster, Dialogfunktion);

				//TODO: hier DialogFenster oeffnen
				return 0;
				//break;
		}
		break;
		
		//initialisiere Menue
	case WM_CREATE:
	{
		HMENU hMenu, hSubMenu;
		HICON hIcon, hIconSm;

		hMenu = CreateMenu();

		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, ID_UHRZEIT, TEXT("&Uhrzeit waehlen"));
		AppendMenu(hSubMenu, MF_STRING, ID_DATEI_BEENDEN, TEXT("&Beenden"));
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("&Datei"));

		SetMenu(Fenster, hMenu);
		SetTimer(Fenster, ID_TIMER, 1000, NULL);
		break;
	}
	case WM_TIMER:
		InvalidateRect(Fenster, NULL, TRUE);
		//return 0;
		break;

	case WM_DESTROY:
		KillTimer(Fenster, ID_TIMER);
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		KillTimer(Fenster, ID_TIMER);
		DestroyWindow(Fenster);
		break;
	default:
		return DefWindowProc(Fenster, nachricht, wParam, lParam);
	}
	return 0;
}

BOOL FAR PASCAL Dialogfunktion(HWND dialogFenster, UINT nachricht, WPARAM wParam, LPARAM lParam) {

	switch (nachricht) {
	case WM_INITDIALOG:
		CheckRadioButton(dialogFenster, 110, 111, radioChecked);
		return TRUE;

	case WM_COMMAND:
		switch (wParam) {
		case 110: radioChecked = 110;	//Systemzeit
			return TRUE;
		case 111: radioChecked = 111;	//Eigene Uhrzeit
			return TRUE;
		case IDOK: 
			if (GetDlgItemInt(dialogFenster, IDC_HOUR, NULL, 5) != 0) {
				lt_hour = GetDlgItemInt(dialogFenster, IDC_HOUR, NULL, 5);
			}
			else {
				lt_hour = 0;
			}
			if (GetDlgItemInt(dialogFenster, IDC_MIN, NULL, 5) != 0) {
				lt_min = GetDlgItemInt(dialogFenster, IDC_MIN, NULL, 5);
			}
			else {
				lt_min = 0;
			}
			if (GetDlgItemInt(dialogFenster, IDC_SEC, NULL, 5) != 0) {
				lt_sec = GetDlgItemInt(dialogFenster, IDC_SEC, NULL, 5);
			}
			else {
				lt_sec = 0;
			}
			//lt_static.wHour = hour_Puffer;

		case IDCANCEL:
			EndDialog(dialogFenster, 0);
			return TRUE;
		}
	}
	return FALSE;
}