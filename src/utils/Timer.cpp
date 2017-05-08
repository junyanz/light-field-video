#include "Timer.h"

//Define the timer class without a name
CTimer::CTimer() {
    m_name = "";
#ifdef _OPENMP
    m_start_mp=omp_get_wtime();
#else
    m_start = clock();
#endif
    m_end = 0;
    m_end_mp = 0;
    //cout << "Start Function <" << m_name << ">..." << endl;
}

//Define the timer class with a specific name 
CTimer::CTimer(string timerName, bool isTiming) {
    assert(timerName.length() > 0);
    m_isTime = isTiming;
    m_name = timerName;
#ifdef _OPENMP
    m_start_mp=omp_get_wtime();
#else
    m_start = clock();
#endif
    /*if (isTiming)
        cout << "Start Function <" << m_name << ">..." << endl;*/
}

double CTimer::Time() {
    #ifdef _OPENMP
    m_end_mp = omp_get_wtime();
    double t = m_end_mp - m_start_mp;
    #else
    m_end = clock();
    double t = (m_end - m_start) / (double) CLOCKS_PER_SEC;
    #endif
    return t;
}

//End the timing when destroyed 
CTimer::~CTimer() {
#ifdef _OPENMP
    m_end_mp = omp_get_wtime();
    double t = m_end_mp - m_start_mp;
#else
    m_end = clock();
    double t = (m_end - m_start) / (double) CLOCKS_PER_SEC;
#endif
    if (m_isTime)
        cout << "Timing <" << m_name << "> " << t << endl;
}
