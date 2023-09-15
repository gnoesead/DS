#pragma once
#include "FMOD/fmod.h"
#include "Engine_Defines.h"
#include "Client_Defines.h"

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.2f
#define SOUND_WEIGHT 0.1f

class CSoundMgr
{
public:
	static CSoundMgr* Get_Instance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CSoundMgr;

		return m_pInstance;
	}
	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
public:
	enum CHANNELID { BGM, PLAYER_IDLE,
		PLAYER_RUN_0, PLAYER_RUN_1, PLAYER_JUMP, PLAYER_SLASH, PLAYER_VOICE, PLAYER_VOICE_SUB,
		SWORD_0, SWORD_1, SWORD_AWAKEN_0, SWORD_AWAKEN_1, SKILL_0, SKILL_1, SKILL_EFFECT,
		MONSTER_MOVE_0, MONSTER_MOVE_1, MONSTER_EFFECT_0, MONSTER_EFFECT_1,
		MONSTER_VOICE, MONSTER_VOICE_SUB,
		METAL_0, METAL_1,
		EFFECT, UI, SYSTEM_EFFECT, SYSTEM_EFFECT2, SYSTEM_EFFECT3, MAXCHANNEL };
private:
	CSoundMgr();
	~CSoundMgr();

public:
	void Initialize();
	void Release();

public:
	int  VolumeUp(CHANNELID eID, _float _vol);
	int  VolumeDown(CHANNELID eID, _float _vol);

	int  BGMVolumeUp(_float _vol);
	int  BGMVolumeDown(_float _vol);

	int  Pause(CHANNELID eID);

	void PlaySound(TCHAR* pSoundKey, CHANNELID eID, _float _vol);
	void PlayBGM(TCHAR* pSoundKey, _float fVolume);

	void StopSound(CHANNELID eID);
	void StopAll();

	void Tick();

private:
	float m_volume = SOUND_DEFAULT;
	float m_BGMvolume = SOUND_DEFAULT;



	FMOD_BOOL m_bool;

private:
	void LoadSoundFile();

private:
	static CSoundMgr* m_pInstance;
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;
	_bool		m_bPause = false;
};

