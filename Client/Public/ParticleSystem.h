#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "Effect.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CParticleSystem final : public CGameObject
{
private:
	CParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticleSystem(const CParticleSystem& rhs);
	virtual ~CParticleSystem() = default;

public:
	virtual HRESULT	Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render(void) override;

public:
	class CParticleSystem* Get_Part(int iIndex) {
		return (iIndex <= m_iNumEffects) ? m_PartEffects[iIndex] : nullptr;
	}
	class CEffect* Get_Effect(void) {
		return m_pEffect;
	}
	int		Get_NumEffects(void) {
		return m_iNumEffects;
	}
	_bool	Get_isPlaying(void) {
		return m_bPlay;
	}
	_bool	Get_isStopped(void) {
		return m_bStop;
	}
	_matrix Get_WorldMatrix(void) {
		return Convert::ToMatrix(m_WorldMatrix);
	}
	_float3 Get_Postion(void) {
		return m_vPosition;
	}
	_float3 Get_Rotation(void) {
		return m_vRotation;
	}
	_float3 Get_Scale(void) {
		return m_vScale;
	}
	CTransform* Get_Transform(void) {
		return m_pTransformCom;
	}


public:
	void Set_LevelIndex(_uint iIndex) {
		m_iLevelIndex = iIndex;
	}
	void Set_NumEffects(int iNum) {
		m_iNumEffects = iNum;	m_PartEffects.reserve(m_iNumEffects);
	}
	void Set_Position(_float3 vPos) {
		m_vPosition = vPos;
	}
	void Set_Rotation(_float3 vRot) {
		m_vRotation = vRot;
	}
	void Set_Scale(_float3 vScale) {
		m_vScale = vScale;
	}
	void Set_EffectDesc(CEffect::EFFECTDESC* pDesc) {
		m_pEffect->Set_EffectDesc(pDesc);
	}
	void Set_isPlaying(_bool bPlay) {
		m_bPlay = bPlay;
	}
	void Set_isStopped(_bool bStop) {
		m_bStop = bStop;
	}
	void Set_Parent(class CTransform* pTransformCom, CParticleSystem* pParticleSystem) {
		m_pParentTransform = pTransformCom; m_pParent = pParticleSystem;
	}
	void Set_PartsParent(class CTransform* pTransformCom);
	void Set_isCollect(_bool bCollect) {
		m_bCollect = bCollect;
	}
	void Set_Effect(CEffect* pEffect) {
		m_pEffect = pEffect;
	}
	void Set_ParentMatrix(_float4x4 WorldMatrix) {
		m_ParentWorldMatrix = WorldMatrix;
	}
	void Set_IsParticle(_bool isParticle) {
		m_isParticle = isParticle;
	}
	void Set_Tag(const char* pTag) {
		m_pTag = pTag;
	}
	void Set_IsParts(_bool isParts) {
		m_isParts = isParts;
	}
	void Set_ParentPartsObject(class CEffectPartsObject* pObj) {
		m_pParentObject = pObj;
	}


public:
	HRESULT Create_Effect(int eEffectType);
	HRESULT Clone_Effect(int eEffectType);
	HRESULT Add_Component_Texture(_uint iLevelIndex, const _tchar* pComponentTag, int eType);
	HRESULT Add_Component_Model(_uint iLevelIndex, const _tchar* pComponentTag);

public:
	void Add_Parts(CParticleSystem* pParticleSystem) {
		m_PartEffects.push_back(pParticleSystem);
	}
	void Play_Parts(_bool isPlaying);
	void Stop_Parts(void);
	void Clear(void);

private:
	class CTransform*			m_pTransformCom = { nullptr };
	class CTransform*			m_pParentTransform = { nullptr };
	class CEffectPartsObject*	m_pParentObject = { nullptr };
	_float4x4			m_WorldMatrix;
	_float4x4			m_ParentWorldMatrix;
	CParticleSystem*	m_pParent = { nullptr };

private:
	vector<CParticleSystem*>	m_PartEffects;
	int							m_iNumEffects = { 0 };
	_int						m_iCurEffectIndex = { -1 };

	_uint					m_iLevelIndex = { 0 };
	class CEffect*			m_pEffect = { nullptr };

private:
	_float3					m_vPosition = { 0.f, 0.f, 0.f};
	_float3					m_vRotation = { 0.f, 0.f, 0.f };
	_float3					m_vScale = { 1.f, 1.f, 1.f };

private:
	_bool					m_bPlay = { false };
	_bool					m_bStop = { false };
	_double					m_dTimeAccTotal = { 0.0 };
	_double					m_dTimeAccCycle = { 0.0 };
	_bool					m_bCollect = { false };
	_bool					m_bInitialTransformSetting = { true };

	_bool					m_isParticle = { false };
	const char*				m_pTag = { nullptr };

	_bool					m_isParts = { false };

private:
	HRESULT Add_Components(void);
	HRESULT SetUp_ShaderResources(void);

public:
	static CParticleSystem* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free(void) override;
};

END