#ifndef _CTIMING_H_
#define _CTIMING_H_

#define TIMING_START		::utils::timing timing; timing.m_start();
#define TIMING_END(x)		timing.print(x);

#ifdef WIN32

#include <windows.h>
#include <stdio.h>

namespace utils {

// timing in milliseconds
	class timing
	{
	public:
		timing() {
			QueryPerformanceFrequency(&m_freq);
		}

		void start() {
			QueryPerformanceCounter(&m_time);
		}

		float restart() {
			float val = elapsed();
			QueryPerformanceCounter(&m_time);
			return val;
		}

		float elapsed() {
			return now() - (*this);
		}

		void print(const char * _name)
		{
			float time = elapsed();
			printf("%s: took %f milliseconds\n", _name, time);
			fflush(stdout);
			restart();
		}

		static inline timing now() {
			timing t;
			t.start();
			return t;
		}

		float operator - (const timing & t1) {
		//return (float)1000.0f*(m_time.tv_sec  - t1.m_time.tv_sec) +
	      // 1.0e-3f*(m_time.tv_usec - t1.m_time.tv_usec);
			return float((double)(m_time.QuadPart-t1.m_time.QuadPart)/(double)m_freq.QuadPart * 1000.0);
    }

	private:
		LARGE_INTEGER m_time;
		LARGE_INTEGER m_freq;
	};
}


#else

#include <sys/time.h>
#include <stdio.h>
#include <time.h>

namespace utils {

// timing in milliseconds
	class timing
	{
	public:
		void start() {
			gettimeofday( &m_time, NULL );
		}

		float restart() {
			float val = elapsed();
			gettimeofday( &m_time, NULL );
			return val;
		}

		float elapsed() {
			return now() - (*this);
		}

		void print(const char * _name)
		{
			float time = elapsed();
			printf("%s: took %f milliseconds\n", _name, time);
			fflush(stdout);
			restart();
		}

		static inline timing now() {
			timing t;
			t.start();
			return t;
		}

		float operator - (const timing & t1) {
		return (float)1000.0f*(m_time.tv_sec  - t1.m_time.tv_sec) +
	       1.0e-3f*(m_time.tv_usec - t1.m_time.tv_usec);
    }

	private:
		timeval m_time;
	};
}

#endif

#endif //_CTIMING_H_
