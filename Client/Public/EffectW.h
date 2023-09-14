#pragma once

#include "Client_Defines.h"
#include "MasterEffect.h"
#include "EffectW_Manager.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;

END

BEGIN(Client)

class CEffectW abstract : public CMasterEffect
{
public:
	typedef struct EffectWtag
	{
		_vector vPos;

		_float2 vStartSizeX = { 0.f, 0.f };
		_float2 vStartSizeY = { 0.f, 0.f };

		_float2 vSizeSpeedX = { 0.f, 0.f };
		_float2 vSizeSpeedY = { 0.f, 0.f };

		_float2 vFrameSpeed = { 0.f, 0.f };
		_float2 vSpeedX = { 0.f, 0.f };
		_float2 vSpeedY = { 0.f, 0.f };
		_float2 vSpeedZ = { 0.f, 0.f };

		_float2 vStartPosX = { 0.f, 0.f };
		_float2 vStartPosY = { 0.f, 0.f };
		_float2 vStartPosZ = { 0.f, 0.f };

		_float	fGravity = { 0.f };

		_float2	vStartFrame = {0.f ,0.f};

		_uint iNumX = { 0 };
		_uint iNumY = { 0 };

		_bool	bLoof = { false };

		_uint eEffectWType;

		_bool	bWorld = { false };

		_float	fAlpha = { 0.f };
		_float	fAlphaSpeed = { 0.f };

		_float2	vColor = { 0.f , 0.f };

		_bool	bSizeEqual = { false };

		_bool bSpecial = { false };

	}EFFECTWDESC;

protected:
	CEffectW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectW(const CEffectW& rhs);
	virtual ~CEffectW() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() = 0;

public:
	void	Update_Frame(_double TimeDelta);
	void	Reset_Data(void* pArg);

protected:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

protected:
	_double					m_FrameAccTime = { 0.0 };
	_uint					m_iFrame = { 0 };

	_float3					m_vSize = { 0.f ,0.f , 0.f };
	_double					m_dFrameSpeed = { 0.f };
	_double					m_dSpeedX = { 0.f };
	_double					m_dSpeedY = { 0.f };
	_double					m_dSpeedZ = { 0.f };
	
	_float					m_fSizeSpeedX = { 0.f };
	_float					m_fSizeSpeedY = { 0.f };

	_float					m_fPlusX = { 0.f };
	_float					m_fPlusY = { 0.f };
	_float					m_fPlusZ = { 0.f };

	EFFECTWDESC				m_EffectWDesc;

	_double					m_dLifeAccTime = { 0.0 };
	

protected:
	_bool					m_IsPlaying = { false };
	
	_float					m_fAlpha = { 0.f };
	_float					m_fAlphaSpeed = { 0.f };

	_float					m_fColor = { 0.f };

public:
	virtual void Free() override;
};

END