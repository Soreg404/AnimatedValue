#pragma once

#include <iostream>
#include <ctime>
#include <list>
#include <thread>

namespace cr = std::chrono;
using namespace std::chrono_literals;

using std::cout;

class Timeline {

public:

	Timeline() {
	}

	enum class State {
		STOP, PLAY, PAUSE, END
	};

	enum class End {
		LOOP_CONTINUE, WAIT, STOP, CONTINUE
	} onEnd{ End::LOOP_CONTINUE };

	inline void fps(double value, bool adjustTimeToFrames = true) {
		m_fps = value >= 1 ? value : 1;
		if(adjustTimeToFrames) durationFrames(m_durationFrames);
		else durationTime(m_durationTime);
	}
	inline double fps() { return m_fps; }

	inline double playTime() const {
		return m_playTime.count();
	}
	inline void playTime(cr::duration<double> setPlayTime) { m_newPlayTime = setPlayTime; m_playTimeChanged = true; }

	inline size_t frame() const {
		return static_cast<size_t>(playTime() * m_fps);
	}
	inline void frame(size_t setFrame) {  }

	inline void durationTime(cr::duration<double> d) { m_durationTime = d; m_durationFrames = std::ceil(m_durationTime.count() * m_fps); }
	inline cr::duration<double> durationTime() { return m_durationTime; }

	inline void durationFrames(size_t d) { m_durationFrames = d > 1 ? d : 1; m_durationTime = cr::duration<double>(d / m_fps); }
	inline size_t durationFrames() { return m_durationFrames; }

	inline State state() const { return m_state; }
	inline void state(State newState) {
		if(newState != m_state) {
			m_stateChanged = true;
			m_newState = newState;
		}
	}

	inline void play() { state(State::PLAY); }
	inline void pause() { state(State::PAUSE); }
	inline void stop() { state(State::STOP); }

	inline void toggle() {
		switch(m_state) {
			case State::STOP:
			case State::PAUSE:
				play();
				break;
			case State::PLAY:
				pause();
				break;
		}
	}

	void update() {
		auto tnow = cr::steady_clock::now();
		auto dt = tnow - m_tLast;
		m_tLast = tnow;
		if(m_playTimeChanged) {
			m_playTime = m_newPlayTime;
			m_playTimeChanged = false;
		} else if(m_state == State::PLAY) {
			m_playTime += dt;
		}
		if(m_durationTime != 0s && m_playTime > m_durationTime) {
			switch(onEnd) {
				case End::LOOP_CONTINUE:
					cr::duration<double> newPlayTime = m_playTime;
					do { newPlayTime -= m_durationTime; } while(newPlayTime > m_durationTime);
					m_playTime = newPlayTime;
					break;
				case End::STOP:
					m_playTime = 0s;
					m_state = State::STOP;
					break;
				case End::WAIT:
					m_playTime = m_durationTime;
					m_state = State::END;
					break;
			}
		}
		if(m_stateChanged) {
			switch(m_newState) {
				case State::STOP:
					m_playTime = 0s;
					break;
				case State::END:
					m_playTime = m_durationTime;
					break;
				case State::PLAY:
					if(m_state == State::END)
						m_playTime = 0s;
			}
			m_state = m_newState;
			m_stateChanged = false;
		}
	}

private:

	cr::steady_clock::time_point m_tLast;

	cr::duration<double> m_playTime{ 0 };
	cr::duration<double> m_newPlayTime{ 0 };
	bool m_playTimeChanged = false;

	State m_state{ State::STOP };
	State m_newState{ State::STOP };
	bool m_stateChanged = false;

	double m_fps{ 24 };

	size_t m_durationFrames{ 0 };
	cr::duration<double> m_durationTime{ 0 };

};


struct Animated {

	Animated(float v) {
		keyframes.push_back({ 0, v });
	}
	Animated(float v, size_t f) {
		keyframes.push_back({ f, v });
	}

	void move(size_t from, size_t to);

	void insert(size_t f, float v) {
		auto fr = findClosestFrameToRight(f);
		if(fr == keyframes.end() || fr->f != f)
			keyframes.insert(fr, {f, v});
		else fr->v = v;
	}

	Timeline *timeline{ nullptr };

	float val() {
		if(!timeline) return keyframes.begin()->v;

		size_t currFrame = timeline->frame();

		if(currFrame <= keyframes.begin()->f) return keyframes.begin()->v;
		if(currFrame >= keyframes.rbegin()->f) return keyframes.rbegin()->v;

		float ret{};
		auto firstKeyframe = keyframes.begin();

		for(auto it = keyframes.begin(); it != keyframes.end(); it++) {
			if(it->f <= currFrame && currFrame < std::next(it)->f) {
				firstKeyframe = it;
				break;
			}
		}

		float delta = std::next(firstKeyframe)->v - firstKeyframe->v;
		double percent = static_cast<double>(currFrame - firstKeyframe->f) / static_cast<double>(std::next(firstKeyframe)->f - firstKeyframe->f);

		//float intpVal = interpolate(delat, percent);

		return firstKeyframe->v + /* intpVal */ delta * percent;
	}

private:

	struct Keyframe {
		size_t f;
		float v;
	};

	std::list<Keyframe> keyframes;

	std::list<Keyframe>::iterator findClosestFrameToRight(size_t f) {
		auto last = keyframes.end();
		if(!keyframes.size()) return last;
		for(auto it = last; it != keyframes.begin(); it--) {
			if(std::prev(it)->f < f) return it;
		}
		return keyframes.begin();
	}
};
