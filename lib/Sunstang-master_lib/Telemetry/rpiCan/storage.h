#pragma once
#include<queue>
#include <pthread.h>
#include <linux/can.h>
#include <semaphore.h>
#include "frameHolder.h"

class storage{
	private:
		std::queue<frameHolder> data;
		pthread_mutex_t mutexData;
		sem_t semDataAmount;	
	public:
		frameHolder getNext();
		bool push(frameHolder);
		storage();	
};
