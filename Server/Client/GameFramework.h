/*
	kpu-warp-winapi-framework		Copyright ⓒ https://github.com/KPU-WARP

	#0. 해당 프로젝트는 한국산업기술대 게임공학과 학술 소모임 WARP의 오픈소스 WinAPI Framework입니다.
	#1. 관련 세부 정보는 깃헙 저장소 리드미에서 확인하실 수 있습니다.
		- https://github.com/KPU-WARP/winapi-framework

	#2. 기타 문의는, KoreaGameMaker@gmail.com으로 부탁드립니다. 감사합니다 :)
*/

#pragma once

class NetworkManager;
class RenderModelManager;

class StretchModel;
class TransparentModel;

class BaseActor;
class Pawn;

class WGameFramework
{
	using _ClientKeyType = UINT;
	using _OtherClientCont = std::list<std::pair<_ClientKeyType, std::unique_ptr<Pawn>>>;

public:
	WGameFramework(const std::string_view&);
	~WGameFramework();

	void Create(HWND hWnd);
	void OnDraw(HDC hdc);
	void OnUpdate(const float frameTime = 0.17f);
	void KeyBoard(UINT iMessage, WPARAM wParam, LPARAM lParam);
	void Mouse(UINT iMessage, WPARAM wParan, LPARAM lParam);

public:	// Called in NetworkManager
	void RecvLoginOK(char*);
	void RecvPutPlayer(char*);
	void RecvRemovePlayer(char*);
	void RecvPosition(char*);
	void RecvChat(char*);
	void RecvLoginFail(char*);
	void RecvStatChange(char*);

	static void StarChatFunction(LPVOID arg);
	void ChatFunction();
private:
	void UpdateBackgroundActor();
	void UpdateOtherObject();

private:
	HWND	m_hWnd;

private:
	std::unique_ptr<NetworkManager> networkManager;
	std::unique_ptr<RenderModelManager> renderModelManager;

private:
	std::unique_ptr<Pawn> playerCharacter;
	std::vector<std::vector<std::unique_ptr<Pawn>>> backgroundActorCont;

	_OtherClientCont otherPlayerCont;
	std::mutex otherPlayerContLock;	// 이 바보같은 락 나중에 백스페이스로 고냥 뿌셔버리기

	_OtherClientCont monsterCont;
	std::mutex monsterContLock;	

	std::unique_ptr<BaseActor> coverUI;
	std::unique_ptr<BaseActor> broadcastAreaUI;

	bool isLogin;
	bool isDeath;
	std::unique_ptr<CImage> notLoginUI;
	std::unique_ptr<CImage> diedUI;

	std::vector<CImage> numberUICont;

	CImage AttackImageCont[3][3];
private:
	std::string ipAddress;
	_ClientKeyType myClientKey;

	int attack_0_tick;
	int attack_1_tick;
	int attack_2_tick;

public:
	std::thread chatThread;
	int level;
	int hp;
	int myExp;
	int mp;
	int job;

	int money;

	int redCount;
	int blueCount;
	int treeCount;

	int moveOK;
	int attackOK;
	int skill1OK;
	int skill2OK;

	int posX;
	int posY;
};