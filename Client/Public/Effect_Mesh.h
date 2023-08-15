#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "Effect.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)

class CEffect_Mesh final : public CEffect
{
private:
	CEffect_Mesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CEffect_Mesh(const CEffect_Mesh& rhs);
	virtual ~CEffect_Mesh() = default;

public:
	virtual HRESULT	Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render(void) override;

public:
	HRESULT Add_Component_Model(_uint iLevelIndex, const _tchar* pComponentTag);
	virtual void Clear(void);

private:
	class CModel* m_pModelCom = { nullptr };

private:
	HRESULT Add_Components(void);
	virtual HRESULT SetUp_ShaderResources(void) override;
	virtual void Check_PassIndex(void) override;

public:
	static CEffect_Mesh* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free(void) override;
};

END