#pragma once
#include "Client_Defines.h"
#include "MasterEffect.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTexture;
class CTransform;
END

BEGIN(Client)

class CAlertMesh_Akaza final : public CMasterEffect
{
public:
	enum STATE { STATE_SHOWON, STATE_SHOWOFF };
	enum TYPE { TYPE_INNER_0, TYPE_INNER_1, TYPE_OUTWAVE };
	typedef struct tagEffectDesc
	{
		CTransform* pOwnerTransform = { nullptr };
		TYPE		eType = { TYPE_INNER_0 };
		_float3		vScale = { 1.f, 1.f, 1.f };
	}EFFECTDESC;
private:
	CAlertMesh_Akaza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAlertMesh_Akaza(const CAlertMesh_Akaza& rhs);
	virtual ~CAlertMesh_Akaza() = default;

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
	_float		m_fLandY = { 0.f };
	_float		m_fScale = { 9.f };

	STATE		m_eState = STATE_SHOWON;
	_double		m_dAccTime = { 0.0 };

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CTexture* m_pRampTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CAlertMesh_Akaza* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END