#pragma once

#include "Client_Defines.h"

#include "Effect.h"

BEGIN(Engine)
class CVIBuffer_Point_Instance_Effect;
END

BEGIN(Client)

class CEffect_Particle final : public CEffect
{
private:
	CEffect_Particle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CEffect_Particle(const CEffect_Particle& rhs);
	virtual ~CEffect_Particle() = default;

public:
	virtual HRESULT	Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render(void) override;

	virtual void Set_Initial_Data(void) override;
	virtual void Reset_Data(void) override;


public:
	void Set_CameraRightLookPos(_float2 vPos) {
		m_vCameraRightLookPos = vPos;
	}
	_float2 Get_CameraRightLookPos(void) {
		return m_vCameraRightLookPos;
	}

private:
	CVIBuffer_Point_Instance_Effect* m_pVIBufferCom = { nullptr };

private:
	_float2			m_vCameraRightLookPos = { 0.f, 0.f };

private:
	HRESULT Add_Components(void);
	virtual HRESULT SetUp_ShaderResources(void) override;
	virtual void Check_PassIndex(void) override;

public:
	static CEffect_Particle* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char* pComponentTag = nullptr);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free(void) override;
};

END