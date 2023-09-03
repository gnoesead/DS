#include "pch.h"
#include "..\Public\MonsterManager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CMonsterManager)

CMonsterManager::CMonsterManager()
{
}




_bool CMonsterManager::Can_Attack()
{
	if (m_iCnt_AttackOn >= 2)
	{
		return false;
	}
	else
	{
		return true;
	}

}


void CMonsterManager::Free()
{
	
}
