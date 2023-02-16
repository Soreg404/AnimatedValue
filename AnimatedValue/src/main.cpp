#include "animated.hpp"
#include <conio.h>
#include <iomanip>
#include <Windows.h>


int main(int argc, const char *argv[]) {

	system("@chcp 65001 > nul");

	DWORD ncw;
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO cbi;
	GetConsoleScreenBufferInfo(hCon, &cbi);

	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(hCon, &cci);
	cci.bVisible = false;
	SetConsoleCursorInfo(hCon, &cci);

	COORD beginPos = cbi.dwCursorPosition;

	Timeline tl;
	tl.mode = Timeline::Mode::FRAMES;
	tl.durationFrames(250);

	cr::steady_clock::time_point timer;

	Animated av1 = 2;
	av1.timeline = &tl;
	av1.insert(8, 20);
	av1.insert(45, 30);
	av1.insert(2, 90);
	av1.insert(23, 100);
	av1.insert(34, 150);
	av1.insert(1, 220);

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
				FillConsoleOutputCharacter(hCon, ' ', 200, beginPos, &ncw);
				std::cin >> newFps;
				SetConsoleCursorPosition(hCon, beginPos);
				tl.fps(newFps);

				break;
			case 32: tl.toggle(); break;
			case 's': tl.stop();
		}

		size_t currFrame = tl.getFrame();
		float animPercent = static_cast<double>(currFrame) / tl.durationFrames();

		cout << std::fixed << std::setw(4) << "time: " << tl.getSecs() << "s;  frame: " << currFrame << "            \n";

		int pos = 30 * animPercent;
		for(int i = 0; i < 30; i++) {
			cout << (i != pos ? '-' : '#');
		}
		cout << "\n\n";

		for(int i = 0; i < 50; i++) cout << ( i == (int)av1.val() ? '#' : ' ');
		cout << "\n";


		for(int i = 0; i < 200; i++) cout << "     ";
		std::this_thread::sleep_for(cr::duration<float>(1.f/tl.fps()) - (cr::steady_clock::now() - timer));
		timer = cr::steady_clock::now();


	} while(true);

	return 0;
}