
#ifndef RAM_CORE_TASK
#define RAM_CORE_TASK

// Library Includes
#include <boost/utility.hpp>
#include <boost/thread/mutex.hpp>

// Forward declare boost::thread
namespace boost { class thread; }

// Project Includes
#include "core/include/IUpdatable.h"
#include "core/include/CountDownLatch.h"
#include "core/include/TimeVal.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

inline int timeval_compare(const struct timeval *a, const struct timeval *b) {
    assert(a);
    assert(b);

    if (a->tv_sec < b->tv_sec)
        return -1;

    if (a->tv_sec > b->tv_sec)
        return 1;

    if (a->tv_usec < b->tv_usec)
        return -1;

    if (a->tv_usec > b->tv_usec)
        return 1;

    return 0;
}

class RAM_EXPORT Task
{
public:

	// updateRate - microseconds
    Task(long updateRate, int priority, bool repeat);
	virtual ~Task();
	
	// getters
	long getUpdateRate();
	int getPriority();
	bool getRepeat();
	bool getEnabled();
	
	// setters
	void setUpdateRate(long updateRate);
	void setPriority(int priority);
	void setRepeat(bool repeat);
	void setEnabled(bool enabled);
	
	// gets the next time that the task is suppose to run
	timeval getNextRunTime();
	
	// gets the total time in microseconds that it has been running
	long getTotalRunTime();
	// gets the total time in microseconds that is has not been running
	long getTotalOffTime();
	
	// gets the average time in microseconds that it has been running
	long getAverageRunTime();
	// gets the average time in microseconds that it has not been running
	long getAverageOffTime();

	template<class T>
	struct compare_less : std::binary_function<T,T,bool>
	{
		bool operator() (const T& t1, const T& t2)
		{
			int r = timeval_compare(&(t1->m_nextRunTime), &(t2->m_nextRunTime));
			if (r < 0)
				return false;
			else if (r > 0)
				return true;
			else
			return t1->m_priority < t2->m_priority;
		}
	};
	
	void update() 
	{
		startUpdate();
		if (m_enabled)
			doUpdate();
		endUpdate();
	}

protected:
	// Must be implemented, run at the desired rate
	virtual void doUpdate() = 0;

private:
	// these should be called by classes implementing update()
	// they are called at the begining and end of update, respectively
	void startUpdate();
	void endUpdate();
    
	long m_updateRate;
	int m_priority;
	bool m_repeat;
	bool m_enabled;
	
	timeval m_lastRunStart;
	timeval m_lastRunEnd;
	timeval m_nextRunTime;
	long m_totalRunTime;
	long m_totalOffTime;
	int m_runs;
};


inline long Task::getUpdateRate()
{
	return m_updateRate;
}

inline int Task::getPriority()
{
	return m_priority;
}

inline bool Task::getRepeat()
{
	return m_repeat;
}

inline bool Task::getEnabled()
{
	return m_enabled;
}
	
inline void Task::setUpdateRate(long updateRate)
{
	m_updateRate = updateRate;
}

inline void Task::setPriority(int priority)
{
	m_priority = priority;
}

inline void Task::setRepeat(bool repeat)
{
	m_repeat = repeat;
}

inline void Task::setEnabled(bool enabled)
{
	m_enabled = enabled;
}


} // namespace core
} // namespace ram

#endif
