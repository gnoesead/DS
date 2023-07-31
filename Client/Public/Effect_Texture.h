#pragma once

#include "Client_Defines.h"

#include "Effect.h"

BEGIN(Engine)
class CVIBuffer_Point_Effect;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Texture final : public CEffect
{
protected:
	CEffect_Texture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CEffect_Texture(const CEffect_Texture& rhs);
	virtual ~CEffect_Texture() = default;

public:
	virtual HRESULT	Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render(void) override;

public:
	CVIBuffer_Point_Effect* m_pVIBufferPointCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferRectCom = { nullptr };

protected:
	HRESULT Add_Components(void);
	HRESULT SetUp_ShaderResources(void);
	virtual void Check_PassIndex(void) override;

public:
	static CEffect_Texture* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free(void) override;
};

END