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
	void Create_Particle(const char* pPoolTag, CCustomParticle::CUSTOMPARTDESC* pCustomPartDesc, CGameInstance* pGameInstance);
	void Collect_Particle(const char* pPoolTag, CGameObject* pGameObject);
	void Reuse_Particle(const char* pPoolTag, CCustomParticle::CUSTOMPARTDESC* pCustomPartDesc);

public:
	//Size
	void PlayParticle(const char* pPoolTag, const _tchar* pBufferTag, const _tchar* pTextureTag,
		_float3 vPos, _float fLifeTime, _float fMinScale, _float fMaxScale, _float fMinSpeed, _float fMaxSpeed,
		_float3 vRange, _float fTickPerSize, CCustomParticle::CUSTOM_PARTICLE_PASS eShaderPass,
		_double dSpriteSpeed = 1.0, _int2 vSpriteCount = { 1, 1 }, const _tchar* pRampTag = nullptr);
	//Dir
	void PlayParticle(const char* pPoolTag, const _tchar* pBufferTag, const _tchar* pTextureTag,
		_float3 vPos, _float fLifeTime, _float fMinScale, _float fMaxScale, _float fMinSpeed, _float fMaxSpeed,
		_float3 vRange, _float3 vTickPerDir, CCustomParticle::CUSTOM_PARTICLE_PASS eShaderPass,
		_double dSpriteSpeed = 1.0, _int2 vSpriteCount = { 1, 1 }, const _tchar* pRampTag = nullptr);
	//Size&Dir
	void PlayParticle(const char* pPoolTag, const _tchar* pBufferTag, const _tchar* pTextureTag,
		_float3 vPos, _float fLifeTime, _float fMinScale, _float fMaxScale, _float fMinSpeed, _float fMaxSpeed,
		_float3 vRange, _float fTickPerSize, _float3 vTickPerDir, CCustomParticle::CUSTOM_PARTICLE_PASS eShaderPass,
		_double dSpriteSpeed = 1.0, _int2 vSpriteCount = { 1, 1 }, const _tchar* pRampTag = nullptr);

private:
	unordered_map<const char*, list<CGameObject*>*> m_ParticlePool;
	_int m_iCount = { 0 };

private:
	list<CGameObject*>* Find_ParticleList(const char* pPoolTag);

public:
	virtual void Free() override;
};

END