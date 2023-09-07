#pragma once

#include "Client_Defines.h"
#include "EffectW.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;

END

BEGIN(Client)

class CStoneParticle final : public CEffectW
{
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
	_float					m_fAlpha = { 1.f };
	_float					m_fAlphaDecreseSpeed = { 0.f };
	_float					m_fColor = { 0.f };

public:
	static CStoneParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END