#pragma once
#include "Client_Defines.h"
#include "MasterEffect.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
class CBone;
END

BEGIN(Client)

class CHandAura_Akaza final : public CMasterEffect
{
public:
	typedef struct tagEffectDesc
	{
		CTransform* pOwnerTransform = { nullptr };
		CBone*		pBone = { nullptr };
		_float3		vScale = { 1.f, 1.f, 1.f };
		_float3		vPos = { 0.f, 0.f, 0.f };
		_float2		vCustomUV = { 0.f, 0.f };
		_float2		vTime = { 1.f, 1.f };
	}EFFECTDESC;

private:
	CHandAura_Akaza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHandAura_Akaza(const CHandAura_Akaza& rhs);
	virtual ~CHandAura_Akaza() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	EFFECTDESC	m_EffectDesc;

	_float		m_fAlpha = { 0.1f };
	_float		m_fScale = { 1.f };

	_double		m_dAccTime = { 0.0 };

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CHandAura_Akaza* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END