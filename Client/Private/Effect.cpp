#include "pch.h"
#include "..\Public\Effect.h"

#include "GameInstance.h"
#include "ParticleSystem.h"

CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CEffect::CEffect(const CEffect& rhs)
	: CGameObject(rhs)
{

}

HRESULT CEffect::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_eEffectDesc, pArg, sizeof EFFECTDESC);


	//m_pTransformCom->Scaling(1.f);

	return S_OK;
}

void CEffect::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	m_pTransformCom->Scaling(m_eEffectDesc.vShapeScale);
	m_pTransformCom->Rotation(m_eEffectDesc.vShapeRotation);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_eEffectDesc.vPivot.x, m_eEffectDesc.vPivot.y, m_eEffectDesc.vPivot.z, 1.f));

	if (m_ParentDesc.pParent->Get_isPlaying())
	{
		m_eEffectDesc.fTimeAcc += (_float)dTimeDelta;

		m_fLifeTime -= (_float)dTimeDelta;

		if (0 >= m_fLifeTime)
		{
			if (m_eEffectDesc.isLooping)
			{
				if (OP_CONSTANT == m_eEffectDesc.eStartLifeTimeOption)
					m_fLifeTime = m_eEffectDesc.fStartLifeTimeMin;
				else if (OP_RAND_TWO_CONSTANT == m_eEffectDesc.eStartLifeTimeOption)
				{
					_float fRandNum = Random::Generate_Float(m_eEffectDesc.fStartLifeTimeMin, m_eEffectDesc.fStartLifeTimeMax);
					m_fLifeTime = fRandNum;
				}
			}
			else
				m_isDead = true;
		}

		if (OP_CURVE == m_eEffectDesc.eSizeOverLifetimeOption)
		{
			size_t iSize = m_SizeOverLifeTimes[0].size();

			if (0 < iSize)
			{
				if (!m_eEffectDesc.isSeparateAxesSzOverLifeTime)
				{
					if (m_iCurSizeIndex < iSize)
					{
						while (true)
						{
							if (m_fLifeTime >= m_SizeOverLifeTimes[0][m_iCurSizeIndex + 1].fLifetime)
								break;

							++m_iCurSizeIndex;
						}

						_float y = fabs(m_SizeOverLifeTimes[0][m_iCurSizeIndex + 1].fValue - m_SizeOverLifeTimes[0][m_iCurSizeIndex].fValue);
						_float fWeight = fabs(m_fLifeTime - m_SizeOverLifeTimes[0][m_iCurSizeIndex].fLifetime)
							/ fabs(m_SizeOverLifeTimes[0][m_iCurSizeIndex + 1].fLifetime - m_SizeOverLifeTimes[0][m_iCurSizeIndex].fLifetime);

						if (m_SizeOverLifeTimes[0][m_iCurSizeIndex + 1].fValue < m_SizeOverLifeTimes[0][m_iCurSizeIndex].fValue)
							y *= -1;

						_float	fCurSize = fWeight * y + m_SizeOverLifeTimes[0][m_iCurSizeIndex].fValue;

						m_pTransformCom->Scaling(fCurSize);
					}
				}
			}
		}

	}
	else if ((m_ParentDesc.pParent->Get_isStopped()))
	{
		m_eEffectDesc.fTimeAcc = 0;
	}
}

void CEffect::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	//CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//CTransform* pTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), TEXT("Com_Transform")));
	//
	//if (pTransform != nullptr)	
	//	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * pTransform->Get_WorldMatrix());
	
	
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_ParentDesc.pParent->Get_WorldMatrix());
	//XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_ParentDesc.pParentTransform->Get_WorldMatrix());

	Check_PassIndex();

	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT, this)))
		return;
}

HRESULT CEffect::Render(void)
{
	if (m_isRendering)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CEffect::Add_Component_Texture(_uint iLevelIndex, const _tchar* pComponentTag, int eType)
{
	switch (eType)
	{
	case 0:		// TEX_DIFFUSE
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(iLevelIndex, pComponentTag,
			TEXT("Com_Diffuse"), (CComponent**)&m_pTextures[eType])))
			return E_FAIL;
		break;
	case 1:		// TEX_MASK
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(iLevelIndex, pComponentTag,
			TEXT("Com_MaskTexure"), (CComponent**)&m_pTextures[eType])))
			return E_FAIL;
		break;
	case 2:		// TEX_RAMP
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(iLevelIndex, pComponentTag,
			TEXT("Com_RampTexure"), (CComponent**)&m_pTextures[eType])))
			return E_FAIL;
		break;
	case 3:		// TEX_NOISE
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(iLevelIndex, pComponentTag,
			TEXT("Com_NoiseTexure1"), (CComponent**)&m_pTextures[eType])))
			return E_FAIL;
		break;
	case 4:		// TEX_NOISE2
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(iLevelIndex, pComponentTag,
			TEXT("Com_NoiseTexure2"), (CComponent**)&m_pTextures[eType])))
			return E_FAIL;
		break;
	case 5:		// TEX_DISTORTION
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(iLevelIndex, pComponentTag,
			TEXT("Com_DistortionTexure"), (CComponent**)&m_pTextures[eType])))
			return E_FAIL;
		break;
	case 6:		// TEX_NORMAL
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(iLevelIndex, pComponentTag,
			TEXT("Com_NormalTexure"), (CComponent**)&m_pTextures[eType])))
			return E_FAIL;
		break;
	}

	return S_OK;
}

HRESULT CEffect::Add_Components(void)
{
	CTransform::TRANSFORMDESC	Desc;
	ZeroMemory(&Desc, sizeof Desc);

	if (OP_CONSTANT == m_eEffectDesc.eStartSpeedOption)
		Desc.dSpeedPerSec = m_eEffectDesc.fStartSpeedMin;

	Desc.dSpeedPerSec *= m_eEffectDesc.fSimulationSpeed;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &Desc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect::SetUp_ShaderResources(void)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_float4 vCameraPos = pGameInstance->Get_CameraPosition();
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_vCamPosition", &vCameraPos, sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (nullptr != m_pTextures[TEX_DIFFUSE])
	{
		/* For.Com_Texture */
		if (FAILED(m_pTextures[TEX_DIFFUSE]->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->SetUp_RawValue("g_vPanningSpeed", (void*)&m_eEffectDesc.vPanningSpeed, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->SetUp_RawValue("g_vFlip", (void*)&m_eEffectDesc.vFlip, sizeof(_float2))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->SetUp_RawValue("g_vColor", (void*)&m_eEffectDesc.vColor, sizeof(_float4))))
			return E_FAIL;
	}

	if (nullptr != m_pTextures[TEX_MASK])
	{
		/* For.Com_Texture */
		if (FAILED(m_pTextures[TEX_MASK]->Bind_ShaderResourceView(m_pShaderCom, "g_MaskTexture", 0)))
			return E_FAIL;
	}

	if (nullptr != m_pTextures[TEX_DISTORTION])
	{
		/* For.Com_Texture */
		if (FAILED(m_pTextures[TEX_DISTORTION]->Bind_ShaderResourceView(m_pShaderCom, "g_DistortionTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->SetUp_RawValue("g_fDistortionStrength", (void*)&m_eEffectDesc.fDistortionStrength, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->SetUp_RawValue("g_fDistortionSpeed", (void*)&m_eEffectDesc.fDistortionSpeed, sizeof(_float))))
			return E_FAIL;
	}

	if (nullptr != m_pTextures[TEX_RAMP])
	{
		/* For.Com_Texture */
		if (FAILED(m_pTextures[TEX_RAMP]->Bind_ShaderResourceView(m_pShaderCom, "g_RampTexture", 0)))
			return E_FAIL;
	}

	if (nullptr != m_pTextures[TEX_NOISE])
	{
		/* For.Com_Texture */
		if (FAILED(m_pTextures[TEX_NOISE]->Bind_ShaderResourceView(m_pShaderCom, "g_NoiseTexture1", 0)))
			return E_FAIL;

		//if (FAILED(m_pShaderCom->SetUp_RawValue("g_vPanningSpeed", (void*)&m_eEffectDesc.vPanningSpeed, sizeof(_float4))))
		//	return E_FAIL;

		//if (FAILED(m_pShaderCom->SetUp_RawValue("g_vFlip", (void*)&m_eEffectDesc.vFlip, sizeof(_float2))))
		//	return E_FAIL;

		if (FAILED(m_pShaderCom->SetUp_RawValue("g_fDissolveAmount", (void*)&m_eEffectDesc.fCutOff, sizeof(_float))))
			return E_FAIL;

		//if (FAILED(m_pShaderCom->SetUp_RawValue("g_fDissolveSoftness", (void*)&m_eEffectDesc.fCutOffSoftness, sizeof(_float))))
		//	return E_FAIL;
	}

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fTimeAcc", (void*)&m_eEffectDesc.fTimeAcc, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

//CEffect* CEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
//{
//	CEffect* pInstance = new CEffect(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize_Prototype()))
//	{
//		MSG_BOX("Failed to Create : CEffect");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//CGameObject* CEffect::Clone(void* pArg)
//{
//	CEffect* pInstance = new CEffect(*this);
//
//	if (FAILED(pInstance->Initialize(pArg)))
//	{
//		MSG_BOX("Failed to Clone : CEffect");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}

void CEffect::Free(void)
{
	__super::Free();

	for (auto Texture : m_pTextures)
		Safe_Release(Texture);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
}
