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

	enum class Mode {
		TIME, FRAMES
	} mode{ Mode::FRAMES };

	enum class End {
		LOOP, PAUSE, STOP, PLAY
	} end;

	inline void fps(double value) { m_fps = value >= 1 ? value : 1; }
	inline double fps() { return m_fps; }

	inline void durationTime(cr::duration<double> d) { m_dTime = d; }
	inline cr::duration<double> durationTime() { return mode == Mode::TIME ? m_dTime : cr::duration<double>(m_dFrames / m_fps); }

	inline void durationFrames(size_t d) { m_dFrames = d; }
	inline size_t durationFrames() { return mode == Mode::FRAMES ? m_dFrames : m_dTime.count() * m_fps; }

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

	inline size_t getFrame() const {
		return static_cast<size_t>(getSecs() * m_fps);
	}

private:

	double m_fps{ 24 };
	State m_state{ State::STOP };

	size_t m_dFrames{ 0 };
	cr::duration<double> m_dTime{ 0 };

};

