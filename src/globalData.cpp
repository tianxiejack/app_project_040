
#include "globalData.h"

CGlobalData* CGlobalData::_Instance = NULL;
pthread_mutex_t CGlobalData::mtx;

CGlobalData::CGlobalData()
{
	pthread_mutex_init(&mtx,0);
}

CGlobalData* CGlobalData::getInstance()
{
	if(_Instance == NULL)
	{
		pthread_mutex_lock(&mtx);
		_Instance = new CGlobalData();
		pthread_mutex_unlock(&mtx);
	}
	return _Instance;
}
