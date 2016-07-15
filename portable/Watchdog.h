// Watchdog.h
// Copyright 2016 Robin.Rowe@CinePaint.org
// License open source MIT

#ifndef Watchdog_h
#define Watchdog_h

#include <thread>
#include <mutex>
#include <condition_variable>

namespace portable
{

class Watchdog
{   typedef std::unique_lock<std::mutex> Lock;
    typedef std::chrono::milliseconds milliseconds;
    milliseconds delay;
    bool isGo;
	std::mutex m;
    std::condition_variable cv;
    void Run()
	{	while(isGo && delay.count())
		{   Lock lock(m);
			const bool isTimeout=std::cv_status::timeout==cv.wait_for(lock,delay);
			if(isGo)
			{   Action(isTimeout);
	}	}   }
    static void Main(Watchdog* self)
    {   self->Run();
    }
public:
    Watchdog()
	:	delay(0)
	,	isGo(false)
	{}
	void Start(unsigned delay)
    {	isGo =true;
		this->delay = milliseconds(delay);
		std::thread worker(Main,this);
		worker.detach();
    }
    virtual ~Watchdog()
    {   isGo=false;
    }
	void Wake()
	{	cv.notify_one();
	}
    void Stop()
    {   isGo=false;
		Wake();
    }
	virtual void Action(bool isTimeout)=0;
};

}

#endif