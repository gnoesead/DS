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

class CSwampWaterEffect final : public CMasterEffect
{
public:
	typedef struct Effecttag
	{
		_vector vPos;
		_float3 vScale;
		_double	dDelay = 0.0;

	}EFFECTDESC;

private:
	CSwampWaterEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSwampWaterEffect(const CSwampWaterEffect& rhs);
	virtual ~CSwampWaterEffect() = default;

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

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_double					m_FrameAccTime = { 0.0 };
	_uint					m_iFrame = { 0 };

	EFFECTDESC				m_EffectDesc;

	_double					m_dDelayAccTime = { 0.0 };
	_double					m_dFrameSpeed = { 0.0 };

	


public:
	static CSwampWaterEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END