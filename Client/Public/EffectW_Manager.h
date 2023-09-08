#pragma once

#include "Engine_Defines.h"
#include "Client_Defines.h"

BEGIN(Client)

class CEffectW_Manager
{
public:
	enum EFFECTW_TYPE{ EFFECT_GROUNDSMOKE , EFFECT_SWAMPWATER , EFFECT_STONEPARTICLE , EFFECT_SWAMPSMOKE};

public:
	static CEffectW_Manager* Get_Instance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CEffectW_Manager;

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
private:
	CEffectW_Manager();
	~CEffectW_Manager();

public:
	HRESULT Add_Effect_In_Pool(EFFECTW_TYPE eEffectType);

public:
	void	Play(EFFECTW_TYPE eEffectType, void* pArg);
	void	Reuse_Effect(EFFECTW_TYPE eEffectType, void* pArg);

	void	Collect_Effect(EFFECTW_TYPE eEffectType , class CEffectW* pEffectW);

private:
	static CEffectW_Manager* m_pInstance;

private:
	list<class CEffectW*>							m_GroundSmokePool;
	list<class CEffectW*>							m_SwampWaterPool;
	list<class CEffectW*>							m_StoneParticlePool;
	list<class CEffectW*>							m_SwampSmokePool;

public:
	void Release(void);
};

END