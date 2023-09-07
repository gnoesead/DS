#include "pch.h"
#include "..\Public\WebManager.h"

#include "AtkCollManager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CWebManager)

CWebManager::CWebManager()
{
}




void CWebManager::Tick(_double dTimeDelta)
{
}

void CWebManager::Make_WebBullet(const _tchar* pLayerTag, _float3 Size, _float3 Pos, _double DurationTime,
	CAtkCollider::ATK_TYPE AtkType, _vector vAtkDir, _float fDmg, CTransform* pTransform,
	_double Speed, CAtkCollider::BULLET_TYPE eBulletType, const char* pEffectTag, CEffectPlayer::EFFECTWORLDDESC* pEffectWorldDesc)
{
	CAtkCollider::ATKCOLLDESC AtkCollDesc;
	ZeroMemory(&AtkCollDesc, sizeof AtkCollDesc);

	AtkCollDesc.ColliderDesc.vSize = Size;
	AtkCollDesc.ColliderDesc.vPosition = Pos;

	AtkCollDesc.dLifeTime = DurationTime;

	AtkCollDesc.pParentTransform = pTransform;

	AtkCollDesc.eAtkType = AtkType;
	AtkCollDesc.eBulletType = eBulletType;

	AtkCollDesc.fDamage = fDmg;
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

	CAtkCollManager::GetInstance()->Reuse_Collider(pLayerTag, &AtkCollDesc);
}

void CWebManager::Free()
{
	
}
