#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CModel;
class CRenderer;
class CTransform;

END

BEGIN(Client)

class CAlertRect final : public CGameObject
{
public:
	enum  STATE{ STATE_SHOWON , STATE_WAIT,STATE_SHOWOFF};

	typedef struct tagEffectDesc
	{
		_vector		vLook;
		_vector		vPos;
		_bool		bLiarColor = false;
		
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
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	EFFECTDESC				m_EffectDesc;

private:
	_float					m_fAlpha = 0.f;
	_float					m_fLandY = { 0.f };
	_float					m_fScale = { 9.f };

	_float4					m_vLook = { 0.f , 0.f , 0.f , 0.f };
	_float					m_fDiffuseRatio = { 1.5f };

	STATE					m_eSTATE = { STATE_SHOWON};
	_double					m_dWaitAccTime = { 0.f };

public:
	static CAlertRect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END