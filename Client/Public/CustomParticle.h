#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "VIBuffer_Custom_Instance.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CCustomParticle final : public CGameObject
{
public:
	enum CUSTOM_PARTICLE_PASS {
		PASS_NORMAL, PASS_RAMP, PASS_SPRITE, PASS_SPRITE_RAMP, PASS_END
	};
	typedef struct tagCustomParticleDesc
	{
		CUSTOM_PARTICLE_PASS		eShaderPass = { PASS_NORMAL };
		_float3		vPosition = { 0.f, 0.f, 0.f };
		_double		dSpriteSpeed = { 1.0 };
		_int2		vSpriteCount = { 1, 1 };
		_float		fLifeTime = { 1.f };
		_bool		bUseRamp = { false };
		_tchar		szTextureTag[MAX_PATH] = { TEXT("") };
		_tchar		szBufferTag[MAX_PATH] = { TEXT("") };
		_tchar		szRampTextureTag[MAX_PATH] = { TEXT("") };
		char		szPoolTag[MAX_PATH] = { ("") };
		CVIBuffer_CustomParticle::VIB_CUSTOMPARTDESC VIB_CustomPartDesc;
	}CUSTOMPARTDESC;
private:
	CCustomParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCustomParticle(const CCustomParticle& rhs);
	virtual ~CCustomParticle() = default;

public:
	void Reset_Particle(CUSTOMPARTDESC* pCustomPartDesc);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual void LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CUSTOMPARTDESC m_CustomPartDesc;
	_double		m_dTimeAcc = { 0.0 };
	_float		m_fScale = { 0.001f };
	_float		m_fAlpha = { 0.f };
	_float2		m_vTexCoord = { 0.f, 0.f };
	_double		m_dCycle = { 1.0 };

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CTexture*					m_pRampTextureCom = { nullptr };
	CRenderer*					m_pRendererCom = { nullptr };
	CTransform*					m_pTransformCom = { nullptr };
	CVIBuffer_CustomParticle*	m_pVIBufferCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CCustomParticle* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END