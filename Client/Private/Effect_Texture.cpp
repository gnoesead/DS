#include "pch.h"
#include "..\Public\Effect_Texture.h"

#include "GameInstance.h"
#include "ParticleSystem.h"

CEffect_Texture::CEffect_Texture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect(pDevice, pContext)
{
}

CEffect_Texture::CEffect_Texture(const CEffect_Texture& rhs)
	: CEffect(rhs)
{

}

HRESULT CEffect_Texture::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Texture::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eEffectDesc.eEffectType = EFFECT_TEXTURE;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(m_pTransformCom->Get_State(CTransform::STATE_POSITION),
		XMMatrixTranslation(m_eEffectDesc.vPivot.x, m_eEffectDesc.vPivot.y, m_eEffectDesc.vPivot.z)));

	if (OP_CONSTANT == m_eEffectDesc.eStartLifeTimeOption)
		m_fLifeTime = m_eEffectDesc.fStartLifeTimeMin;
	else if (OP_RAND_TWO_CONSTANT == m_eEffectDesc.eStartLifeTimeOption)
	{
		_float fRandNum = Random::Generate_Float(m_eEffectDesc.fStartLifeTimeMin, m_eEffectDesc.fStartLifeTimeMax);
		m_fLifeTime = fRandNum;
	}

	if (m_eEffectDesc.isRandomStartDelay)
	{
		_float fRandNum = Random::Generate_Float(m_eEffectDesc.fStartDelayMin, m_eEffectDesc.fStartDelayMax);
		m_fStartDelay = fRandNum;
	}
	else
		m_fStartDelay = m_eEffectDesc.fStartDelayMin;

	Check_PassIndex();

	return S_OK;
}

void CEffect_Texture::Tick(_double _dTimeDelta)
{
	_double dTimeDelta = _dTimeDelta * m_dPlaySpeed;

	__super::Tick(dTimeDelta);

	if (m_ParentDesc.pParent->Get_isPlaying())
	{
		if (m_fDelayTimeAcc > m_fStartDelay)
		{
			if (m_eEffectDesc.isTextureSheetAnimation)
			{
				if (OPT_SPEED == m_eEffectDesc.eTimeModeOption)
				{
					m_fFrameTimeAcc += (float)dTimeDelta * m_fFrameSpeed;

					//if (m_eEffectDesc.fFrameSpeedMin != m_eEffectDesc.fFrameSpeedMax)
					//	m_fFrameSpeed = Random::Generate_Float(m_eEffectDesc.fFrameSpeedMin, m_eEffectDesc.fFrameSpeedMax);
					//else
					//	m_fFrameSpeed = m_eEffectDesc.fFrameSpeedMin;

					if (m_fFrameTimeAcc >= 1.f)
					{
						++m_fCurFrame;
						m_fFrameTimeAcc = 0.0;
						if (m_fCurFrame >= m_eEffectDesc.vTiles.x * m_eEffectDesc.vTiles.y)
							m_fCurFrame = 0;
					}
				}
				else if (OPT_LIFETIME == m_eEffectDesc.eTimeModeOption)
				{
					size_t iSize = m_FrameOverTime.size();

					if (0 < iSize)
					{
						if (m_iCurFrameIndex < iSize)
						{
							while (true)
							{
								if (m_iCurFrameIndex >= iSize - 1)
									m_iCurFrameIndex = (_uint)iSize - 2;

								if (m_fLifeTime >= m_FrameOverTime[m_iCurFrameIndex + 1].fLifetime)
									break;

								++m_iCurFrameIndex;

								if (m_iCurFrameIndex >= iSize - 1)
									m_iCurFrameIndex = (_uint)iSize - 2;
							}

							_float y = fabs(m_FrameOverTime[m_iCurFrameIndex + 1].fValue - m_FrameOverTime[m_iCurFrameIndex].fValue);
							_float fWeight = fabs(m_fLifeTime - m_FrameOverTime[m_iCurFrameIndex].fLifetime)
								/ fabs(m_FrameOverTime[m_iCurFrameIndex + 1].fLifetime - m_FrameOverTime[m_iCurFrameIndex].fLifetime);

							if (m_FrameOverTime[m_iCurFrameIndex + 1].fValue < m_FrameOverTime[m_iCurFrameIndex].fValue)
								y *= -1.f;

							m_fCurFrame = fWeight * y + m_FrameOverTime[m_iCurFrameIndex].fValue;
						}
					}
				}
			}
		}
	}
}

void CEffect_Texture::LateTick(_double _dTimeDelta)
{
	_double dTimeDelta = _dTimeDelta * m_dPlaySpeed;

	__super::LateTick(dTimeDelta);
}

HRESULT CEffect_Texture::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (m_isRendering)
	{
		if (FAILED(m_pShaderCom->Begin(m_iPassIndex)))
			return E_FAIL;

		if (BILLBOARD == m_eEffectDesc.eRenderMode)
		{
			if (FAILED(m_pVIBufferPointCom->Render()))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CEffect_Texture::Add_Components(void)
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	/* For.Com_VIBufferPoint */
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_VIBuffer_Point_Effect"),
		TEXT("Com_VIBufferPoint"), (CComponent**)&m_pVIBufferPointCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_Shader_VtxPoint_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Texture::SetUp_ShaderResources(void)
{
	if(FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	if (BILLBOARD == m_eEffectDesc.eRenderMode)
	{
		if (FAILED(m_pShaderCom->SetUp_RawValue("g_vSize", (void*)&m_vStartSize, sizeof(_float3))))
			return E_FAIL;
		
		if (FAILED(m_pShaderCom->SetUp_RawValue("g_fTextureOrder", (void*)&m_fTextureOrder, sizeof(float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->SetUp_RawValue("g_fCameraRightLookPos", (void*)&m_vCameraRightLookPos, sizeof(_float2))))
			return E_FAIL;

		if (0 != m_eEffectDesc.vStartRotationMin.x)
		{
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			_float4 vCameraPos = pGameInstance->Get_CameraPosition();

			Safe_Release(pGameInstance);
			
			_float3 vAxis = _float3(0.f, 1.f, 0.f);
			vAxis = Convert::ToFloat3(XMVector3TransformNormal(Convert::ToVector(vAxis), XMMatrixRotationAxis(Convert::ToVector(vCameraPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION), XMConvertToRadians(m_eEffectDesc.vStartRotationMin.x))));

			if (FAILED(m_pShaderCom->SetUp_RawValue("g_vAxis", (void*)&vAxis, sizeof(_float3))))
				return E_FAIL;
		}
	}

	if (m_eEffectDesc.isTextureSheetAnimation)
	{
		_float2 vTileSize = _float2(1.f / m_eEffectDesc.vTiles.x, 1.f / m_eEffectDesc.vTiles.y);

		if (FAILED(m_pShaderCom->SetUp_RawValue("g_vTileSize", (void*)&vTileSize, sizeof(_float2))))
			return E_FAIL;

		_float2 vCurTile = _float2(vTileSize.x * ((int)m_fCurFrame % (int)m_eEffectDesc.vTiles.x), vTileSize.y * ((int)m_fCurFrame / (int)m_eEffectDesc.vTiles.x));

		if (FAILED(m_pShaderCom->SetUp_RawValue("g_vCurTile", (void*)&vCurTile, sizeof(_float2))))
			return E_FAIL;
	}

	return S_OK;
}

void CEffect_Texture::Check_PassIndex(void)
{
	m_iPassIndex = 0;

	if (nullptr != m_pTextures[TEX_DIFFUSE])
	{
		if (nullptr != m_pTextures[TEX_NOISE])
			m_iPassIndex = 2;
		else
			m_iPassIndex = 1;

		if (nullptr != m_pTextures[TEX_DISTORTION])
			m_iPassIndex = 4;

		if (m_eEffectDesc.isTextureSheetAnimation)
			m_iPassIndex = 7;
	}
	else if (nullptr != m_pTextures[TEX_MASK])
	{
		m_iPassIndex = 6;

		if (nullptr != m_pTextures[TEX_RAMP])
		{
			m_iPassIndex = 3;

			if (m_eEffectDesc.isTextureSheetAnimation)
				m_iPassIndex = 9;
		}
		else
		{
			if (m_eEffectDesc.isTextureSheetAnimation)
				m_iPassIndex = 8;
		}

		if (nullptr != m_pTextures[TEX_DISTORTION])
			m_iPassIndex = 5;

	}
}

CEffect_Texture* CEffect_Texture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Texture* pInstance = new CEffect_Texture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect_Texture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Texture::Clone(void* pArg)
{
	CEffect_Texture* pInstance = new CEffect_Texture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect_Texture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Texture::Free(void)
{
	__super::Free();

	Safe_Release(m_pVIBufferPointCom);
	Safe_Release(m_pVIBufferRectCom);
}
