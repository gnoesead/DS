#pragma once

#include "Client_Defines.h"
#include "MasterEffect.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;

END

BEGIN(Client)

class CAurora final : public CMasterEffect
{
public:
	typedef struct Effecttag
	{
		CTransform* pTransform;
		CGameObject* pGameObject;

	}EFFECTDESC;

private:
	CAurora(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAurora(const CAurora& rhs);
	virtual ~CAurora() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

	void	Update_Frame(_double TimeDelta);

	void	Reset_Data();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	CTexture* m_pRampTextureCom = { nullptr };

private:
	_uint					m_iNumX = 6;
	_uint					m_iNumY = 6;

private:
	_double					m_FrameAccTime = { 0.0 };
	_uint					m_iFrame = { 0 };

	_float3					m_vSize = { 0.f ,0.f , 0.f };
	_double					m_dFrameSpeed = { 0.f };

	_double					m_dSpeedX = { 0.f };
	_double					m_dSpeedY = { 0.f };
	_double					m_dSpeedZ = { 0.f };

	_float					m_fPlusX = { 0.f };
	_float					m_fPlusY = { 0.f };
	_float					m_fPlusZ = { 0.f };

	_float					m_fSizeSpeedX = { 0.f };
	_float					m_fSizeSpeedY = { 0.f };

	EFFECTDESC				m_EffectDesc;

	_float					m_fAlpha = { 0.f };


	_float					m_fAccY = { 0.f };

	_float3					m_vColor = { 0.f , 0.f , 0.f };

public:
	static CAurora* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END