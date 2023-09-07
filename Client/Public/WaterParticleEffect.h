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

class CWaterParticleEffect final : public CEffectW
{
private:
	CWaterParticleEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWaterParticleEffect(const CWaterParticleEffect& rhs);
	virtual ~CWaterParticleEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CWaterParticleEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END