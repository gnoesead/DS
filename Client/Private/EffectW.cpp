#include "pch.h"
#include "..\Public\EffectW.h"

#include "GameInstance.h"
#include "EffectW_Manager.h"

CEffectW::CEffectW(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMasterEffect(pDevice, pContext)
{

}

CEffectW::CEffectW(const CEffectW& rhs)
	: CMasterEffect(rhs)
{

}

HRESULT CEffectW::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectW::Initialize(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectWDesc, pArg, sizeof m_EffectWDesc);

	m_fPlusX = Random::Generate_Float(m_EffectWDesc.vStartPosX.x, m_EffectWDesc.vStartPosX.y);
	m_fPlusY = Random::Generate_Float(m_EffectWDesc.vStartPosY.x, m_EffectWDesc.vStartPosY.y);
	m_fPlusZ = Random::Generate_Float(m_EffectWDesc.vStartPosZ.x, m_EffectWDesc.vStartPosZ.y);

	m_dFrameSpeed = (_double)Random::Generate_Float(m_EffectWDesc.vFrameSpeed.x, m_EffectWDesc.vFrameSpeed.y);

	m_dSpeedX = (_double)Random::Generate_Float(m_EffectWDesc.vSpeedX.x, m_EffectWDesc.vSpeedX.y);
	m_dSpeedY = (_double)Random::Generate_Float(m_EffectWDesc.vSpeedY.x, m_EffectWDesc.vSpeedY.y);
	m_dSpeedZ = (_double)Random::Generate_Float(m_EffectWDesc.vSpeedZ.x, m_EffectWDesc.vSpeedZ.y);

	m_vSize = { Random::Generate_Float(m_EffectWDesc.vStartSizeX.x,m_EffectWDesc.vStartSizeX.y) , Random::Generate_Float(m_EffectWDesc.vStartSizeY.x, m_EffectWDesc.vStartSizeY.y),1.f };

	m_fSizeSpeedX = Random::Generate_Float(m_EffectWDesc.vSizeSpeedX.x, m_EffectWDesc.vSizeSpeedX.y);
	m_fSizeSpeedY = Random::Generate_Float(m_EffectWDesc.vSizeSpeedY.x, m_EffectWDesc.vSizeSpeedY.y);

	m_iFrame = Random::Generate_Int((_uint)m_EffectWDesc.vStartFrame.x, (_uint)m_EffectWDesc.vStartFrame.y);

	m_fColor = Random::Generate_Float(m_EffectWDesc.vColor.x, m_EffectWDesc.vColor.y);

	return S_OK;
}

void CEffectW::Tick(_double TimeDelta) 
{
	__super::Tick(TimeDelta);
}

void CEffectW::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CEffectW::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CEffectW::Update_Frame(_double TimeDelta)
{
	m_FrameAccTime += TimeDelta;

	if (m_FrameAccTime >= m_dFrameSpeed)
	{
		++m_iFrame;
		m_FrameAccTime = 0.0;
	
		if (m_iFrame >= m_EffectWDesc.iNumX * m_EffectWDesc.iNumY)
		{
			if (!m_EffectWDesc.bLoof)
			{
				m_iFrame = m_EffectWDesc.iNumX * m_EffectWDesc.iNumY - 1;
				
				CEffectW_Manager::Get_Instance()->Collect_Effect((CEffectW_Manager::EFFECTW_TYPE)m_EffectWDesc.eEffectWType , this);

				Set_Dead();
			}
			else
			{
				m_iFrame = 0;
			}
		}
	}
}

void CEffectW::Reset_Data(void* pArg)
{
	memcpy(&m_EffectWDesc, pArg, sizeof m_EffectWDesc);

	m_fPlusX = Random::Generate_Float(m_EffectWDesc.vStartPosX.x, m_EffectWDesc.vStartPosX.y);
	m_fPlusY = Random::Generate_Float(m_EffectWDesc.vStartPosY.x, m_EffectWDesc.vStartPosY.y);
	m_fPlusZ = Random::Generate_Float(m_EffectWDesc.vStartPosZ.x, m_EffectWDesc.vStartPosZ.y);

	m_dFrameSpeed = (_double)Random::Generate_Float(m_EffectWDesc.vFrameSpeed.x, m_EffectWDesc.vFrameSpeed.y);

	m_dSpeedX = (_double)Random::Generate_Float(m_EffectWDesc.vSpeedX.x, m_EffectWDesc.vSpeedX.y);
	m_dSpeedY = (_double)Random::Generate_Float(m_EffectWDesc.vSpeedY.x, m_EffectWDesc.vSpeedY.y);
	m_dSpeedZ = (_double)Random::Generate_Float(m_EffectWDesc.vSpeedZ.x, m_EffectWDesc.vSpeedZ.y);

	m_vSize = { Random::Generate_Float(m_EffectWDesc.vStartSizeX.x,m_EffectWDesc.vStartSizeX.y) , Random::Generate_Float(m_EffectWDesc.vStartSizeY.x, m_EffectWDesc.vStartSizeY.y),1.f };

	m_fSizeSpeedX = Random::Generate_Float(m_EffectWDesc.vSizeSpeedX.x, m_EffectWDesc.vSizeSpeedX.y);
	m_fSizeSpeedY = Random::Generate_Float(m_EffectWDesc.vSizeSpeedY.x, m_EffectWDesc.vSizeSpeedY.y);

	m_iFrame = Random::Generate_Int((_uint)m_EffectWDesc.vStartFrame.x, (_uint)m_EffectWDesc.vStartFrame.y);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_EffectWDesc.vPos + XMVectorSet(m_fPlusX, m_fPlusY, m_fPlusZ, 0.f));

	m_FrameAccTime = 0.0;
	m_dLifeAccTime = 0.0;

	m_fAlpha = m_EffectWDesc.fAlpha;
	m_fAlphaSpeed = m_EffectWDesc.fAlphaSpeed;

	m_fColor = Random::Generate_Float(m_EffectWDesc.vColor.x, m_EffectWDesc.vColor.y);
}

void CEffectW::Free()
{
	__super::Free();
}
