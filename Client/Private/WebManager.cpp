#include "pch.h"
#include "..\Public\WebManager.h"

#include "AtkCollManager.h"

#include "GameInstance.h"

#include "WebShot.h"

#include "Boss_Akaza.h"

IMPLEMENT_SINGLETON(CWebManager)

CWebManager::CWebManager()
{
}

void CWebManager::Tick(_double dTimeDelta)
{
	_float4 PlayerPos;
	XMStoreFloat4(&PlayerPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_float4 Dir = { 0.0f, 0.0f , -1.0f, 0.0f };


#pragma region TriggerSet
	//Trigger onoff
	if (m_iTrigger_Index == 1 && PlayerPos.z >= 264.2f)
	{
		m_iTrigger_Index++;
		m_isTrigger_First = true;
	}
	else if (m_iTrigger_Index == 2 && PlayerPos.z >= 292.9f)
	{
		m_iTrigger_Index++;
		m_isTrigger_Second = true;
	}
	else if (m_iTrigger_Index == 3 && PlayerPos.z >= 321.6f)
	{
		m_iTrigger_Index++;
		m_isTrigger_Third = true;
	}
	//아카자 만남
	else if (m_iTrigger_Index == 4 && PlayerPos.z >= 364.2f)
	{
		m_iTrigger_Index++;
		m_isTrigger_Akaza = true;
	}
#pragma endregion

	/*
		PlayerPos.y += 1.5f;

		Shoot_ArrowWeb(PlayerPos, Dir);
		Shoot_WebBall(PlayerPos, Dir);
		Shoot_JikWeb(PlayerPos, Dir, -45.0f);
		Shoot_WideWeb(PlayerPos, Dir);
		Shoot_JikWeb(PlayerPos, Dir, -45.0f);
		Shoot_SlideWeb(PlayerPos, Dir, 180.0f);
	*/

#pragma region Event
	//첫 거미줄 웨이브
	if (m_isTrigger_First)
	{

	}
	//둘 거미줄 웨이브
	if (m_isTrigger_Second)
	{

	}
	//셋 거미줄 웨이브
	if (m_isTrigger_Third)
	{

	}
	//아카자 이벤트
	if (m_isTrigger_Akaza)
	{
		if (m_isFirst_Akaza)
		{
			m_isFirst_Akaza = false;
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			CBoss_Akaza::CHARACTERDESC CharacterDesc;
			ZeroMemory(&CharacterDesc, sizeof CharacterDesc);

			CharacterDesc.WorldInfo.vPosition = _float4(204.9f, 0.f, 380.f, 1.f);
			pGameInstance->Add_GameObject(LEVEL_TRAIN, TEXT("Layer_NPC"),
				TEXT("Prototype_GameObject_Monster_Akaza"), &CharacterDesc);

			Safe_Release(pGameInstance);

		}
	}
#pragma endregion
}

void CWebManager::Create_WebShot(_int TexIdx, _float4 CreatePos, _float3 Scale, _float4 ShotDir, _float Turn)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CWebShot::WEBDESC WebDesc;
	WebDesc.iWebTexIndex = TexIdx;
	WebDesc.WorldInfo.vPosition = CreatePos;
	WebDesc.vScale = Scale;
	WebDesc.vDir = ShotDir;
	WebDesc.fTurn = Turn;

	if (FAILED(pGameInstance->Add_GameObject(LEVEL_TRAIN, TEXT("Layer_Shot"), TEXT("Prototype_GameObject_WebShot"), &WebDesc)))
	{
		MSG_BOX("Failed to Add_GameObject : WebWebShot");
		return;
	}

	Safe_Release(pGameInstance);
}

void CWebManager::Shoot_ArrowWeb(_float4 CreatePos, _float4 ShotDir)
{
	// index, Pos, Scale, Dir
	Create_WebShot(7, CreatePos, _float3{ 1.5f, 4.5f, 1.5f }, ShotDir);
}

void CWebManager::Shoot_WebBall(_float4 CreatePos, _float4 ShotDir)
{
	// index, Pos, Scale, Dir
	Create_WebShot(28, CreatePos, _float3{ 1.5f, 1.5f, 1.5f }, ShotDir);
}

void CWebManager::Shoot_WideWeb(_float4 CreatePos, _float4 ShotDir)
{
	// index, Pos, Scale, Dir
	Create_WebShot(12, CreatePos, _float3{ 20.0f, 6.0f, 6.0f }, ShotDir);
}

void CWebManager::Shoot_JikWeb(_float4 CreatePos, _float4 ShotDir, _float Turn)
{
	// index, Pos, Scale, Dir
	Create_WebShot(32, CreatePos, _float3{ 25.0f, 3.5f, 3.5f }, ShotDir, Turn);
}

void CWebManager::Shoot_SlideWeb(_float4 CreatePos, _float4 ShotDir, _float Turn)
{
	// index, Pos, Scale, Dir
	Create_WebShot(13, CreatePos, _float3{ 17.0f, 8.0f, 6.0f }, ShotDir);
}

void CWebManager::Make_WebBullet(_float3 Size, _float3 Pos, _vector vAtkDir, _double Speed,
	CAtkCollider::BULLET_TYPE eBulletType, const char* pEffectTag)
{
	CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
	EffectWorldDesc.vPosition.y = 1.f;
	CEffectPlayer::EFFECTWORLDDESC* pEffectWorldDesc = &EffectWorldDesc;


	CAtkCollider::ATKCOLLDESC AtkCollDesc;
	ZeroMemory(&AtkCollDesc, sizeof AtkCollDesc);

	AtkCollDesc.ColliderDesc.vSize = Size;
	AtkCollDesc.ColliderDesc.vPosition = Pos;

	AtkCollDesc.dLifeTime = 10.0;

	AtkCollDesc.pParentTransform = m_pTransformCom;

	AtkCollDesc.eAtkType = CAtkCollider::TYPE_WEB;
	AtkCollDesc.eBulletType = eBulletType;

	AtkCollDesc.fDamage = 1.0f;
	AtkCollDesc.Speed = Speed;

	AtkCollDesc.bBullet = true;
	//if (true == AtkCollDesc.bBullet)
	{
		strcpy_s(AtkCollDesc.pEffectTag, pEffectTag);
	}

	if (nullptr != pEffectWorldDesc)
		AtkCollDesc.EffectWorldDesc = *pEffectWorldDesc;
	else
	{
		AtkCollDesc.EffectWorldDesc = CEffectPlayer::EFFECTWORLDDESC();
	}

	XMStoreFloat4(&AtkCollDesc.AtkDir, XMVector3Normalize(vAtkDir));

	CAtkCollManager::GetInstance()->Reuse_Collider(TEXT("Layer_MonsterAtk"), &AtkCollDesc);
}

void CWebManager::Free()
{

}
