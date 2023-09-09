#pragma once

#include "VIBuffer_Instance.h"
#include "MasterEffect.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance_Effect final : public CVIBuffer_Instance
{
public:
	typedef struct tagInstanceDesc
	{
		class CMasterEffect*		pParent = { nullptr };
	}INSTANCEDESC;

private:
	CVIBuffer_Point_Instance_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instance_Effect(const CVIBuffer_Point_Instance_Effect& rhs);
	virtual ~CVIBuffer_Point_Instance_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;
//
//public:
//	// 함수로 기능 제어
	virtual void Tick(_double TimeDelta) {};
	virtual void Tick(_double TimeDelta, INSTANCEDESC* pDesc);
	virtual HRESULT Render() override;
	void InitialSetting();		// 로드할때
	void Reset_Data();		// Collect할때

private:
	INSTANCEDESC		m_InstanceDesc;
	_double				m_dTimeAcc = { 0.0 };
	_double				m_dTimeAccDuration = { 0.0 };
	_double				m_dTimeAccRateOverTime = { 0.0 };
	size_t				m_iStartIndex = { 0 };
	_bool				m_bStart = { true };

	_float				m_fSpeedModifier = { 1.f };
	_float				m_fGravityModifier = { 0.f };
	_uint				m_iCurRateOverTimeIndex = { 0 };

	CMasterEffect::EFFECTDESC				m_eEffectDesc;
	vector<CMasterEffect::LIFETIMEVALUE>	m_RateOverTimes;
	vector<CMasterEffect::LIFETIMEVALUE>	m_SpeedOverLifeTimes;
	vector<CMasterEffect::LIFETIMEVALUE>	m_GravityModiferOverLifetimes;
	vector<CMasterEffect::LIFETIMEVALUE>	m_FrameOverTime;

	vector<CMasterEffect::LIFETIMEVALUE>	m_SizeOverLifeTimesX;
	vector<CMasterEffect::LIFETIMEVALUE>	m_SizeOverLifeTimesY;

	vector<CMasterEffect::LIFETIMEVALUE>	m_AlphaOverLifeTime;

public:
	static CVIBuffer_Point_Instance_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END