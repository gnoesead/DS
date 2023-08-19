#include "pch.h"
#include "..\Public\Effect.h"

#include "GameInstance.h"
#include "ParticleSystem.h"
#include "EffectPlayer.h"

CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMasterEffect(pDevice, pContext)
{
}

CEffect::CEffect(const CEffect& rhs)
	: CMasterEffect(rhs)
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

	m_fTextureOrder = m_eEffectDesc.fTextureOrder;

	return S_OK;
}

void CEffect::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if(!m_eEffectDesc.is3DStartSize)
		m_pTransformCom->Scaling(m_eEffectDesc.vShapeScale);
	else
		m_pTransformCom->Scaling(_float3(m_eEffectDesc.vStartSizeMin.x, m_eEffectDesc.vStartSizeMin.y, m_eEffectDesc.vStartSizeMin.z));
	m_pTransformCom->Rotation(m_eEffectDesc.vShapeRotation);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_eEffectDesc.vPivot.x, m_eEffectDesc.vPivot.y, m_eEffectDesc.vPivot.z, 1.f));

	if (m_ParentDesc.pParent->Get_isPlaying())
	{
		m_fDelayTimeAcc += (_float)dTimeDelta;

		m_fLifeTime -= (_float)dTimeDelta;

		if (m_fDelayTimeAcc > m_eEffectDesc.fPixelDiscardDelay)
			m_fDiscardTimeAcc += (_float)dTimeDelta;

		if (m_fDelayTimeAcc > m_eEffectDesc.fDissolveDelay)
			m_fDissolveTimeAcc += (_float)dTimeDelta;

		if (m_fDelayTimeAcc > m_eEffectDesc.vPaddingDelayStartEnd.x)
			m_fPaddingTimeStartAcc += (_float)dTimeDelta;

		if (m_fDelayTimeAcc > m_eEffectDesc.vPaddingDelayStartEnd.y)
			m_fPaddingTimeEndAcc += (_float)dTimeDelta;

		if (m_fDelayTimeAcc > m_fStartDelay)
		{
			m_eEffectDesc.fTimeAcc += (_float)dTimeDelta;

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
				{
					switch (m_eEffectDesc.eEffectType)
					{
					case 0:
						CEffectPlayer::Get_Instance()->Collect_EffectTexture(this);
						break;
					case 1:
						CEffectPlayer::Get_Instance()->Collect_EffectMesh(this);
						break;
					case 2:
						CEffectPlayer::Get_Instance()->Collect_EffectParticle(this);
						break;
					}
					m_isCollect = true;
					m_ParentDesc.pParent->Set_isCollect(true);

					m_eEffectDesc.fTimeAcc = 0;
					m_fLifeTime = m_eEffectDesc.fStartLifeTimeMin;
				}
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

							if(!m_eEffectDesc.is3DStartSize)
								m_pTransformCom->Scaling(fCurSize);
							else
								m_pTransformCom->Scaling(_float3(m_eEffectDesc.vStartSizeMin.x * fCurSize, m_eEffectDesc.vStartSizeMin.y * fCurSize, m_eEffectDesc.vStartSizeMin.z * fCurSize));
						}
					}
				}
			}

			if (OP_CURVE == m_eEffectDesc.eRotOverLifetimeOption)
			{
				if (m_eEffectDesc.isSeparateAxesRotOverLifeTime)
				{
					_float3	vCurRot = { 0.f, 0.f, 0.f };

					for (int i = 0; i < 3; ++i)
					{
						size_t iSize = m_RotOverLifeTimes[i].size();

						if (0 < iSize)
						{
							if (m_iCurRotIndex[i] < iSize)
							{
								while (true)
								{
									if (m_fLifeTime >= m_RotOverLifeTimes[i][m_iCurRotIndex[i] + 1].fLifetime)
										break;

									++m_iCurRotIndex[i];
								}

								_float y = fabs(m_RotOverLifeTimes[i][m_iCurRotIndex[i] + 1].fValue - m_RotOverLifeTimes[i][m_iCurRotIndex[i]].fValue);
								_float fWeight = fabs(m_fLifeTime - m_RotOverLifeTimes[i][m_iCurRotIndex[i]].fLifetime)
									/ fabs(m_RotOverLifeTimes[i][m_iCurRotIndex[i] + 1].fLifetime - m_RotOverLifeTimes[i][m_iCurRotIndex[i]].fLifetime);

								if (m_RotOverLifeTimes[i][m_iCurRotIndex[i] + 1].fValue < m_RotOverLifeTimes[i][m_iCurRotIndex[i]].fValue)
									y *= -1;

								if (0 == i)
									vCurRot.x = fWeight * y + m_RotOverLifeTimes[i][m_iCurRotIndex[i]].fValue;
								else if (1 == i)
									vCurRot.y = fWeight * y + m_RotOverLifeTimes[i][m_iCurRotIndex[i]].fValue;
								else if (2 == i)
									vCurRot.z = fWeight * y + m_RotOverLifeTimes[i][m_iCurRotIndex[i]].fValue;
							}
							m_pTransformCom->Rotation(vCurRot);
						}
					}
				}
			}
		
			if (OP_CURVE == m_eEffectDesc.ePosOverLifetimeOption)
			{
				for (int i = 0; i < 3; ++i)
				{
					size_t iSize = m_PosOverLifeTimes[i].size();

					if (0 < iSize)
					{
						if (m_iCurPosIndex[i] < iSize)
						{
							while (true)
							{
								if (m_iCurPosIndex[i] >= (_uint)iSize - 1)
									m_iCurPosIndex[i] = (_uint)iSize - 2;

								if (m_fLifeTime >= m_PosOverLifeTimes[i][m_iCurPosIndex[i] + 1].fLifetime)
									break;

								++m_iCurPosIndex[i];

								if (m_iCurPosIndex[i] >= (_uint)iSize - 1)
									m_iCurPosIndex[i] = (_uint)iSize - 2;
							}

							_float y = fabs(m_PosOverLifeTimes[i][m_iCurPosIndex[i] + 1].fValue - m_PosOverLifeTimes[i][m_iCurPosIndex[i]].fValue);
							_float fWeight = fabs(m_fLifeTime - m_PosOverLifeTimes[i][m_iCurPosIndex[i]].fLifetime)
								/ fabs(m_PosOverLifeTimes[i][m_iCurPosIndex[i] + 1].fLifetime - m_PosOverLifeTimes[i][m_iCurPosIndex[i]].fLifetime);

							if (m_PosOverLifeTimes[i][m_iCurPosIndex[i] + 1].fValue < m_PosOverLifeTimes[i][m_iCurPosIndex[i]].fValue)
								y *= -1;

							if (0 == i)
								m_vCurPos.x = fWeight * y + m_PosOverLifeTimes[i][m_iCurPosIndex[i]].fValue;
							else if (1 == i)
								m_vCurPos.y = fWeight * y + m_PosOverLifeTimes[i][m_iCurPosIndex[i]].fValue;
							else if (2 == i)
								m_vCurPos.z = fWeight * y + m_PosOverLifeTimes[i][m_iCurPosIndex[i]].fValue;
						}

					}
				}
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, Convert::ToVector_W1(m_vCurPos));
			}

			size_t iSizeAlpha = m_AlphaOverLifetimes.size();
			if (0 < iSizeAlpha)
			{
				if (m_iCurAlphaIndex < iSizeAlpha)
				{
					while (true)
					{
						if (m_fLifeTime >= m_AlphaOverLifetimes[m_iCurAlphaIndex + 1].fLifetime)
							break;

						++m_iCurAlphaIndex;
					}

					_float y = fabs(m_AlphaOverLifetimes[m_iCurAlphaIndex + 1].fValue - m_AlphaOverLifetimes[m_iCurAlphaIndex].fValue);
					_float fWeight = fabs(m_fLifeTime - m_AlphaOverLifetimes[m_iCurAlphaIndex].fLifetime)
						/ fabs(m_AlphaOverLifetimes[m_iCurAlphaIndex + 1].fLifetime - m_AlphaOverLifetimes[m_iCurAlphaIndex].fLifetime);

					if (m_AlphaOverLifetimes[m_iCurAlphaIndex + 1].fValue < m_AlphaOverLifetimes[m_iCurAlphaIndex].fValue)
						y *= -1;

					m_fAlpha = fWeight * y + m_AlphaOverLifetimes[m_iCurAlphaIndex].fValue;

					if (m_fAlpha > 1)
						m_fAlpha = 1;
					else if (m_fAlpha < 0)
						m_fAlpha = 0;
				}
			}
		}
	
		m_vPaddingStart = _float2(0.f, 0.f);

		if (m_eEffectDesc.vPaddingSpeedStart.x != 0)
		{
			int iDirection = m_eEffectDesc.vPaddingSpeedStart.x < 0 ? -1 : 1;

			if (iDirection == 1)
				m_vPaddingStart.x = (m_eEffectDesc.vDiscardedPixelMax.x - m_eEffectDesc.vPixelDiscardSpeedMin.x) - m_fPaddingTimeStartAcc * m_eEffectDesc.vPaddingSpeedStart.x;
			else
				m_vPaddingStart.x = (m_eEffectDesc.vDiscardedPixelMax.x - m_eEffectDesc.vPixelDiscardSpeedMin.x) + m_fPaddingTimeStartAcc * m_eEffectDesc.vPaddingSpeedStart.x;

			if (m_vPaddingStart.x < 0)
				m_vPaddingStart.x = 0;

			m_vPaddingStart.x *= iDirection;
		}

		if (m_eEffectDesc.vPaddingSpeedStart.y != 0)
		{
			int iDirection = m_eEffectDesc.vPaddingSpeedStart.y < 0 ? -1 : 1;

			if (iDirection == 1)
				m_vPaddingStart.y = (m_eEffectDesc.vDiscardedPixelMax.y - m_eEffectDesc.vPixelDiscardSpeedMin.y) - m_fPaddingTimeStartAcc * m_eEffectDesc.vPaddingSpeedStart.y;
			else
				m_vPaddingStart.y = (m_eEffectDesc.vDiscardedPixelMax.y - m_eEffectDesc.vPixelDiscardSpeedMin.y) + m_fPaddingTimeStartAcc * m_eEffectDesc.vPaddingSpeedStart.y;

			if (m_vPaddingStart.y < 0)
				m_vPaddingStart.y = 0;

			m_vPaddingStart.y *= iDirection;
		}

		m_vPaddingEnd = _float2(0.f, 0.f);

		if (m_vPaddingStart.x == 0)
		{
			if (m_eEffectDesc.vPaddingSpeedEnd.x != 0)
			{
				int iDirection = m_eEffectDesc.vPaddingSpeedEnd.x < 0 ? -1 : 1;

				if (iDirection == 1)
					m_vPaddingEnd.x = m_fPaddingTimeEndAcc * m_eEffectDesc.vPaddingSpeedEnd.x;
				else
					m_vPaddingEnd.x = m_fPaddingTimeEndAcc * m_eEffectDesc.vPaddingSpeedEnd.x * -1;

				if (m_vPaddingEnd.x > m_eEffectDesc.vDiscardedPixelMax.x - m_eEffectDesc.vPixelDiscardSpeedMin.x)
					m_vPaddingEnd.x = m_eEffectDesc.vDiscardedPixelMax.x - m_eEffectDesc.vPixelDiscardSpeedMin.x;

				m_vPaddingEnd.x *= iDirection;
			}
		}

		if (m_vPaddingStart.y == 0)
		{
			if (m_eEffectDesc.vPaddingSpeedEnd.y != 0)
			{
				int iDirection = m_eEffectDesc.vPaddingSpeedEnd.y < 0 ? -1 : 1;

				if (iDirection == 1)
					m_vPaddingEnd.y = m_fPaddingTimeEndAcc * m_eEffectDesc.vPaddingSpeedEnd.y;
				else
					m_vPaddingEnd.y = m_fPaddingTimeEndAcc * m_eEffectDesc.vPaddingSpeedEnd.y * -1;

				if (m_vPaddingEnd.y > m_eEffectDesc.vDiscardedPixelMax.y - m_eEffectDesc.vPixelDiscardSpeedMin.y)
					m_vPaddingEnd.y = m_eEffectDesc.vDiscardedPixelMax.y - m_eEffectDesc.vPixelDiscardSpeedMin.y;

				m_vPaddingEnd.y *= iDirection;
			}
		}
	}
	else if ((m_ParentDesc.pParent->Get_isStopped()))
	{
		//m_eEffectDesc.fTimeAcc = 0.f;
		//m_fDelayTimeAcc = 0.f;
	}
}

void CEffect::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_ParentDesc.pParent->Get_WorldMatrix());

	Check_PassIndex();

	if (m_fDelayTimeAcc > m_fStartDelay)	// 이거 확인
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this)))
			return;
	}
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

void CEffect::Clear(void)
{
	for (int i = 0; i < TEX_END; ++i)
		Safe_Release(m_pTextures[i]);

	for (int i = 0; i < TEX_END; ++i)
		Safe_Release(m_pTextures[i]);

	if (Find_Component(TEXT("Com_Diffuse")))
		m_Components.erase(TEXT("Com_Diffuse"));

	if (Find_Component(TEXT("Com_MaskTexure")))
		m_Components.erase(TEXT("Com_MaskTexure"));

	if (Find_Component(TEXT("Com_RampTexure")))
		m_Components.erase(TEXT("Com_RampTexure"));

	if (Find_Component(TEXT("Com_NoiseTexure1")))
		m_Components.erase(TEXT("Com_NoiseTexure1"));

	if (Find_Component(TEXT("Com_NoiseTexure2")))
		m_Components.erase(TEXT("Com_NoiseTexure2"));

	if (Find_Component(TEXT("Com_DistortionTexure")))
		m_Components.erase(TEXT("Com_DistortionTexure"));

	if (Find_Component(TEXT("Com_NormalTexure")))
		m_Components.erase(TEXT("Com_NormalTexure"));
}

void CEffect::Set_Initial_Data(void)
{
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVector3TransformCoord(m_pTransformCom->Get_State(CTransform::STATE_POSITION),
		XMMatrixTranslation(m_eEffectDesc.vPivot.x, m_eEffectDesc.vPivot.y, m_eEffectDesc.vPivot.z)));

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_ParentDesc.pParentTransform->Get_WorldMatrix());

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

	if (m_AlphaOverLifetimes.size() > 0)
		m_fAlpha = m_AlphaOverLifetimes[0].fValue;
	else
		m_fAlpha = 1.f;

	m_fTextureOrder = m_eEffectDesc.fTextureOrder;

	m_vCurPos = _float3(0.f, 0.f, 0.f);
}

void CEffect::Reset_Data(void)
{
	m_iCurSizeIndex = 0;

	for (int i = 0; i < 3; ++i)
		m_iCurRotIndex[i] = 0;

	for (int i = 0; i < 3; ++i)
		m_iCurPosIndex[i] = 0;

	m_iCurAlphaIndex = 0;

	m_eEffectDesc.fTimeAcc = 0.f;
	m_fDelayTimeAcc = 0.f;
	m_fDiscardTimeAcc = 0.f;
	m_fDissolveTimeAcc = 0.f;
	m_fPaddingTimeStartAcc = 0.f;
	m_fPaddingTimeEndAcc = 0.f;

	m_BurstList.clear();

	for(int i = 0; i < 3; ++i)
		m_SizeOverLifeTimes[i].clear();
	for (int i = 0; i < 3; ++i)
		m_RotOverLifeTimes[i].clear();
	for (int i = 0; i < 3; ++i)
		m_PosOverLifeTimes[i].clear();

	m_AlphaOverLifetimes.clear();

	m_fDissolveAmount = 0.f;
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
	//Safe_AddRef(pGameInstance);

	_float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_float4 vCameraPos = pGameInstance->Get_CameraPosition();
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_vCamPosition", &vCameraPos, sizeof(_float4))))
		return E_FAIL;

	//Safe_Release(pGameInstance);

	if (nullptr != m_pTextures[TEX_DIFFUSE])
	{
		/* For.Com_Texture */
		if (FAILED(m_pTextures[TEX_DIFFUSE]->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", 0)))
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

		if (FAILED(m_pShaderCom->SetUp_RawValue("g_vGradientOffset", (void*)&m_eEffectDesc.vGradientOffset, sizeof(_float2))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->SetUp_RawValue("g_vGradientTilling", (void*)&m_eEffectDesc.vGradientTilling, sizeof(_float2))))
			return E_FAIL;
	}

	if (nullptr != m_pTextures[TEX_NORMAL])
	{
		/* For.Com_Texture */
		if (FAILED(m_pTextures[TEX_NORMAL]->Bind_ShaderResourceView(m_pShaderCom, "g_NormalTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->SetUp_RawValue("g_vNormalPanningSpeed", (void*)&m_eEffectDesc.vNormalPanningSpeed, sizeof(_float4))))
			return E_FAIL;
	}

	if (nullptr != m_pTextures[TEX_NOISE])
	{
		/* For.Com_Texture */
		if (FAILED(m_pTextures[TEX_NOISE]->Bind_ShaderResourceView(m_pShaderCom, "g_NoiseTexture1", 0)))
			return E_FAIL;

		if (m_fDissolveTimeAcc != 0)
		{
			float fDissolveAmount = m_fDissolveTimeAcc * m_eEffectDesc.fDissolveSpeed;
			if (FAILED(m_pShaderCom->SetUp_RawValue("g_fDissolveAmount", (void*)&fDissolveAmount, sizeof(_float))))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->SetUp_RawValue("g_fDissolveAmount", (void*)&m_eEffectDesc.fCutOff, sizeof(_float))))
				return E_FAIL;
		}
	}

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fTimeAcc", (void*)&m_eEffectDesc.fTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fAlpha", (void*)&m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_vPanningSpeed", (void*)&m_eEffectDesc.vPanningSpeed, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_vFlip", (void*)&m_eEffectDesc.vFlip, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_vDiscardedPixelMin", (void*)&m_eEffectDesc.vDiscardedPixelMin, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_vDiscardedPixelMax", (void*)&m_eEffectDesc.vDiscardedPixelMax, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_vPixelDiscardSpeedMin", (void*)&m_eEffectDesc.vPixelDiscardSpeedMin, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_vPixelDiscardSpeedMax", (void*)&m_eEffectDesc.vPixelDiscardSpeedMax, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fDiscardTimeAcc", (void*)&m_fDiscardTimeAcc, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_vOffset", (void*)&m_eEffectDesc.vDiffuseOffset, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_vTilling", (void*)&m_eEffectDesc.vDiffuseTilling, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_vPaddingStart", (void*)&m_vPaddingStart, sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_RawValue("g_vPaddingEnd", (void*)&m_vPaddingEnd, sizeof(_float2))))
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

	for (int i = 0; i < TEX_END; ++i)
		Safe_Release(m_pTextures[i]);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
}
