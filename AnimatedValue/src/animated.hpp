#pragma once

#include <iostream>
#include <ctime>
#include <thread>

namespace cr = std::chrono;
using namespace std::chrono_literals;

using std::cout;

class Timeline {

	cr::steady_clock::duration m_playTime{ 0 };

	cr::steady_clock::time_point m_tLast;

	inline void timer() {
		m_tLast = cr::steady_clock::now();
	}

	inline cr::steady_clock::duration elapsed() const {
		return cr::steady_clock::now() - m_tLast;
	}

public:

	Timeline() {
		timer();
	}

	enum class State {
		STOP, PLAYING, PAUSE
	};

	double fps{ 24 };

	void play(), pause(), stop(), toggle();
	inline State getState() const { return m_state; }

	inline cr::steady_clock::duration getTime() const {
		switch(m_state) {
			case State::STOP: return 0s;
			case State::PAUSE: return m_playTime;
			case State::PLAYING: return m_playTime + elapsed();
		}
	}

	inline double getSecs() const {
		return cr::duration<double>(getTime()).count();
	}

	inline int getFrame() const {
		return static_cast<int>(getSecs() * fps);
	}

private:

	State m_state{ State::STOP };

};
