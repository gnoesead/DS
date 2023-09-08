#include "pch.h"
#include "..\Public\WebManager.h"

#include "AtkCollManager.h"

#include "GameInstance.h"

#include "WebShot.h"

#include "Player.h"
#include "PlayerManager.h"

IMPLEMENT_SINGLETON(CWebManager)

CWebManager::CWebManager()
{
}

void CWebManager::Initialize()
{
	//공용 제어용
	m_isFirst_All =  true;
	m_dDelay_All =  0.0;

	//트리거 제어용
	m_iTrigger_Index = 1;

	m_isTrigger_First =  false;
	m_dDelay_First =  0.0;
	m_fLimit_First =  0.0f;

	m_isTrigger_Second =  false;
	m_dDelay_Second =  0.0;

	m_isTrigger_Third =  false;
	m_dDelay_Third = 0.0;

	m_isTrigger_Akaza = false;
	m_dDelay_Akaza = 0.0;
	m_isFirst_Akaza = true;



	m_WebBallPos[0] = { 199.07f, 7.44f, 404.06f, 1.0f };
	m_WebBallPos[1] = { 200.1f, 7.6f, 404.06f, 1.0f };
	m_WebBallPos[2] = { 201.4f, 7.8f, 404.06f, 1.0f };
	m_WebBallPos[3] = { 202.7f, 8.0f, 404.06f, 1.0f };
	m_WebBallPos[4] = { 204.0f, 8.20f, 404.06f, 1.0f };
	m_WebBallPos[5] = { 205.2f, 8.20f, 404.06f, 1.0f };
	m_WebBallPos[6] = { 206.4f, 8.0f, 404.06f, 1.0f };
	m_WebBallPos[7] = { 207.7f, 7.8f, 404.06f, 1.0f };
	m_WebBallPos[8] = { 209.0f, 7.6f, 404.06f, 1.0f };
	m_WebBallPos[9] = { 210.00f, 7.44f, 404.06f, 1.0f };
}

void CWebManager::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), CPlayerManager::GetInstance()->Get_PlayerIndex()));

	m_pTransformCom = pPlayer->Get_TransformCom();

	_float4 PlayerPos;
	XMStoreFloat4(&PlayerPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_float4 Dir = { 0.0f, 0.0f , -1.0f, 0.0f };
	
	Safe_Release(pGameInstance);

#pragma region TriggerSet
	//Trigger onoff
	if (m_iTrigger_Index == 1 && PlayerPos.z >= 263.5f)
	{
		m_iTrigger_Index++;
		m_isTrigger_First = true;

		m_isFirst_All = true;
		m_dDelay_All = 0.0;
	}
	else if (m_iTrigger_Index == 2 && PlayerPos.z >= 297.0f)
	{
		m_iTrigger_Index++;
		m_isTrigger_Second = true;

		m_isFirst_All = true;
		m_dDelay_All = 0.0;
	}
	else if (m_iTrigger_Index == 3 && PlayerPos.z >= 330.5f)
	{
		m_iTrigger_Index++;
		m_isTrigger_Third = true;

		m_isFirst_All = true;
		m_dDelay_All = 0.0;
	}
	//아카자 만남
	else if (m_iTrigger_Index == 4 && PlayerPos.z >= 364.2f)
	{
		m_iTrigger_Index++;
		m_isTrigger_Akaza = true;

		m_isFirst_All = true;
		m_dDelay_All = 0.0;
	}
#pragma endregion

	/*
		PlayerPos.y += 1.5f;
		
		Shoot_ArrowWeb(PlayerPos, Dir);
		Shoot_WebBall();
		Shoot_JikWeb(PlayerPos, Dir, -45.0f);
		Shoot_WideWeb(PlayerPos, Dir);
		
		Shoot_SlideWeb(PlayerPos, Dir, 180.0f);
	*/

#pragma region Event
	m_dDelay_All += dTimeDelta;

	//첫 거미줄 웨이브
	if (m_isTrigger_First)
	{
		if (m_dDelay_All > 10.0f)
		{
			m_dDelay_All = 0.0;
			m_isTrigger_First = false;
		}
		else
		{
			m_dDelay_First += dTimeDelta;
			if (m_fLimit_First < m_dDelay_First)
			{
				m_dDelay_First = 0.0;
				m_fLimit_First = Random::Generate_Float(0.1f, 0.40f);

				//Shoot_WebBall();
				Shoot_JikWeb();
			}
		}
	}
	//둘 거미줄 웨이브
	if (m_isTrigger_Second)
	{
		if (m_dDelay_All > 10.0f)
		{
			m_dDelay_All = 0.0;
			m_isTrigger_First = false;
		}
		else
		{
			m_dDelay_First += dTimeDelta;
			if (m_fLimit_First < m_dDelay_First)
			{
				m_dDelay_First = 0.0;
				m_fLimit_First = Random::Generate_Float(0.1f, 0.40f);

				//Shoot_WebBall();
				Shoot_JikWeb();
			}
		}
	}
	//셋 거미줄 웨이브
	if (m_isTrigger_Third)
	{
		if (m_dDelay_All > 10.0f)
		{
			m_dDelay_All = 0.0;
			m_isTrigger_First = false;
		}
		else
		{
			m_dDelay_First += dTimeDelta;
			if (m_fLimit_First < m_dDelay_First)
			{
				m_dDelay_First = 0.0;
				m_fLimit_First = Random::Generate_Float(0.1f, 0.40f);

				//Shoot_WebBall();
				Shoot_JikWeb();
			}
		}
	}
	//아카자 이벤트
	if (m_isTrigger_Akaza)
	{
		if (m_isFirst_Akaza)
		{
			m_isFirst_Akaza = false;

		}
	}
#pragma endregion

}

void CWebManager::Create_WebShot(_int TexIdx, _float4 CreatePos, _float3 Scale, _float4 ShotDir, _float Speed, _float Turn)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CWebShot::WEBDESC WebDesc;
	WebDesc.iWebTexIndex = TexIdx;
	WebDesc.WorldInfo.vPosition = CreatePos;
	WebDesc.vScale = Scale;
	WebDesc.vDir = ShotDir;
	WebDesc.fTurn = Turn;
	WebDesc.fSpeed = Speed;

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
	Create_WebShot(7, CreatePos, _float3{ 1.5f, 4.5f, 1.5f }, ShotDir, 1.0f);
}

void CWebManager::Shoot_WebBall()
{
	_int		RandomIndex = Random::Generate_Int(0, 9);
	_float		RandomSize = Random::Generate_Float(0.7f, 2.6f);
	_float4		ShotDir = { 0.0f, 0.0f, -1.0f, 0.0f };
	_float		RandomSpeed = Random::Generate_Float(2.0f, 4.0f);

	_int	iRandomTextures = Random::Generate_Int(0, 3);
	_int	iIndexTextures = 28;
	if(iRandomTextures == 0)
		iIndexTextures = 28;
	if (iRandomTextures == 1)
		iIndexTextures = 22;
	if (iRandomTextures == 2)
		iIndexTextures = 19;
	if (iRandomTextures == 3)
		iIndexTextures = 25;

	// index, Pos, Scale, Dir, speed, turn
	Create_WebShot(iIndexTextures, m_WebBallPos[RandomIndex], _float3{ RandomSize, RandomSize, RandomSize }, ShotDir, RandomSpeed);
}

void CWebManager::Shoot_WideWeb()
{
	_float4 PlayerPos;
	XMStoreFloat4(&PlayerPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_float4 CreatePos = { 204.99f, 7.42f, PlayerPos.z + 80.0f, 1.0f };
	_float4 ShotDir = { 0.0f, 0.0f, -1.0f, 0.0f };

	// index, Pos, Scale, Dir, speed, turn
	Create_WebShot(12, CreatePos, _float3{ 20.0f, 6.0f, 6.0f }, ShotDir, 5.0f);
}

void CWebManager::Shoot_JikWeb()
{
	_int		RandomIndex = Random::Generate_Int(0, 9);
	_float4		ShotDir = { 0.0f, 0.0f, -1.0f, 0.0f };

	_int		iRandomTurn_Idx = Random::Generate_Int(0, 5);
	_float		Turn = Random::Generate_Float(-75.f, 75.f);


	_float4 PlayerPos;
	XMStoreFloat4(&PlayerPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	// index, Pos, Scale, Dir, speed, turn
	Create_WebShot(32, _float4{Random::Generate_Float(199.07f, 210.00f), 8.5f, PlayerPos.z + 20.0f, 1.0f}, _float3{ 25.0f, 3.5f, 3.5f }, ShotDir, 1.0f, Turn);
}

void CWebManager::Shoot_SlideWeb(_float4 CreatePos, _float4 ShotDir, _float Turn)
{
	// index, Pos, Scale, Dir, speed, turn
	Create_WebShot(13, CreatePos, _float3{ 17.0f, 8.0f, 6.0f }, ShotDir, 1.0f);
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
