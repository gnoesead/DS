#pragma once
#include "Client_Defines.h"
#include "Base.h"

#include "CustomParticle.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CParticleManager final : public CBase
{
	DECLARE_SINGLETON(CParticleManager)

private:
	CParticleManager();
	virtual ~CParticleManager() = default;

public:
	void PlayParticle(const char* pPoolTag, _ubyte bShaderPass, _float3 vPos, 
					_double dSpriteSpeed, _int2 vSpriteCount, _float fLifeTime,
					const _tchar* pTextureTag, const _tchar* pBufferTag);

public:
	void Create_Particle(const char* pPoolTag, CCustomParticle::CUSTOMPARTDESC* pCustomPartDesc, CGameInstance* pGameInstance);
	void Collect_Particle(const char* pPoolTag, CGameObject* pGameObject);
	void Reuse_Particle(const char* pPoolTag, CCustomParticle::CUSTOMPARTDESC* pCustomPartDesc);

private:
	unordered_map<const char*, list<CGameObject*>*> m_ParticlePool;
	_int m_iCount = { 0 };

private:
	list<CGameObject*>* Find_ParticleList(const char* pPoolTag);

public:
	virtual void Free() override;
};

END