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

class CTrainSmoke final : public CMasterEffect
{
private:
	CTrainSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrainSmoke(const CTrainSmoke& rhs);
	virtual ~CTrainSmoke() = default;

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
	_uint					m_iNumX = 4;
	_uint					m_iNumY = 4;

private:
	_double					m_FrameAccTime = { 0.0 };
	_uint					m_iFrame = { 0 };

	_float					m_fSize = { 0.f };
	_double					m_dFrameSpeed = { 0.f };
	_double					m_dSpeedX = { 0.f };
	_double					m_dSpeedY = { 0.f };
	_float					m_fSizeSpeed = { 0.f };

	_float					m_fPlusZ = { 0.f };
public:
	static CTrainSmoke* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END