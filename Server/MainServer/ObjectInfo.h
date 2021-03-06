#pragma once

#include "InHeaderDefine.hh"

/*
	몬스터와, NPC가 공통으로 사용할 Object의 정보를 담고 있음.
*/
struct ObjectInfo
{
public:
	ObjectInfo();
	ObjectInfo(_PosType,  _PosType);
	virtual ~ObjectInfo() = default;

public:	// 이럴꺼면 님 왜 이거 class했어요? -> 조용히해 -> 결국 Struct...ㅎ
	_PosType posX;
	_PosType posY;

	_LevelType level;
	
	_JobType job;

	_HpType hp;

	_DamageType damage;

	_TickCountType faintTick;	// 기절 중, 또 기절데미지를 받으면, 해당 시간을 모두 계산해야함

	_TickCountType burnTick;	//불 카운트 

	_SectorIndexType sectorIndexX;
	_SectorIndexType sectorIndexY;

	BYTE possibleSectorCount;
	std::array<std::pair<_SectorIndexType, _SectorIndexType>, 3> sectorArr;
};

/*
	결국 상속을 사용합니다..
*/
struct PlayerObjectInfo : public ObjectInfo
{
	_NicknameType nickname[10];
	_MpType mp;
	_ExpType exp;
	_MoneyType money;
	_CountType redCount;
	_CountType blueCount;
	_TreeCountType treeCount;

	_FlagType noDamageFlag;	// 한번 실행하면 그 이후, off된 후 on되는 것이 확정됨.
	_FlagType selfHealFlag;	// 피 회복은 onoFF로 처리가 확정됨.
	_FlagType moveFlag;
	_FlagType attackFlag;

	_FlagType skill1Flag;
	_FlagType skill2Flag;

	_TickCountType redTickCount;
	_TickCountType blueTickCount;

public:
	PlayerObjectInfo();
	PlayerObjectInfo(_NicknameType*, _PosType, _PosType);
	~PlayerObjectInfo() final = default;
};