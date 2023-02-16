#include "animated.hpp"

void Timeline::play() {
	if(m_state == State::PLAYING) return;
	timer();
	m_state = State::PLAYING;
}

void Timeline::pause() {
	m_playTime += elapsed();
	m_state = State::PAUSE;
}

void Timeline::stop() {
	m_playTime = 0s;
	m_state = State::STOP;
}

void Timeline::toggle() {
	switch(m_state) {
		case State::STOP:
		case State::PAUSE:
			play();
			break;
		case State::PLAYING:
			pause();
			break;
	}
}