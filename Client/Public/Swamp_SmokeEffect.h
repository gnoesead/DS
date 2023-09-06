#pragma once

#include "Client_Defines.h"
#include "EffectW.h"

BEGIN(Client)

class CSwamp_SmokeEffect final : public CEffectW
{
private:
	CSwamp_SmokeEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSwamp_SmokeEffect(const CSwamp_SmokeEffect& rhs);
	virtual ~CSwamp_SmokeEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

private:
	_float					m_fAlpha = { 1.f };
	_float					m_fAlphaDecreseSpeed = { 0.f };

public:
	static CSwamp_SmokeEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END