#include "pch.h"
#include "..\Public\NPC.h"

#include "GameInstance.h"
#include "SoundMgr.h"
#include "Camera_Free.h"

#include "PlayerManager.h"
#include "Player.h"
#include "DialogManager.h"
#include "MissionManager.h"





CNPC::CNPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter(pDevice, pContext)
{
}

CNPC::CNPC(const CNPC& rhs)
	: CCharacter(rhs)
{
}

HRESULT CNPC::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNPC::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Create_Icon();
	Create_Interaction();


	return S_OK;
}

void CNPC::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCharacter* pPlayer = dynamic_cast<CCharacter*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), 0));

	CTransform* m_pTargetTransformCom = pPlayer->Get_TransformCom();

	m_Player_Pos = m_pTargetTransformCom->Get_State(CTransform::STATE_POSITION);

	_float Distance = Convert::GetLength(m_Player_Pos - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (Distance < 2.f) {

		CDialogManager::GetInstance()->Set_Dialog_Type(m_CharacterDesc.NPCDesc.Dialog_Type);

		if (m_CharacterDesc.NPCDesc.Dialog_Type >= 3 && m_CharacterDesc.NPCDesc.Dialog_Type <= 7) {

			if (pGameInstance->Get_DIKeyDown(DIK_F) == true && m_Sub_Up_Done == false) {
				m_Sub_Up_Done = true;
				CMissionManager::GetInstance()->Plus_Sub_Num();
			}
				

		}
	
	}

	

	Safe_Release(pGameInstance);

	if (m_pIcon != nullptr)
		m_pIcon->Tick(dTimeDelta);

	if (m_pInteraction != nullptr) {
		m_pInteraction->Tick(dTimeDelta);
	}

	if (m_pInteraction_Back != nullptr) {
		m_pInteraction_Back->Tick(dTimeDelta);
	}

	if (true == m_isDead)
		return;


}

void CNPC::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

	if (m_pIcon != nullptr)
		m_pIcon->LateTick(dTimeDelta);

	if (m_pInteraction != nullptr) {
		m_pInteraction->LateTick(dTimeDelta);
	}

	if (m_pInteraction_Back != nullptr) {
		m_pInteraction_Back->LateTick(dTimeDelta);
	}

	
}

HRESULT CNPC::Render()
{
	

	return S_OK;
}

HRESULT CNPC::Render_ShadowDepth()
{

	return S_OK;
}

void CNPC::Create_Icon()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CFIcon::UIDESC UIDesc;
	ZeroMemory(&UIDesc, sizeof UIDesc);


	if (m_CharacterDesc.NPCDesc.Icon_Type == 0) {
		UIDesc.m_Is_Reverse = false;
		UIDesc.m_Type = 0;
		UIDesc.m_Up_Mount = 1.85f;
		UIDesc.pParentTransform = m_pTransformCom;

		m_pIcon = dynamic_cast<CFIcon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_FIcon"), &UIDesc));
	}
	if (m_CharacterDesc.NPCDesc.Icon_Type == 1) {
		UIDesc.m_Type = 1;
		UIDesc.m_Up_Mount = 1.85f;
		UIDesc.pParentTransform = m_pTransformCom;

		m_pIcon = dynamic_cast<CFIcon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_FIcon"), &UIDesc));
	}
	if (m_CharacterDesc.NPCDesc.Icon_Type == 2) {
		UIDesc.m_Type = 2;
		UIDesc.m_Up_Mount = 1.85f;
		UIDesc.pParentTransform = m_pTransformCom;

		m_pIcon = dynamic_cast<CFIcon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_FIcon"), &UIDesc));
	}
	if (m_CharacterDesc.NPCDesc.Icon_Type == 3) {
		UIDesc.m_Type = 3;
		UIDesc.m_Up_Mount = 1.85f;
		UIDesc.pParentTransform = m_pTransformCom;

		m_pIcon = dynamic_cast<CFIcon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_FIcon"), &UIDesc));
	}
	if (m_CharacterDesc.NPCDesc.Icon_Type == 4) {
		UIDesc.m_Type = 4;
		UIDesc.m_Up_Mount = 1.85f;
		UIDesc.pParentTransform = m_pTransformCom;

		m_pIcon = dynamic_cast<CFIcon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_FIcon"), &UIDesc));
	}
	if (m_CharacterDesc.NPCDesc.Icon_Type == 5) {
		UIDesc.m_Type = 5;
		UIDesc.m_Up_Mount = 1.85f;
		UIDesc.pParentTransform = m_pTransformCom;

		m_pIcon = dynamic_cast<CFIcon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_FIcon"), &UIDesc));
	}
	if (m_CharacterDesc.NPCDesc.Icon_Type == 6) {
		UIDesc.m_Type = 6;
		UIDesc.m_Up_Mount = 1.85f;
		UIDesc.pParentTransform = m_pTransformCom;

		m_pIcon = dynamic_cast<CFIcon*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_FIcon"), &UIDesc));
	}

	Safe_Release(pGameInstance);
}

void CNPC::Create_Interaction()
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CInteraction::UIDESC UIDesc;
	ZeroMemory(&UIDesc, sizeof UIDesc);

	if (m_CharacterDesc.NPCDesc.Interaction == true) {
		UIDesc.m_Is_Reverse = false;
		UIDesc.m_Type = 1;
		UIDesc.m_Up_Mount = 1.2f;
		UIDesc.pParentTransform = m_pTransformCom;

		m_pInteraction = dynamic_cast<CInteraction*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Interaction"), &UIDesc));

		//UIDesc.m_Type = 0;

		//m_pInteraction_Back = dynamic_cast<CInteraction*>(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Interaction"), &UIDesc));

	}


	Safe_Release(pGameInstance);
}

void CNPC::Dir_Setting(_bool Reverse)
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

void CNPC::Get_PlayerComponent()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), CPlayerManager::GetInstance()->Get_PlayerIndex()));

	//m_pPlayerTransformCom = dynamic_cast<CTransform*>(pGameInstance->Get_Component(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), TEXT("Com_Transform")));
	m_pPlayerTransformCom = pPlayer->Get_TransformCom();

	if (m_pPlayerTransformCom == nullptr)
	{
		Safe_Release(pGameInstance);
		return;
	}

	Safe_Release(pGameInstance);
}

_bool CNPC::Check_Distance(_float fDistance)
{
	// 내가 설정한 Distance보다 가깝거나 같으면 true 멀면 false
	Get_PlayerComponent();

	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	return Compute::DistCheck(vPlayerPos, vMonsterPos, fDistance);
}

_float CNPC::Calculate_Distance()
{
	Get_PlayerComponent();

	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vDir = XMVector3Normalize(vPlayerPos - vMonsterPos);
	_float fDistance = Convert::GetLength(vPlayerPos - vMonsterPos);

	return fDistance;
}

_vector CNPC::Calculate_Dir()
{
	Get_PlayerComponent();

	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vDir = XMVector3Normalize(vPlayerPos - vMonsterPos);

	return vDir;
}

_vector CNPC::Calculate_Dir_FixY()
{
	Get_PlayerComponent();

	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	return Compute::Dir_FixY(vPlayerPos, vMonsterPos);
}

_vector CNPC::Calculate_Dir_Cross()
{
	Get_PlayerComponent();

	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vDir = XMVector3Normalize(vPlayerPos - vMonsterPos);

	_vector vUp = { 0.0f, 1.0f, 0.0f, 0.0f };
	_vector vCross = XMVector3Normalize(XMVector3Cross(vDir, vUp));


	return vCross;
}

_float4 CNPC::Calculate_PlayerPos()
{
	return _float4();
}

_float CNPC::Calculate_To_Spot()
{
	_vector vNPCPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vSpotPos = XMLoadFloat4(&m_CharacterDesc.NPCDesc.WalkSpot[(m_iSpot_Index)]);
	_vector vDir = XMVector3Normalize(vSpotPos - vNPCPos);
	_float fDistance = Convert::GetLength(vSpotPos - vNPCPos);

	m_pTransformCom->LerpVector(vDir, 0.05f);

	if (fDistance < 0.15f)
	{
		if (m_isSpot_Reverse == false)
		{
			if (m_iSpot_Index == 2)
			{
				m_iSpot_Index = 1;
				m_isSpot_Reverse = true;
			}
			else
			{
				m_iSpot_Index++;
			}
		}
		else
		{
			if (m_iSpot_Index == 0)
			{
				m_iSpot_Index = 1;
				m_isSpot_Reverse = false;
			}
			else
			{
				m_iSpot_Index--;
			}
		}
		
	}

	return fDistance;
}

void CNPC::Find_Section()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), 0));

	m_iPlayer_Section = pPlayer->Get_Section();
	m_iPlayer_Section_Sub = pPlayer->Get_Section_Sub();

	Safe_Release(pGameInstance);

}

HRESULT CNPC::Add_Components()
{
	m_CharacterDesc.NaviDesc.iCurrentIndex = 0;

	/* for.Com_Navigation_Village_MainRoad1 */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_MainRoad1"),
		TEXT("Com_Navigation_Village_MainRoad1"), (CComponent**)&m_pNavigationCom[NAVI_VILLAGE_MAINROAD1], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_Village_MainRoad1: CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_Village_MainRoad2 */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_MainRoad2"),
		TEXT("Com_Navigation_Village_MainRoad2"), (CComponent**)&m_pNavigationCom[NAVI_VILLAGE_MAINROAD2], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_Village_MainRoad2 : CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_Village_InsideWall1*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_InsideWall1"),
		TEXT("Com_Navigation_Village_InsideWall1"), (CComponent**)&m_pNavigationCom[NAVI_VILLAGE_INSIDEWALL1], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_Village_InsideWall1: CPlayer");
		return E_FAIL;
	}

	/* for.Com_Navigation_Village_InsideWall2*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Village_InsideWall2"),
		TEXT("Com_Navigation_Village_InsideWall2"), (CComponent**)&m_pNavigationCom[NAVI_VILLAGE_INSIDEWALL2], &m_CharacterDesc.NaviDesc)))
	{
		MSG_BOX("Failed to Add_Com_Navigation_Village_InsideWall2: CPlayer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNPC::SetUp_ShaderResources()
{

	return S_OK;
}

CNPC* CNPC::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNPC* pInstance = new CNPC(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNPC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNPC::Clone(void* pArg)
{
	CNPC* pInstance = new CNPC(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNPC");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC::Free()
{
	Safe_Release(m_pIcon);
	Safe_Release(m_pInteraction);
	Safe_Release(m_pInteraction_Back);


	__super::Free();
}
