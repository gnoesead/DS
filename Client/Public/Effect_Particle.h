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

private:
	CVIBuffer_Point_Instance_Effect* m_pVIBufferCom = { nullptr };

private:
	HRESULT Add_Components(void);
	HRESULT SetUp_ShaderResources(void);
	virtual void Check_PassIndex(void) override;

public:
	static CEffect_Particle* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free(void) override;
};

END