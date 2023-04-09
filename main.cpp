#include "animated.hpp"
#include <conio.h>
#include <iomanip>
#include <vector>
#include <sstream>
#include <Windows.h>

int scw = 80, sch = 20;
std::vector<char> screen = std::vector<char>(scw * sch);

void drawOnScreen(int x, int y, const char *text) {
	size_t tlen = strlen(text), ti = 0;
	for(size_t i = y * scw + x; i < screen.size() && ti < tlen; i++, ti++) { screen[i] = text[ti]; }
}

char sprintfBuff[200]{ 0 };

int main(int argc, const char *argv[]) {

	system("@chcp 65001 > nul");

	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO cbi;
	GetConsoleScreenBufferInfo(hCon, &cbi);
	COORD beginPos = cbi.dwCursorPosition;

	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(hCon, &cci);
	cci.bVisible = false;


	Timeline tl;
	tl.durationFrames(250);


	Animated av1 = 5;
	av1.timeline = &tl;
	av1.insert(100, 20);
	av1.insert(150, 79);
	av1.insert(220, 0);
	av1.insert(250, 0);

	int counter = 0;
	int fps = 0;


	while(true) {

		SetConsoleCursorInfo(hCon, &cci);
		SetConsoleCursorPosition(hCon, beginPos);

		cr::steady_clock::time_point delayTimer = cr::steady_clock::now();

		for(auto &c : screen) c = ' ';


		if(_kbhit()) {
			char c = _getch();
			switch(c) {
				case 'f':
					float newFps;
					std::cout << "new fps value: ";
					std::cin >> newFps;
					tl.fps(newFps);
					continue;
					break;
				case ' ': tl.toggle(); break;
				case 's': tl.stop(); break;
			}
			if(c >= '0' && c <= '9') {
				float n = static_cast<float>(c - '0') / 10.f;
				tl.playTime(tl.durationTime() * n);
			}
		}


		size_t currFrame = tl.frame();
		float animPercent = static_cast<float>(currFrame) / tl.durationFrames();
		
		sprintf_s(sprintfBuff, 200, "time: %f s;  frame: %zi", tl.playTime(), currFrame);
		drawOnScreen(0, 18, sprintfBuff);
		
		int pos = (scw - 1) * animPercent;
		for(int i = 0; i < scw; i++) { drawOnScreen(i, 19, "-"); }
		drawOnScreen(pos, 19, "#");

		sprintf_s(sprintfBuff, 200, "counter: %i;  fps: %i", counter++, fps);
		drawOnScreen(0, 0, sprintfBuff);

		drawOnScreen(av1.val(), 10, "#");

		for(size_t i = 0; i < sch; i++) {
			for(size_t j = 0; j < scw; j++) printf("%c", screen[i * scw + j]);
			printf("|\n");
		}
		for(size_t k = 0; k < scw; k++) printf("_");
		printf("|");

		tl.update();

		fps = 1s / (cr::steady_clock::now() - delayTimer);

	}

	return 0;
}