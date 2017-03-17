#include "storage.h"
#include<queue>
#include <iostream>

storage::storage()
{
	pthread_mutex_init(&mutexData, NULL);  //intialize the mutex
	sem_init(&semDataAmount, 0, 0);
}

frameHolder storage::getNext()
{
	int semval;
	sem_getvalue(&semDataAmount, &semval);

	sem_wait(&semDataAmount);
	pthread_mutex_lock(&mutexData);
	frameHolder front = data.front();
 	data.pop();
	pthread_mutex_unlock(&mutexData);
	return front;
}

bool storage::push(frameHolder toSend)
{
	pthread_mutex_lock(&mutexData);
	data.push(toSend);		
	sem_post(&semDataAmount);
	pthread_mutex_unlock(&mutexData);
}
