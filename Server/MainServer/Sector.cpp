#include "pch.h"
#include "../Define.h"
#include "ServerDefine.h"

#include "ClientContUnit.h"
#include "MemoryUnit.h"
#include "UserData.h"

#include "ObjectInfo.h"
#include "BaseMonster.h"

#include "Sector.h"

#include "TimerManager.h"

Sector::Sector(const BYTE inX, const BYTE inY)
	: indexX(inX)
	, indexY(inY)
	, centerX(inX * GLOBAL_DEFINE::SECTOR_DISTANCE + GLOBAL_DEFINE::SECTOR_HALF_DISTANCE)
	, centerY(inY * GLOBAL_DEFINE::SECTOR_DISTANCE + GLOBAL_DEFINE::SECTOR_HALF_DISTANCE)
	, sectorContUnit(new SectorContUnit)
{
	//std::cout << "indexX : " << (int)indexX << ", centerX : " << (int)centerX << ", indexY : " << (int)indexY << ", centerY : " << (int)centerY << std::endl;
}

Sector::~Sector()
{
	delete sectorContUnit;
}

/*
	Join
		- 섹터에 새로운 클라이언트가 들어옵니다.
*/
void Sector::Join(SocketInfo* pClient)
{
	sectorContUnit->clientLock.lock(); //+++++++++++++++++++++++++++++++++++++1
	sectorContUnit->clientCont.emplace(pClient->key);
	sectorContUnit->clientLock.unlock(); //-----------------------------------0

	pClient->objectInfo->sectorIndexX = indexX;
	pClient->objectInfo->sectorIndexY = indexY;
}

/*
	Exit
		- 섹터에 있던 클라이언트가 나갑니다.
*/
void Sector::Exit(SocketInfo* pInClient)
{
	// 지역 정보에서 나를 지워주고.
	sectorContUnit->clientLock.lock(); //+++++++++++++++++++++++++++++++++++++1
	
	// 이게 무슨 미친놈의 코드야;
	//for (auto iter = sectorContUnit->clientCont.begin()
	//	; iter != sectorContUnit->clientCont.end()
	//	; ++iter)
	//
	//	if (pInClient->key == *iter)
	//	{
	//		sectorContUnit->clientCont.erase(iter);
	//		break;
	//	}
	//

	sectorContUnit->clientCont.erase(pInClient->key);

	sectorContUnit->clientLock.unlock(); //-----------------------------------0
}

void Sector::JoinForNpc(BaseMonster* pClientObject)
{
	sectorContUnit->monsterLock.lock(); //+++++++++++++++++++++++++++++++++++++1
	sectorContUnit->monsterCont.emplace(pClientObject->key);
	sectorContUnit->monsterLock.unlock(); //-----------------------------------0

	pClientObject->objectInfo->sectorIndexX = indexX;
	pClientObject->objectInfo->sectorIndexY = indexY;
}

void Sector::ExitForNpc(BaseMonster* pMonster)
{
	sectorContUnit->monsterLock.lock(); //+++++++++++++++++++++++++++++++++++++1
	
	// 니도 이게 무슨 코드야;
	//for (auto iter = sectorContUnit->monsterCont.begin()
	//	; iter != sectorContUnit->monsterCont.end()
	//	; ++iter)
	//{
	//	if (pInClient->key == *iter)
	//	{
	//		sectorContUnit->monsterCont.erase(iter);
	//		break;
	//	}
	//}
	sectorContUnit->monsterCont.erase(pMonster->key);

	sectorContUnit->monsterLock.unlock(); //-----------------------------------0
}

/*
	JudgeClientInViewList
	 - Sector의 컨테이너에서 볼수 있는 클라이언트를 판단하고, viewList의 여부에 따라 처리합니다..

	 !0. 미친 이상한 구조 떔시, ZoneContUnit을 갖고 옵니다... 잘못쓰면 성능 간다간다 훅간다!
	 !1. 서로의 viewList에 넣을 때, 동기화 백프로 꺠질걸?? ConCurrency가 아닌 Lock을 써야 될꺼 같어.
	 !2. 내부에서 네트워크 함수가 호출됩니다.
*/
void Sector::MakeNewViewList(std::unordered_set<_KeyType>& newViewList, std::unordered_set<_KeyType>& newMonsterViewList, SocketInfo* pClient, ZoneContUnit* pZoneContUnit)
{
	if (sectorContUnit->clientCont.size() > 0)
	{
		sectorContUnit->clientLock.lock_shared();	//++++++++++++++++++++++++++++1	Sector : Read Lock!
		for (auto/*&*/ otherKey : sectorContUnit->clientCont)
		{
			if (otherKey == pClient->key) continue;

			SocketInfo* pOtherClient = pZoneContUnit->clientContArr[otherKey];

			//if (!isOn) continue; // 이럴일 없을듯해.

			if (IsSeeEachOther(pClient->objectInfo, pOtherClient->objectInfo))
			{
				// 서로 보입니다.
				newViewList.insert(otherKey);
				/*
				if (pClient->viewList.find(otherKey) == pClient->viewList.end())
				{
					// 서로 보이고, 서로 모르는 사이였을 때.
					SendPutPlayer(pClient->objectInfo, pOtherClient);
					SendPutPlayer(pOtherClient->objectInfo, pClient);

					// 서로의 뷰 리스트에 추가할 때... 문제가 될 수 있습니다.
					pClient->viewList.insert(otherKey);
					pOtherClient->viewList.insert(pClient->objectInfo->key);
				}
				else
				{
					//서로 보이고, 서로 아는 사이였을 때,

					// 나는 내가 알아서 할게 너 나 바뀐거 받아라 얌마!
					SendMovePlayer(pClient->objectInfo, pOtherClient);
				}
				*/
			}
			/*
			else
			{
				// 서로 안보입니다.
				if (pClient->viewList.find(otherKey) != pClient->viewList.end())
				{
					// 서로 안 보이고, 서로 원래 알던 클라이언트였을 때.
					SendRemovePlayer(pClient->objectInfo->key, pOtherClient);
					SendRemovePlayer(otherKey, pClient);

					// 서로의 뷰 리스트에서 삭제할 때... 문제가 될 수 있습니다.
					// 언세이프 보이지?? 살벌하다살벌해
					pClient->viewList.unsafe_erase(otherKey);
					pOtherClient->viewList.unsafe_erase(pClient->objectInfo->key);
				}
				//else
				//{
				//	// 서로 안 보이고, 서로 원래 모르던 클라이언트였을 때.
				//}
			}
			*/

		}
		sectorContUnit->clientLock.unlock_shared();	//----------------------------0	Sector : Read Lock!
	}

	if (sectorContUnit->monsterCont.size() > 0)
	{
		sectorContUnit->monsterLock.lock_shared(); //++++++++++++++++++++++++++++1	Sector : Read Lock!
		for (auto/*&*/ otherKey : sectorContUnit->monsterCont)
		{
			auto pMonster = pZoneContUnit->monsterCont[otherKey - BIT_CONVERTER::NOT_PLAYER_INT];
			if (IsSeeEachOther(pClient->objectInfo, pMonster->objectInfo))
			{
				// 서로 보입니다.
				newMonsterViewList.insert(otherKey);
				/*
				if (pClient->monsterViewList.find(otherKey) == pClient->monsterViewList.end())
				{
					// 서로 보이고, 서로 모르는 사이였을 때.
					SendPutPlayer(pMonster->objectInfo, pClient);

					// 서로의 뷰 리스트에 추가할 때... 문제가 될 수 있습니다.
					pClient->monsterViewList.insert(otherKey);
				}
				else
				{
					//서로 보이고, 서로 아는 사이였을 때,

					// 나는 내가 알아서 할게 너 나 바뀐거 받아라 얌마!
					SendMovePlayer(pMonster->objectInfo, pClient);
				}
				*/
			}
			/*
			else
			{
				// 서로 안보입니다.
				if (pClient->monsterViewList.find(otherKey) != pClient->monsterViewList.end())
				{
					// 서로 안 보이고, 서로 원래 알던 클라이언트였을 때.
					//SendRemovePlayer(pClient->objectInfo->key, pOtherClient);
					SendRemovePlayer(otherKey, pClient);

					// 서로의 뷰 리스트에서 삭제할 때... 문제가 될 수 있습니다.
					// 언세이프 보이지?? 살벌하다살벌해
					pClient->monsterViewList.unsafe_erase(otherKey);
				}
				//else
				//{
				//	// 서로 안 보이고, 서로 원래 모르던 클라이언트였을 때.
				//}
			}
			*/
		}
		sectorContUnit->monsterLock.unlock_shared(); //----------------------------0	Sector : Read Lock!
	}
}

/*bool*/ void Sector::MakeNewViewListForNpc(std::unordered_set<_KeyType>& newViewList, BaseMonster* pMonster, ZoneContUnit* pZoneContUnit)
{
	sectorContUnit->clientLock.lock_shared();	//++++++++++++++++++++++++++++1	Sector : Read Lock!
	// 대충 0명이느냐 아니느냐 검사
	
	if (sectorContUnit->clientCont.size() == 0) {
		/*std::cout << "텅비어있습니다!" << std::endl; */ 
		sectorContUnit->clientLock.unlock_shared();	//----------------------------0	Sector : Read Lock!
		return; //이게 많아야할텐데? }
	}

	for (auto/*&*/ otherKey : sectorContUnit->clientCont)
	{
		//auto[isOn, pOtherClient] = pZoneContUnit->FindClient(otherKey /*- BIT_CONVERTER::NOT_PLAYER_INT*/);
		//if (!isOn) continue; // ? 왜 없댱
		if (IsSeeEachOther(pMonster->objectInfo, pZoneContUnit->clientContArr[otherKey]->objectInfo))
		{
			newViewList.emplace(otherKey);
			// 서로 보입니다.
			//if (pOtherClient->monsterViewList.find(pClient->key /* - BIT_CONVERTER::NOT_PLAYER_INT*/) == pOtherClient->monsterViewList.end())
			//{
			//	// 서로 보이고, 서로 모르는 사이였을 때.
			//	SendPutPlayer(pClient, pOtherClient);
			//	//SendPutPlayer(pOtherClient->objectInfo, pClient);
			//
			//	// 서로의 뷰 리스트에 추가할 때... 문제가 될 수 있습니다.
			//	//pClient->viewList.insert(otherKey);
			//	pOtherClient->monsterViewList.insert(pClient->key);
			//}
			//else
			//{
			//	//서로 보이고, 서로 아는 사이였을 때,
			//
			//	// 나는 이미 움직였어, 너 나 바뀐거 받아라 얌마!
			//	SendMovePlayer(pClient, pOtherClient);
			//}
		}
		//else
		//{
		//	// 서로 안보입니다.
		//	if (pOtherClient->monsterViewList.find(pClient->key) != pOtherClient->monsterViewList.end())
		//	{
		//		// 서로 안 보이고, 서로 원래 알던 클라이언트였을 때.
		//
		//		SendRemovePlayer(pClient->key, pOtherClient);
		//		//SendRemovePlayer(otherKey, pClient);
		//
		//		// 서로의 뷰 리스트에서 삭제할 때... 문제가 될 수 있습니다.
		//		// 언세이프 보이지?? 살벌하다살벌해
		//		pOtherClient->monsterViewList.unsafe_erase(pClient->key);
		//	}
		//	//else
		//	//{
		//	//	// 서로 안 보이고, 서로 원래 모르던 클라이언트였을 때.
		//	//}
		//}
	}
	sectorContUnit->clientLock.unlock_shared();	//----------------------------0	Sector : Read Lock!
}

void Sector::MakeOldViewListForNpc(std::unordered_set<_KeyType>& retOldViewList, BaseMonster* pMonster, ZoneContUnit* pZoneContUnit)
{
	sectorContUnit->clientLock.lock_shared();
	for (auto/*&*/ otherKey : sectorContUnit->clientCont)
	{
		if (IsSeeEachOther(pMonster->objectInfo, pZoneContUnit->clientContArr[otherKey]->objectInfo))
		{
			retOldViewList.emplace(otherKey);
		}
	}
	sectorContUnit->clientLock.unlock_shared();
}
/*
	IsSeeEachOther
		- 마! 서로 볼수 있나 없나! 위치값 둘다 내나 봐라 마!
*/
//bool Sector::IsSeeEachOther(const _PosType aPosX, const _PosType aPosY, const _PosType bPosX, const _PosType bPosY) const noexcept
//{
//	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(aPosX - bPosX)) return false;
//	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(aPosY - bPosY)) return false;
//	return true;
//}
//
//bool Sector::IsSeeEachOther(const std::pair<_PosType, _PosType>& inAPosition, const std::pair<_PosType, _PosType>& inBPosition) const noexcept
//{
//	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(inAPosition.first - inBPosition.first)) return false;
//	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(inAPosition.second - inBPosition.second)) return false;
//	return true;
//}

bool Sector::IsSeeEachOther(const ObjectInfo* const objectA, const ObjectInfo* const objectB) const noexcept
{
	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(objectA->posX - objectB->posX)) return false;
	if (GLOBAL_DEFINE::VIEW_DISTANCE < abs(objectA->posY - objectB->posY)) return false;
	return true;
}

