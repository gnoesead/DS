#include "pch.h"
#include "..\Public\Character.h"
#include "Camera_Free.h"
#include "GameInstance.h"

#include "AtkCollManager.h"

CCharacter::CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLandObject(pDevice, pContext)
{
}

CCharacter::CCharacter(const CCharacter& rhs)
	: CLandObject(rhs)
{
}

void CCharacter::Add_HitCollider(CGameObject* pAtkColl)
{
	if (nullptr == pAtkColl)
		return;

	if (1 > m_HitCollider.size())
		m_HitCollider.emplace_back(dynamic_cast<CAtkCollider*>(pAtkColl));
	else
	{
		_uint iCount = { 0 };

		for (auto iter = m_HitCollider.begin(); iter != m_HitCollider.end(); iter++)
		{
			if (pAtkColl == (*iter))
				break;

			iCount++;
		}

		if (m_HitCollider.size() <= iCount)
		{
			m_HitCollider.emplace_back(dynamic_cast<CAtkCollider*>(pAtkColl));
		}
	}
}

HRESULT CCharacter::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		memcpy(&m_CharacterDesc, pArg, sizeof m_CharacterDesc);
	}

	m_CharacterDesc.WorldInfo.vScale = _float3(0.8f, 0.8f, 0.8f);
	m_CharacterDesc.WorldInfo.fDegree = 0.f;

	if (FAILED(__super::Initialize(&m_CharacterDesc.WorldInfo)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	// 점광원 달기
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	LIGHTDESC tLightInfo;
	ZeroMemory(&tLightInfo, sizeof tLightInfo);

	tLightInfo.eType = LIGHTDESC::TYPE_POINT;

	XMStoreFloat4(&tLightInfo.vLightPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	tLightInfo.fLightRange = 10.f;
	tLightInfo.vLightDiffuse = _float4(0.3f, 0.3f, 0.3f, 1.f);
	tLightInfo.vLightAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	tLightInfo.vLightSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	pGameInstance->Add_Light(m_pDevice, m_pContext,tLightInfo ,  m_pTransformCom);

	Safe_Release(pGameInstance);

	if (nullptr != pArg)
	{
		//초기 위치 설정
		m_pTransformCom->Scaling(m_WorldInfo.vScale);
		m_pTransformCom->Rotation(XMLoadFloat3(&m_WorldInfo.vAxis), m_WorldInfo.fDegree);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_WorldInfo.vPosition));

		m_fLand_Y = m_CharacterDesc.Land_Y;
		m_eCurNavi = m_CharacterDesc.eCurNavi;
		m_eNextNavi = m_eCurNavi;
	}

	return S_OK;
}

void CCharacter::Tick(_double dTimeDelta)
{
	__super::Tick(dTimeDelta);

	Check_HitCollDead();
	Check_HitType();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_VILLAGE || pGameInstance->Get_CurLevelIdx() == LEVEL_HOUSE) {
		m_pTransformCom->Scaling(_float3(0.67f, 0.67f, 0.67f));
	}
	

	Safe_Release(pGameInstance);


	for (_uint i = 0; i < COLL_END; i++)
		m_pColliderCom[i]->Tick(m_pTransformCom->Get_WorldMatrix(), dTimeDelta);
}

void CCharacter::LateTick(_double dTimeDelta)
{
	__super::LateTick(dTimeDelta);

#ifdef _DEBUG
	for (_uint i = 0; i < COLL_END; i++)
	{
		if (FAILED(m_pRendererCom->Add_DebugGroup(m_pColliderCom[i])))
			return;
	}
#endif // _DEBUG
}

HRESULT CCharacter::Render()
{
	return S_OK;
}

CTransform* CCharacter::Get_TransformCom()
{
	return m_pTransformCom;
}

HRESULT CCharacter::Read_Animation_Control_File(const char* szBinfilename)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	char szFullPath[MAX_PATH] = { "" };
	strcpy_s(szFullPath, "../Bin/Resources/AnimToolBin/");
	strcat_s(szFullPath, szBinfilename);

	ifstream fin;
	fin.open(szFullPath, ios::binary);
	if (false == fin.is_open())
	{
		return E_FAIL;
	}

	_int AnimSize;
	fin.read(reinterpret_cast<char*>(&AnimSize), sizeof(_int));

	for (_int i = 0; i < AnimSize; i++)
	{
		CAnimation::CONTROLDESC ControlDesc;

		fin.read(reinterpret_cast<char*>(&ControlDesc.m_fAnimationSpeed), sizeof(_float));
		fin.read(reinterpret_cast<char*>(&ControlDesc.m_iConnect_Anim), sizeof(_int));
		fin.read(reinterpret_cast<char*>(&ControlDesc.m_isCombo), sizeof(_bool));
		fin.read(reinterpret_cast<char*>(&ControlDesc.m_iConnect_ComboAnim), sizeof(_int));
		fin.read(reinterpret_cast<char*>(&ControlDesc.m_isRootAnimation), sizeof(_bool));

		_int isizeEvent;
		fin.read(reinterpret_cast<char*>(&isizeEvent), sizeof(_int));

		for (_int i = 0; i < isizeEvent; i++)
		{
			CAnimation::EVENTDESC EventDesc;
			fin.read(reinterpret_cast<char*>(&EventDesc.m_dTime), sizeof(_double));
			fin.read(reinterpret_cast<char*>(&EventDesc.m_isFirst), sizeof(_bool));

			ControlDesc.m_vecTime_Event.emplace_back(EventDesc);
		}
		m_pModelCom->Set_Animation_Control(i, ControlDesc);
	}
	fin.close();

	Safe_Release(pGameInstance);

	return S_OK;
}

void CCharacter::RootAnimation(_double dTimeDelta)
{
	CAnimation* pAnim = m_pModelCom->Get_Animation();

	if (pAnim->Get_ControlDesc().m_isRootAnimation)
	{
		//애니메이션 시작시 첫 위치
		if (pAnim->Get_AnimationDesc().m_dTimeAcc == 0.0)
		{
			XMStoreFloat4(&m_Save_RootPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_Save_RootPos));
		}
		else
		{
			_float4 fPos;
			XMStoreFloat4(&fPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

			_float3 RootPosition = pAnim->Get_RootPosition();
			_float4x4 RootWorldConvert = m_pTransformCom->Get_WorldFloat4x4();
			_float3 FinalRootPos = { 0.0f, 0.0f, 0.0f };
			XMStoreFloat3(&FinalRootPos, XMVector3TransformCoord(XMLoadFloat3(&RootPosition), XMLoadFloat4x4(&RootWorldConvert)));

			_float4 Final = { -FinalRootPos.x * 0.005f, FinalRootPos.y * 0.005f , -FinalRootPos.z * 0.005f, 1.f };

			// 플레이어의 월드 위치를 기준으로 Root bone의 위치를 변화시킴
			_float4  SubPos = { m_Save_RootPos.x + Final.x, m_Save_RootPos.y + Final.y , m_Save_RootPos.z + Final.z , 1.f };

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&SubPos));
		}
	}
}

_bool CCharacter::EventCallProcess()
{
	CAnimation* pAnim = m_pModelCom->Get_Animation();

	CAnimation::CONTROLDESC ControlDesc = pAnim->Get_ControlDesc();

	if (m_iPreAnimIndex_ForEvent != m_pModelCom->Get_iCurrentAnimIndex())
	{
		m_pModelCom->Set_EventReset(m_iPreAnimIndex_ForEvent);
		m_iPreAnimIndex_ForEvent = m_pModelCom->Get_iCurrentAnimIndex();

		m_iEvent_Index = 0;
	}

	if (ControlDesc.m_isEventCall)
	{
		ControlDesc.m_isEventCall = false;

		pAnim->Set_ControlDesc(ControlDesc);

		return true;
	}

	return false;
}


void CCharacter::Reset_Decleration(_float fResetSpeed)
{
	CAnimation* pAnim = m_pModelCom->Get_Animation();

	if (pAnim->Get_AnimationDesc().m_dTimeAcc == 0)
	{
		m_isReset_Atk_MoveControl = true;
	}
	if (m_isReset_Atk_MoveControl)
	{
		m_isReset_Atk_MoveControl = false;
		m_fAtk_MoveControl = fResetSpeed;
	}
}



void CCharacter::Go_Straight_Deceleration(_double dTimeDelta, _int AnimIndex, _float ResetSpeed, _float fDecrease)
{
	//서서히 느려지는 Transform 이동
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		Reset_Decleration(ResetSpeed);

		m_pTransformCom->Go_Straight(dTimeDelta * m_fAtk_MoveControl , m_pNavigationCom[m_eCurNavi]);
		m_fAtk_MoveControl -= fDecrease;
		if (m_fAtk_MoveControl <= 0.0f)
		{
			m_fAtk_MoveControl = 0.0f;
		}
	}
}

void CCharacter::Go_Backward_Deceleration(_double dTimeDelta, _int AnimIndex, _float ResetSpeed, _float fDecrease)
{
	//서서히 느려지는 Transform 이동
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		Reset_Decleration(ResetSpeed);

		m_pTransformCom->Go_Backward(dTimeDelta * m_fAtk_MoveControl, m_pNavigationCom[m_eCurNavi]);
		m_fAtk_MoveControl -= fDecrease;
		if (m_fAtk_MoveControl <= 0.0f)
		{
			m_fAtk_MoveControl = 0.0f;
		}
	}
}

void CCharacter::Go_Left_Deceleration(_double dTimeDelta, _int AnimIndex, _float ResetSpeed, _float fDecrease)
{
	//서서히 느려지는 Transform 이동
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		Reset_Decleration(ResetSpeed);

		m_pTransformCom->Go_Left(dTimeDelta * m_fAtk_MoveControl, m_pNavigationCom[m_eCurNavi]);
		m_fAtk_MoveControl -= fDecrease;
		if (m_fAtk_MoveControl <= 0.0f)
		{
			m_fAtk_MoveControl = 0.0f;
		}
	}
}

void CCharacter::Go_Right_Deceleration(_double dTimeDelta, _int AnimIndex, _float ResetSpeed, _float fDecrease)
{
	//서서히 느려지는 Transform 이동
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		Reset_Decleration(ResetSpeed);

		m_pTransformCom->Go_Right(dTimeDelta * m_fAtk_MoveControl, m_pNavigationCom[m_eCurNavi]);
		m_fAtk_MoveControl -= fDecrease;
		if (m_fAtk_MoveControl <= 0.0f)
		{
			m_fAtk_MoveControl = 0.0f;
		}
	}
}

void CCharacter::Go_Dir_Deceleration(_double dTimeDelta, _int AnimIndex, _float ResetSpeed, _float fDecrease, _float4 Dir)
{
	//서서히 느려지는 Transform 이동
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		Reset_Decleration(ResetSpeed);

		m_pTransformCom->Go_Dir(dTimeDelta * m_fAtk_MoveControl, XMLoadFloat4(&Dir), m_pNavigationCom[m_eCurNavi]);
		m_fAtk_MoveControl -= fDecrease;
		if (m_fAtk_MoveControl <= 0.0f)
		{
			m_fAtk_MoveControl = 0.0f;
		}
	}
}

void CCharacter::Go_Dir_Constant(_double dTimeDelta, _int AnimIndex, _float constantSpeed, _float4 Dir, _bool bIsJumpOn)
{
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		if(!bIsJumpOn)
			m_pTransformCom->Go_Dir(dTimeDelta * constantSpeed, XMLoadFloat4(&Dir), m_pNavigationCom[m_eCurNavi]);
		else
			m_pTransformCom->Go_Dir(dTimeDelta * constantSpeed, XMLoadFloat4(&Dir));
	}
}

void CCharacter::Go_Straight_Constant(_double dTimeDelta, _int AnimIndex, _float constantSpeed, _bool bIsJumpOn)
{
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		if(!bIsJumpOn)
			m_pTransformCom->Go_Straight(dTimeDelta * constantSpeed , m_pNavigationCom[m_eCurNavi]);
		else
			m_pTransformCom->Go_Straight(dTimeDelta * constantSpeed);
	}
}

void CCharacter::Go_Backward_Constant(_double dTimeDelta, _int AnimIndex, _float constantSpeed)
{
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		m_pTransformCom->Go_Backward(dTimeDelta * constantSpeed, m_pNavigationCom[m_eCurNavi]);
	}
}

void CCharacter::Go_Left_Constant(_double dTimeDelta, _int AnimIndex, _float constantSpeed)
{
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		m_pTransformCom->Go_Left(dTimeDelta * constantSpeed, m_pNavigationCom[m_eCurNavi]);
	}
}

void CCharacter::Go_Right_Constant(_double dTimeDelta, _int AnimIndex, _float constantSpeed)
{
	if (AnimIndex == m_pModelCom->Get_iCurrentAnimIndex())
	{
		m_pTransformCom->Go_Right(dTimeDelta * constantSpeed, m_pNavigationCom[m_eCurNavi]);
	}
}

void CCharacter::Go_Straight_Deceleration_Common(_double dTimeDelta, _float ResetSpeed, _float fDecrease)
{
	//서서히 느려지는 Transform 이동
	Reset_Decleration(ResetSpeed);

	m_pTransformCom->Go_Straight(dTimeDelta * m_fAtk_MoveControl , m_pNavigationCom[m_eCurNavi]);
	m_fAtk_MoveControl -= fDecrease;
	if (m_fAtk_MoveControl <= 0.0f)
	{
		m_fAtk_MoveControl = 0.0f;
	}
}

void CCharacter::Go_Dir_Constant(_double dTimeDelta, DIR Dir, _uint iAnimindex, _float fSpeed, _double dStartRatio, _double dEndRatio)
{
	// 원하는 애니메이션의 비율만큼 이동
	switch (Dir)
	{
	case DIR_UP:
	{
		if (m_pModelCom->Get_AnimRatio(iAnimindex, dStartRatio) && !m_pModelCom->Get_AnimRatio(iAnimindex, dEndRatio))
			Go_Straight_Constant(dTimeDelta, iAnimindex, fSpeed);
	}
	break;
	case DIR_DOWN:
	{
		if (m_pModelCom->Get_AnimRatio(iAnimindex, dStartRatio) && !m_pModelCom->Get_AnimRatio(iAnimindex, dEndRatio))
			Go_Backward_Constant(dTimeDelta, iAnimindex, fSpeed);
	}
	break;
	case DIR_RIGHT:
	{
		if (m_pModelCom->Get_AnimRatio(iAnimindex, dStartRatio) && !m_pModelCom->Get_AnimRatio(iAnimindex, dEndRatio))
			Go_Right_Constant(dTimeDelta, iAnimindex, fSpeed);
	}
	break;
	case DIR_LEFT:
	{
		if (m_pModelCom->Get_AnimRatio(iAnimindex, dStartRatio) && !m_pModelCom->Get_AnimRatio(iAnimindex, dEndRatio))
			Go_Left_Constant(dTimeDelta, iAnimindex, fSpeed);
	}
	break;

	}
	
}

void CCharacter::Navigation_To_Ground(_double dTimeDelta)
{


}

void CCharacter::Gravity(_double dTimeDelta)
{
	_float4 Pos;
	XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	

	//점프 상태
	if (m_isJumpOn)
	{
		if (m_isJumpStop)
		{
			m_dTime_JumpStop += dTimeDelta;
			if (m_dTime_JumpStop >= m_dTime_JumpStop_Duration)
				m_isJumpStop = false;
		}
		else
		{
			m_pTransformCom->Go_Up(dTimeDelta * m_fJump_Acc);
			m_fJump_Acc -= m_fGravity_Fall;
		}

		m_dDelay_Fall += dTimeDelta;
		if (m_dDelay_Fall > 0.1)
		{
			if (Pos.y <= m_fLand_Y)
			{
				m_isJumpOn = false;
				Pos.y = m_fLand_Y;
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Pos));

			}
		}
	}
	//땅 위 상태
	else
	{
		Pos.y = m_fLand_Y;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Pos));

		m_fJump_Acc = 0.0f;
		m_isFirst_JumpAtk = true;
		m_isJumpStop = false;
	}


}

void CCharacter::Ground_Animation_Play(_int CurAnim, _int GroundAnim)
{
	if (m_pModelCom->Get_iCurrentAnimIndex() == CurAnim)
	{
		_float4 Pos;
		XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		if (Pos.y <= m_fLand_Y)
		{
			m_pModelCom->Set_Animation(GroundAnim);
		}
	}

}

void CCharacter::Jumping(_float ResetSpeed, _float fFallDecrease)
{
	m_fJump_Acc = ResetSpeed;
	m_isJumpOn = true;
	m_dDelay_Fall = 0.0;

	m_fGravity_Fall = fFallDecrease;
}

void CCharacter::JumpStop(_double dDuration)
{
	m_isJumpStop = true;
	m_dTime_JumpStop_Duration = dDuration;
	m_dTime_JumpStop = 0.0;
}

void CCharacter::Make_AttackColl(const _tchar* pLayerTag, _float3 Size, _float3 Pos, _double DurationTime, CAtkCollider::ATK_TYPE AtkType, _vector vDir, _float fDmg)
{
	CAtkCollider::ATKCOLLDESC AtkCollDesc;
	ZeroMemory(&AtkCollDesc, sizeof AtkCollDesc);

	AtkCollDesc.ColliderDesc.vSize = Size;
	AtkCollDesc.ColliderDesc.vPosition = Pos;

	AtkCollDesc.dLifeTime = DurationTime;

	AtkCollDesc.pTransform = m_pTransformCom;

	AtkCollDesc.eAtkType = AtkType;

	AtkCollDesc.fDamage = fDmg;

	XMStoreFloat4(&AtkCollDesc.AtkDir, XMVector4Normalize(vDir));

	CAtkCollManager::GetInstance()->Reuse_Collider(pLayerTag, &AtkCollDesc);
}

void CCharacter::Check_HitCollDead()
{
	for (auto iter = m_HitCollider.begin(); iter != m_HitCollider.end();)
	{
		if (true == (*iter)->Get_Dead())
		{
			iter = m_HitCollider.erase(iter);
			continue;
		}

		++iter;
	}
}

void CCharacter::Check_HitType()
{
	for (auto& pHitColl : m_HitCollider)
	{
		if (false == pHitColl->Get_IsAttack(this))
		{
			if (pHitColl->Get_Collider()->Get_Hit_Small())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Small(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_ConnectSmall())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_ConnectSmall(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_Big())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Big(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_Blow())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Blow(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_Blow())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_BigBlow(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_Spin())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Spin(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_Upper())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Upper(true);
			}
			else if (pHitColl->Get_Collider()->Get_Hit_Bound())
			{
				m_pColliderCom[COLL_SPHERE]->Set_Hit_Bound(true);
			}

			pHitColl->Add_AtkObejct(this);
		}
	}
}

void CCharacter::Set_Height()
{
	m_fLand_Y = m_pNavigationCom[m_eCurNavi]->Compute_Height(m_pTransformCom);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (LEVEL_TRAIN == pGameInstance->Get_CurLevelIdx())
	{
		_float vPosX = XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		m_fLand_Y -= fabsf(vPosX - 205.15f) * 0.15f;
	}

	Safe_Release(pGameInstance);
}

void CCharacter::Camera_Shake(_double dShakeTime, _uint iShakePower)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Camera")));
	pCamera->Shake(dShakeTime, iShakePower);

	Safe_Release(pGameInstance);
}

HRESULT CCharacter::Add_Components()
{
	/* for.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		MSG_BOX("Failed to Add_Com_Renderer : CCharacter");
		return E_FAIL;
	}



	return S_OK;
}

void CCharacter::SetUp_Height()
{
	_fvector vPos = Compute_Height(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CCharacter::Tick_Collider(_double dTimeDelta)
{
	for (_uint i = 0; i < COLL_END; i++)
		m_pColliderCom[i]->Tick(m_pTransformCom->Get_WorldMatrix(), dTimeDelta);
}

void CCharacter::Free()
{
	__super::Free();

	m_HitCollider.clear();

	for (_uint i = 0; i < COLL_END; i++)
		Safe_Release(m_pColliderCom[i]);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

}
