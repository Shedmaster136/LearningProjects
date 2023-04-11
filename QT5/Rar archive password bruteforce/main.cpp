#include <windows.h>
#include <stdio.h>
#include <iostream>


#define getToLine 260

int main(int argc, char **argv)
{
	setlocale(0, "");
	DWORD x;
	DWORD y;
	int messageSize = 7;
	char message[7];
	LPCTSTR lpClassName = L"QWidget";
	LPCTSTR lpWindowName = L"Самораспаковывающийся архив";
	RECT Rect;
	HWND OK;
	int startBruteForce = 0;
	printf("Начать подбор пароля?\r\nВо время подбора пароля не совершайте никаких действий на компьютере\r\n(1 - начать, 0 - нет)\r\n");
	scanf("%d", &startBruteForce);
	if((startBruteForce != 0) && (startBruteForce != 1))
	{
		printf("Неверный вариант\r\n");
		system("pause");
		return -1;
	}
	if(startBruteForce == 0)
	{
		system("pause");
		return 0;
	}
	
	
	HWND hWnd = FindWindow(lpClassName, lpWindowName);
	if(hWnd == NULL)
	{
		printf("Окно не найдено\r\n");
		system("pause");
		return -2;
	}

	GetWindowRect(hWnd, &Rect);
	x = Rect.right - (Rect.right - Rect.left)/2;
	y = Rect.bottom - getToLine;
	SetCursorPos(x,y);
	SetForegroundWindow(hWnd);
	mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, NULL, NULL);
	mouse_event(MOUSEEVENTF_LEFTUP, x, y, NULL, NULL);
	for(int L1 = '1'; L1 <= '9'; ++L1)//начать подбор с 1 (0 не принимает);
	{
		message[0] = L1;
		for(int L2 = '0'; L2 <= '9'; ++L2)
		{
			message[1] = L2;
			for(int L3 = '0'; L3 <= '9'; ++L3)
			{
				message[2] = L3;
				for(int L4 = '0'; L4 <= '9'; ++L4)
				{
					message[3] = L4;
					for(int L5 = '0'; L5 <= '9'; ++L5)
					{
						message[4] = L5;
						for(int L6 = '0'; L6 <= '9'; ++L6)
						{
							message[5] = L6;
							for(int L7 = '0'; L7 <= '9'; ++L7)
							{
								message[6] = L7;
	
								for(int i = 0; i < messageSize; ++i)
								{
									SendMessage(hWnd, WM_CHAR,message[i],NULL);
								}
								PostMessage(hWnd, WM_KEYDOWN, VK_RETURN, NULL);
								PostMessage(hWnd, WM_KEYUP, VK_RETURN, NULL);

								Sleep(20);

								OK = FindWindow(lpClassName, L" ");
								if(OK == NULL)
								{
									printf("Подбор завершен\r\n");
									return 0;
								}
								SendMessage(OK,WM_KEYDOWN, VK_RETURN, NULL);
								SendMessage(OK,WM_KEYUP, VK_RETURN, NULL);
								for(int i = 0; i < messageSize; ++i)
								{
									SendMessage(hWnd, WM_KEYDOWN, VK_BACK, NULL);
									SendMessage(hWnd, WM_KEYUP, VK_BACK, NULL);
								}
							}
						}
					}
				}
			}
		}
	}
	printf("Ни один из вариантов не подошел\r\n");
	return 0;
}
