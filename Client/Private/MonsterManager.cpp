#include "pch.h"
#include "..\Public\MonsterManager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CMonsterManager)

CMonsterManager::CMonsterManager()
{
}

_bool CMonsterManager::Check_Can_Attack()
{
	if (2 < m_iCnt_Attack)
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
