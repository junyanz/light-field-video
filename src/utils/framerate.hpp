#ifndef FRAMERATE_HPP_
#define FRAMERATE_HPP_

#include "timing.hpp"
#include <deque>

namespace utils {


class CFrameRate {

public:
	CFrameRate():m_queue_size(50) { reset(); }
	CFrameRate(int queue_size):m_queue_size(queue_size) { reset(); }

	void setQueueSize( int queue_size ) { m_queue_size = queue_size; }

	void reset() {
		m_frame_times.clear();
		m_timer.restart();
		m_frame_count = 0;
	}

	int getFrameCount() const { return m_frame_count; }

	void tick() {
		m_frame_count++;
		m_frame_times.push_back( m_timer.elapsed() );
		if( int(m_frame_times.size()) > m_queue_size ) m_frame_times.pop_front();
	}

	float getFrameRate() const {
		if( m_frame_times.size() > 1 ) {
			return float(m_frame_times.size()-1) * 1000.0 / (m_frame_times.back()-m_frame_times.front());
		}
		else return 0;
	}

private:
	int m_queue_size;
	utils::timing m_timer;
	std::deque<float> m_frame_times;
	int m_frame_count;
};

}

#endif /* FRAMERATE_HPP_ */
