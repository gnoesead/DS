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

class CRoomSmoke final : public CGameObject
{
private:
	CRoomSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRoomSmoke(const CRoomSmoke& rhs);
	virtual ~CRoomSmoke() = default;

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
	_uint					m_iNumX = 6;
	_uint					m_iNumY = 6;

private:
	_double					m_FrameAccTime = { 0.0 };
	_uint					m_iFrame = { 0 };

	_float3					m_vSize = { 0.f ,0.f , 0.f };
	_double					m_dFrameSpeed = { 0.f };
	_double					m_dSpeedX = { 0.f };
	_double					m_dSpeedY = { 0.f };
	_float					m_fSizeSpeed = { 0.f };

	_float					m_fPlusX = { 0.f };
	_float					m_fPlusZ = { 0.f };
public:
	static CRoomSmoke* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END