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

class CStoneParticle final : public CMasterEffect
{
public:
	typedef struct Effecttag
	{
		_vector vPos;

		_float2 vSizeX = { 0.f, 0.f };
		_float2 vSizeY = { 0.f, 0.f };

		_float2 vSizeSpeedX = { 0.f, 0.f };
		_float2 vSizeSpeedY = { 0.f, 0.f };

		_float2 vFrameSpeed = { 0.f, 0.f };
		_float2 vSpeedX = { 0.f, 0.f };
		_float2 vSpeedY = { 0.f, 0.f };
		_float2 vSpeedZ = { 0.f, 0.f };

		_float2 vStartPosX = { 0.f, 0.f };
		_float2 vStartPosY = { 0.f, 0.f };
		_float2 vStartPosZ = { 0.f, 0.f };

		_float fGravity = { 0.f };
	
	}EFFECTDESC;

private:
	CStoneParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStoneParticle(const CStoneParticle& rhs);
	virtual ~CStoneParticle() = default;

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
	_uint					m_iNumX =2;
	_uint					m_iNumY = 2;

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

	_float					m_fAlpha = { 1.f };
	_float					m_fAlphaDecreseSpeed = { 0.f };
	_float					m_fColor = { 0.f };

	_double					m_dLifeAccTime = { 0.0 };

public:
	static CStoneParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END