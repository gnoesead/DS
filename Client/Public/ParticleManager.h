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
		_double dSpriteSpeed = 1.0, _int2 vSpriteCount = { 1, 1 }, const _tchar* pRampTag = nullptr, _float fRampMax = 1.f);
	//Dir||GatherAndDisperse
	void PlayParticle(const char* pPoolTag, const _tchar* pBufferTag, const _tchar* pTextureTag,
		_float3 vPos, _float fLifeTime, _float fMinScale, _float fMaxScale, _float fMinSpeed, _float fMaxSpeed,
		_float3 vRange, _float3 vTickPerDir, _int3 vDirOption, CCustomParticle::CUSTOM_PARTICLE_PASS eShaderPass,
		_double dSpriteSpeed = 1.0, _int2 vSpriteCount = { 1, 1 }, const _tchar* pRampTag = nullptr, _float fRampMax = 1.f);
	//Size&Dir||Size&GaD
	void PlayParticle(const char* pPoolTag, const _tchar* pBufferTag, const _tchar* pTextureTag,
		_float3 vPos, _float fLifeTime, _float fMinScale, _float fMaxScale, _float fMinSpeed, _float fMaxSpeed,
		_float3 vRange, _float fTickPerSize, _float3 vTickPerDir, _int3 vDirOption, CCustomParticle::CUSTOM_PARTICLE_PASS eShaderPass,
		_double dSpriteSpeed = 1.0, _int2 vSpriteCount = { 1, 1 }, const _tchar* pRampTag = nullptr, _float fRampMax = 1.f);

private:
	unordered_map<const char*, list<CGameObject*>*> m_ParticlePool;
	_int m_iCount = { 0 };

private:
	list<CGameObject*>* Find_ParticleList(const char* pPoolTag);

public:
	virtual void Free() override;
};

/*
* Size
	_float3 vPos = Convert::ToFloat3(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	vPos.y += 1.f;
	_float3 vRange = { 1.f, 1.f, 1.f };
	// PoolTag, BufferTag, TextureTag, 
	// Pos, LifeTime, MinScale, MaxScale, MinSpeed, MaxSpeed, 
	// Range, TickPerSize, ShaderPass, SpriteSpeed, SpriteXY
	CParticleManager::GetInstance()->PlayParticle("Test",
		TEXT("Prototype_Component_VIBuffer_50_Particle"), TEXT("Prototype_Component_Texture_T_e_cmn_Grd_Radial007")
		, vPos, 2.f, 0.5f, 2.0f, 0.2f, 1.f, vRange, -1.f, CCustomParticle::PASS_RAMP, 1.f, _int2(1, 1),
		TEXT("Prototype_Component_Texture_Ramp08"), 0.98f);
*/
/*
* Dir
	_float3 vPos = Convert::ToFloat3(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//vPos.y += 1.f;
	_float3 vRange = { 0.1f, 0.f, 0.1f };
	_float3 vTPerD = { 1.f, 0.f, 1.f };
	// PoolTag, BufferTag, TextureTag,
	// Pos, LifeTime, MinScale, MaxScale, MinSpeed, MaxSpeed,
	// Range, TickPerDir, ShaderPass, SpriteSpeed, SpriteXY
	CParticleManager::GetInstance()->PlayParticle("Test2",
		TEXT("Prototype_Component_VIBuffer_20_Particle"), TEXT("Prototype_Component_Texture_T_e_Cmn_Smoke019")
		, vPos, 2.f, 0.5f, 2.0f, 1.f, 3.f, vRange, vTPerD, CCustomParticle::PASS_SPRITE_RAMP, 1.f, _int2(8, 8),
		TEXT("Prototype_Component_Texture_Ramp08"), 0.98f);
*/
/*
* Size&Dir
	_float3 vPos = Convert::ToFloat3(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//vPos.y += 1.f;
	_float3 vRange = { 0.1f, 0.f, 0.1f };
	_float3 vTPerD = { 1.f, 0.f, 1.f };
	// PoolTag, BufferTag, TextureTag, 
	// Pos, LifeTime, MinScale, MaxScale, MinSpeed, MaxSpeed, 
	// Range, TickPerSize, TickPerDir, ShaderPass, SpriteSpeed, SpriteXY
	CParticleManager::GetInstance()->PlayParticle("Test2",
		TEXT("Prototype_Component_VIBuffer_20_Particle"), TEXT("Prototype_Component_Texture_T_e_Cmn_Smoke019")
		, vPos, 2.f, 0.5f, 2.0f, 1.f, 3.f, vRange, 1.f, vTPerD, CCustomParticle::PASS_SPRITE_RAMP, 1.f, _int2(8, 8),
		TEXT("Prototype_Component_Texture_Ramp08"), 0.98f);
*/


END