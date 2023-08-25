#include "pch.h"
#include "..\Public\Effect_Particle.h"

#include "GameInstance.h"
#include "ParticleSystem.h"

CEffect_Particle::CEffect_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect(pDevice, pContext)
{
}

CEffect_Particle::CEffect_Particle(const CEffect_Particle& rhs)
	: CEffect(rhs)
{

}

HRESULT CEffect_Particle::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Particle::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_eEffectDesc.eEffectType = EFFECT_PARTICLE;

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

void CEffect_Particle::Tick(_double _dTimeDelta)
{
	_double dTimeDelta = _dTimeDelta * m_dPlaySpeed;

	__super::Tick(dTimeDelta);

	if (m_ParentDesc.pParent->Get_isPlaying())
	{
		CVIBuffer_Point_Instance_Effect::INSTANCEDESC InstanceDesc;
		ZeroMemory(&InstanceDesc, sizeof InstanceDesc);

		InstanceDesc.eShapeType = m_eEffectDesc.eShapeType;
		InstanceDesc.fStartLifeTimeMin = m_eEffectDesc.fStartLifeTimeMin;
		InstanceDesc.isLooping = m_eEffectDesc.isLooping;
		InstanceDesc.fMinSpeed = 1.f;
		InstanceDesc.fMaxSpeed = 2.f;
		InstanceDesc.vRange = _float3(0.0f, 0.0f, 0.0f);
		InstanceDesc.pParent = this;

		m_pVIBufferCom->Tick(dTimeDelta, &InstanceDesc);

	}
	else if ((m_ParentDesc.pParent->Get_isStopped()))
	{
		m_pVIBufferCom->Set_NumInstance(0);
	}
}

void CEffect_Particle::LateTick(_double _dTimeDelta)
{
	_double dTimeDelta = _dTimeDelta * m_dPlaySpeed;

	__super::LateTick(dTimeDelta);
}

HRESULT CEffect_Particle::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_iPassIndex)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Particle::Add_Components(void)
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	CVIBuffer_Point_Instance_Effect::INSTANCEDESC		InstanceDesc;

	InstanceDesc.fMinSpeed = 1.f;
	InstanceDesc.fMaxSpeed = 2.f;
	InstanceDesc.vRange = _float3(0.0f, 0.0f, 0.0f);
	InstanceDesc.pParent = this;

	/* For.Com_VIBufferPointInstanceEffect */
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_VIBuffer_Point_Instance_Effect"),
		TEXT("Com_VIBufferPointInstance"), (CComponent**)&m_pVIBufferCom, &InstanceDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(0, TEXT("Prototype_Component_Shader_VtxPointInstanceEffect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Particle::SetUp_ShaderResources(void)
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Particle::Check_PassIndex(void)
{
	m_iPassIndex = 0;
}

CEffect_Particle* CEffect_Particle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Particle* pInstance = new CEffect_Particle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Particle::Clone(void* pArg)
{
	CEffect_Particle* pInstance = new CEffect_Particle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect_Particle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Particle::Free(void)
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
}
