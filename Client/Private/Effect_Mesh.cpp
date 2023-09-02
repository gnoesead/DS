#include "pch.h"
#include "..\Public\Effect_Mesh.h"

#include "GameInstance.h"
#include "ParticleSystem.h"

CEffect_Mesh::CEffect_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect(pDevice, pContext)
{
}

CEffect_Mesh::CEffect_Mesh(const CEffect_Mesh& rhs)
	: CEffect(rhs)
{
}

HRESULT CEffect_Mesh::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Mesh::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eEffectDesc.eRenderMode = MESH;
	m_eEffectDesc.eEffectType = EFFECT_MESH;

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

void CEffect_Mesh::Tick(_double _dTimeDelta)
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

void CEffect_Mesh::LateTick(_double _dTimeDelta)
{
	_double dTimeDelta = _dTimeDelta * m_dPlaySpeed;

	__super::LateTick(dTimeDelta);
}

HRESULT CEffect_Mesh::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (m_isRendering)
	{
		if (nullptr != m_pModelCom)
		{
			_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

			for (_uint i = 0; i < iNumMeshes; ++i)
			{
				if (nullptr != m_pTextures[TEX_DIFFUSE])
					m_pTextures[TEX_DIFFUSE]->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture");

				m_pShaderCom->Begin(m_iPassIndex);

				m_pModelCom->Render(i);
			}
		}
	}
	return S_OK;
}

HRESULT CEffect_Mesh::Add_Component_Model(_uint iLevelIndex, const _tchar* pComponentTag)
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(iLevelIndex, pComponentTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

void CEffect_Mesh::Clear(void)
{
	__super::Clear();

	Safe_Release(m_pModelCom);

	Safe_Release(m_pModelCom);

	if (Find_Component(TEXT("Com_Model")))
		m_Components.erase(TEXT("Com_Model"));
}

HRESULT CEffect_Mesh::Add_Components(void)
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_Shader_VtxModel_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Mesh::SetUp_ShaderResources(void)
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	if (nullptr != m_pTextures[TEX_NORMAL])
	{
		/* For.Com_Texture */
		if (FAILED(m_pTextures[TEX_NORMAL]->Bind_ShaderResourceView(m_pShaderCom, "g_NormalTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->SetUp_RawValue("g_vNormalPanningSpeed", (void*)&m_eEffectDesc.vNormalPanningSpeed, sizeof(_float4))))
			return E_FAIL;
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

void CEffect_Mesh::Check_PassIndex(void)
{
	if (nullptr != m_pTextures[TEX_DIFFUSE])
	{
		m_iPassIndex = 1;
		
		if (OPT_BY_RED == m_eEffectDesc.eTextureShaderOption[TEX_DIFFUSE])
		{
			m_iPassIndex = 2;

			if (OPT_NO_ZWRITE == m_eEffectDesc.eTextureShaderOption[TEX_MASK])
				m_iPassIndex = 17;
		}

		if (m_eEffectDesc.isTextureSheetAnimation)
			m_iPassIndex = 18;

		if (nullptr != m_pTextures[TEX_NOISE])
		{
			m_iPassIndex = 3;

			if (OPT_BY_RED == m_eEffectDesc.eTextureShaderOption[TEX_DIFFUSE])
			{
				if (OPT_NO_ZWRITE != m_eEffectDesc.eTextureShaderOption[TEX_MASK])
					m_iPassIndex = 15;
				else
				{
					m_iPassIndex = 16;

					if (m_eEffectDesc.isTextureSheetAnimation)
						m_iPassIndex = 21;
				}
			}
		}

		if (nullptr != m_pTextures[TEX_DISTORTION])
			m_iPassIndex = 5;
	}
	else if (nullptr != m_pTextures[TEX_MASK])
	{

		if (nullptr != m_pTextures[TEX_RAMP])
		{
			m_iPassIndex = 4;

			if (m_eEffectDesc.eTextureShaderOption[TEX_DIFFUSE] == OPT_NO_ZWRITE)
				m_iPassIndex = 14;

			if (m_eEffectDesc.isTextureSheetAnimation)
				m_iPassIndex = 20;
		}
		else
		{
			if (m_eEffectDesc.eTextureShaderOption[TEX_DIFFUSE] == OPT_NO_ZWRITE)
				m_iPassIndex = 11;
			else if (m_eEffectDesc.isTextureSheetAnimation)
				m_iPassIndex = 19;
			else
				m_iPassIndex = 6;
		}

		if (nullptr != m_pTextures[TEX_NOISE])
		{
			if (nullptr != m_pTextures[TEX_RAMP])
			{
				if (m_eEffectDesc.isTextureSheetAnimation)
					m_iPassIndex = 22;
				else
					m_iPassIndex = 13;
			}
			else
				m_iPassIndex = 7;
		}

		if (nullptr != m_pTextures[TEX_DISTORTION])
		{
			if (nullptr != m_pTextures[TEX_RAMP])
			{
				if (nullptr != m_pTextures[TEX_NOISE])
					m_iPassIndex = 9;
				else
					m_iPassIndex = 8;
			}
			else
				m_iPassIndex = 5;
		}
	}
	else
		m_iPassIndex = 0;
}

CEffect_Mesh* CEffect_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Mesh* pInstance = new CEffect_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Mesh::Clone(void* pArg)
{
	CEffect_Mesh* pInstance = new CEffect_Mesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Mesh::Free(void)
{
	__super::Free();

	Safe_Release(m_pModelCom);
}
