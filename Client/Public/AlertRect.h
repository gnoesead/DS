#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;

END

BEGIN(Client)

class CAlertRect final : public CGameObject
{
public:
	enum STATE { STATE_SHOWON, STATE_SHOWOFF };
	enum TYPE { TYPE_KICKDOWN, TYPE_ROOMCHANGE };
	typedef struct tagEffectDesc
	{
		CTransform* pOwnerTransform = { nullptr };
		_uint		iType = { 0 };
		
	}EFFECTDESC;

	

private:
	CAlertRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAlertRect(const CAlertRect& rhs);
	virtual ~CAlertRect() = default;

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
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float4x4				m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	EFFECTDESC				m_EffectDesc;

private:
	_float					m_fAlpha = 1.f;
	_float					m_fLandY = { 0.f };
	_float					m_fScale = { 9.f };

	_float4					m_vLook = { 0.f , 0.f , 0.f , 0.f };
	STATE					m_eState = STATE_SHOWON;

public:
	static CAlertRect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END