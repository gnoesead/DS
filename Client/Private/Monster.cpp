#include "pch.h"
#include "..\Public\Monster.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "Camera_Free.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster& rhs)
	: CCharacter(rhs)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	

	return S_OK;
}

void CMonster::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	if (true == m_isDead)
		return;


}

void CMonster::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	
}

HRESULT CMonster::Render()
{
	return S_OK;
}

HRESULT CMonster::Render_ShadowDepth()
{

	return S_OK;
}

void CMonster::Get_PlayerComponent()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), TEXT("Com_Transform")));
	//m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_FINALBOSS, TEXT("Layer_Player"), TEXT("Com_Transform")));

	Safe_Release(pGameInstance);
}

void CMonster::Calculate_To_Player()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CTransform* pTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), TEXT("Com_Transform")));

	_vector vPlayerPos = pTransform->Get_State(CTransform::STATE_POSITION);

	_vector vDistance = XMVectorSubtract(m_pTransformCom->Get_State(CTransform::STATE_POSITION), vPlayerPos);
	_vector vDir_To_Player = XMVector3Normalize(vDistance);
	
	m_fDistance_To_Player = XMVectorGetX(XMVector3Length(vDistance));
	XMStoreFloat4(&m_PlayerPos, vPlayerPos);
	XMStoreFloat4(&m_Dir_To_Player, vDir_To_Player);

	Safe_Release(pGameInstance);
}


_bool CMonster::Check_Distance(_float fDistance)
{
	// 내가 설정한 Distance보다 가깝거나 같으면 true 멀면 false
	Get_PlayerComponent();

	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	return Compute::DistCheck(vPlayerPos, vMonsterPos, fDistance);
}

_float CMonster::Calculate_Distance()
{
	Get_PlayerComponent();

	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vDir = XMVector3Normalize(vPlayerPos - vMonsterPos);
	_float fDistance = Convert::GetLength(vPlayerPos - vMonsterPos);

	return fDistance;
}

_vector CMonster::Calculate_Dir()
{
	Get_PlayerComponent();

	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vDir = XMVector3Normalize(vPlayerPos - vMonsterPos);

	return vDir;
}

_vector CMonster::Calculate_Dir_FixY()
{
	Get_PlayerComponent();

	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	return Compute::Dir_FixY(vPlayerPos, vMonsterPos);
}

void CMonster::Dir_Setting(_bool Reverse)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	//카메라 방향 구해놓기
	CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Camera"), 0));
	_float4 CameraLook = pCamera->Get_CameraLook();
	CameraLook.y = 0.0f;
	CameraLook.w = 0.0f;
	_vector vLook = XMVector4Normalize(XMLoadFloat4(&CameraLook));
	_vector	vUp = { 0.0f, 1.0f, 0.0f , 0.0f };
	_vector crossLeft = XMVector3Cross(vLook, vUp);

	//45degree look
	_vector quaternionRotation = XMQuaternionRotationAxis(vUp, XMConvertToRadians(45.0f));
	_vector v45Rotate = XMVector3Rotate(vLook, quaternionRotation);

	//135degree look
	_vector quaternionRotation2 = XMQuaternionRotationAxis(vUp, XMConvertToRadians(135.0f));
	_vector v135Rotate = XMVector3Rotate(vLook, quaternionRotation2);

	
	if (Reverse)
	{
		v45Rotate = -v45Rotate;
		v135Rotate = -v135Rotate;
	}

	Safe_Release(pGameInstance);
}

HRESULT CMonster::Add_Components()
{

	return S_OK;
}

HRESULT CMonster::SetUp_ShaderResources()
{

	return S_OK;
}

CMonster* CMonster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster* pInstance = new CMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster::Clone(void* pArg)
{
	CMonster* pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{
	__super::Free();
}
