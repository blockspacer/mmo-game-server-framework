#include "pch.h"

#include "TimerManager.h"

TimerManager::TimerManager(HANDLE hIOCP) :
	hIOCP(hIOCP),
	nowTime(0), MAX_COOL_TIME(300 + 100),
	timerCont(),
	timerMemoryPool()
{
	timerCont.reserve(MAX_COOL_TIME);

	for (int i = 0; i < MAX_COOL_TIME; ++i) { timerCont.emplace_back(); }

	for (int i = 0; i < 500000; ++i) { timerMemoryPool.push(new TimerUnit()); }
}

TimerManager::~TimerManager()
{
	TimerUnit* retTimerUnit = nullptr;
	while (timerMemoryPool.try_pop(retTimerUnit)) { delete retTimerUnit; }

	for (auto& iter : timerCont)
	{
		while (iter.try_pop(retTimerUnit)) { delete retTimerUnit; }
	}
}

void TimerManager::TimerThread()
{
	while (7)
	{
		Sleep(100); //0.1초 슬립인데 이부분 보정좀 해줘야해

		nowTime.load() != MAX_COOL_TIME
			? nowTime.fetch_add(1)
			: nowTime = 0;

		TimerUnit* retTimerUnit = nullptr;
		while (timerCont[nowTime].try_pop(retTimerUnit))
		{
		//----- 꺼낸 타이머 유닛 처리.
			PostQueuedCompletionStatus(hIOCP, 0, retTimerUnit->objectKey, );
		//-----
			// 푸시 여기서 하지말고, Worker Thread에서 하는게 맞아.
			timerMemoryPool.push(retTimerUnit);
		}
	}
}

void TimerManager::AddTimerEvent(TimerUnit* inTimerUnit, int waitTime)
{
	waitTime += nowTime;

	if (waitTime >= MAX_COOL_TIME) waitTime -= MAX_COOL_TIME;

	timerCont[waitTime].push(inTimerUnit);
}

TimerUnit* TimerManager::GetTimerUnit()
{
	TimerUnit* retTimerUnit = nullptr;
	
	return timerMemoryPool.try_pop(retTimerUnit)
		? retTimerUnit
		: []()->TimerUnit* { std::cout << "TimerUnit 이 부족해서 추가 할당을 하였습니다. " << std::endl; new TimerUnit(); }();
}

