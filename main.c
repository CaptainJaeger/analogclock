#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "resource.h"

HINSTANCE Instanz;
const char g_szClassName[] = "analogClockClass";

const int ID_TIMER = 1;
static int radioChecked = 110;

//tempRadioCheck -> speicher radioButton nur wenn Nutzer auf OK klickt, bis dahin temporär speichern
int tempRadioCheck;
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
	
	//Beim Erstaufruf Uhrzeit initialiseren
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

	HDC hdc;	//Handle zum zeichnen
	PAINTSTRUCT ps;	//enthaelt infos ueber das zu malende Fenster sowie paint messages(BeginPaint() usw.)
	int heigth=720, width= 720;	//Breite u. Laenge des Fenster
	double pi = 3.14;
	//SYSTEMTIME lt = { 0 };
	switch (nachricht)
	{
	
	//initialisiere einmalig Menue waehrend Applikation startet
	case WM_CREATE:
	{
		HMENU hMenu, hSubMenu;
		HICON hIcon, hIconSm;

		//Menu erzeugen
		hMenu = CreateMenu();

		hSubMenu = CreatePopupMenu();
		AppendMenu(hSubMenu, MF_STRING, ID_UHRZEIT, TEXT("&Uhrzeit waehlen"));
		AppendMenu(hSubMenu, MF_STRING, ID_DATEI_BEENDEN, TEXT("&Beenden"));
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, TEXT("&Datei"));
		
		SetMenu(Fenster, hMenu);	//erzeugtes Menu dem aktuellen Fenster uebergeben
		
		/* Timer setzen. Parameter:
			Fenster -> Timer wird an aktuelles Fenster gebunden
			ID_TIMER -> ID fuer Timer setzen
			1000 ->	Timer soll alle 1000ms reagieren
			NULL -> Eigene Funktion zur Reaktion kann eingebunden werden, hier NULL u. sendet WM_TIMER ans System
		*/
		SetTimer(Fenster, ID_TIMER, 1000, NULL);
		break;
	}

	case WM_PAINT:
	{						
		//Systemzeit wurde ausgewaehlt im Dialogfenster
		if (radioChecked == 110) {
			GetLocalTime(&lt);
			lt_hour = lt.wHour, lt_min = lt.wMinute, lt_sec = lt.wSecond;
		}

		/* Eigene Uhrzeit wurde ausgewaehlt im Dialogfenster
		/* u.  wenn eigene UHrzeit gewaehlt wurde, soll Uhr nach dieser Uhrzeit weiterlaufen
		/* Jeweils fuer Sekunde, Minute, Stunde */		
		if (radioChecked == 111) {
			lt_sec++;
			if (lt_sec == 60) {
				lt_sec = 0;
				lt_min++;
			}

			if (lt_min == 60) {
				lt_min = 0;
				lt_hour++;
			}

			if (lt_hour % 12 == 0) {
				lt_hour = 0;
			}
		}

		/* return HDC, repraesentiert das HWND Fenster
		/*	alle Zeichenoperation auf hdc werden sofort im Bildschirm angezeigt 
		 */
		hdc = BeginPaint(Fenster, &ps);	
				
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
		//wParam bezieht sich hier auf die Menu-IDs die durch Nutzerauswahl hierher gesendet werden
		switch (wParam) {
			//case IDs definiert in resource.h
			
			//1000 -> Benutzer klickt auf Beenden im Menuepunkt
			case 1000:	//Beenden			
				PostQuitMessage(0);
				return 0;
					
			//10 -> Benutzer klickt auf Uhrzeit waehlen im Menupunkt
			case 10:	//Uhrzeit waehlen

				//Oeffnet Dialogbox zum auswaehlen der Uhrzeit
				/* Parameter die Uebergeben werden: 
						Instanz -> aktuelle Instanz dieser Applik.
						MAKEINTRESOURCE(..) -> cast int ID zu LPCTSTR
							- ID, definiert in resource.h, bezieht sich auf Dialogfenster, welches in .rc implementiert wurde
						Fenster -> Handle des parent Fenster
						Dialogfuntion -> Funktion, auf welcher sich der Dialog bezieht */
				tempRadioCheck = radioChecked;
				DialogBox(Instanz, MAKEINTRESOURCE(IDD_DIALOGCLOCK), Fenster, Dialogfunktion);				
				return 0;
				//break;
		}
		break;
		

	case WM_TIMER:
		InvalidateRect(Fenster, NULL, TRUE);
		//return 0;
		break;

	//wenn Message kommt die Applikation zu schließen, Ressourcen freiegeben bzw. Timer ebenfalls beenden
	case WM_DESTROY:
		KillTimer(Fenster, ID_TIMER);
		PostQuitMessage(0);
		break;

		/* wenn Benutzer auf 'X' im Fenster oben rechts, ALT-F4 oder schliesen in Fenstermenu klickt,
		/* und Nutzer bittet zur Beendigung der Applikation
		/* Timer und Fenster werden geschlossen */
	case WM_CLOSE:
		KillTimer(Fenster, ID_TIMER);
		DestroyWindow(Fenster);	//Methode ruft WM_DESTROY message ans System
		break;
	default:
		return DefWindowProc(Fenster, nachricht, wParam, lParam);
	}
	return 0;
}

BOOL FAR PASCAL Dialogfunktion(HWND dialogFenster, UINT nachricht, WPARAM wParam, LPARAM lParam) {	

	switch (nachricht) {
	
	//Dialogfenster wird initialisierst
	case WM_INITDIALOG:
		CheckRadioButton(dialogFenster, 110, 111, tempRadioCheck);
		if (SetDlgItemInt(dialogFenster, IDC_HOUR, lt_hour, 5) == 0) {
			lt_hour = 0;
		}
		if (SetDlgItemInt(dialogFenster, IDC_MIN, lt_min, 5) == 0) {
			lt_min = 0;
		}
		if (SetDlgItemInt(dialogFenster, IDC_SEC, lt_sec, 5) == 0) {
			lt_sec = 0;
		}
		return TRUE;

	//Benutzer interagiert mit (Radio-)Buttons
	case WM_COMMAND:
		switch (wParam) {

		//Radiobuttons
		case 110: 
			tempRadioCheck = 110;
			//radioChecked = 110;	//Systemzeit
			return TRUE;

		case 111: 
			tempRadioCheck = 111;
			//radioChecked = 111;	//Eigene Uhrzeit
			return TRUE;

		//Benutzer klickt OK-Button -> Aenderungen speichern
		case IDOK:	
			radioChecked = tempRadioCheck;
			//Speichern nur wenn radioButton geklickt wurde
			if (radioChecked == 111) {

				//gueltig nur Zahlen in EditText akzeptieren, bei Fehlerfall Wert auf 0 setzen
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
			}

		//Benutzer klickt Abbrechen-Button -> Aenderungen nicht speichern u. Dialog schliesen
		case IDCANCEL:
			EndDialog(dialogFenster, 0);
			return TRUE;
		}
	}
	return FALSE;
}