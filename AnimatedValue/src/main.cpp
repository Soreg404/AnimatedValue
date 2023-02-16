#include "animated.hpp"
#include <conio.h>
#include <iomanip>
#include <Windows.h>


int main(int argc, const char *argv[]) {

	system("@chcp 65001 > nul");

	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO cbi;
	GetConsoleScreenBufferInfo(hCon, &cbi);

	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(hCon, &cci);
	cci.bVisible = false;
	SetConsoleCursorInfo(hCon, &cci);

	COORD beginPos = cbi.dwCursorPosition;

	Timeline tl;

	cr::steady_clock::time_point timer;

	do {

		SetConsoleCursorPosition(hCon, beginPos);

		timer = cr::steady_clock::now();

		char c = 0;
		if(_kbhit()) {
			c = _getch();
		}

		switch(c) {
			case 'f':
				float newFps;
				DWORD ncw;
				FillConsoleOutputCharacter(hCon, ' ', 200, beginPos, &ncw);
				std::cin >> newFps;
				SetConsoleCursorPosition(hCon, beginPos);
				tl.fps = newFps;

				break;
			case 32: tl.toggle(); break;
			case 's': tl.stop();
		}

		cout << std::fixed << std::setw(4) << "time: " << tl.getSecs() << "s;  frame: " << tl.getFrame() << "                  ";

		std::this_thread::sleep_for(cr::duration<float>(1.f/tl.fps) - (cr::steady_clock::now() - timer));
		timer = cr::steady_clock::now();


	} while(true);

	return 0;
}