#include "pch.h"
#include "..\Public\PlayerManager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CPlayerManager)

CPlayerManager::CPlayerManager()
{
}





void CPlayerManager::Slow_Acc(_double dTimeDelta)
{
	if (m_isSlow)
	{
		m_dDelay_Slow += dTimeDelta;
		if (m_dDelay_Slow >= m_dLimit_Slow)
		{
			m_dDelay_Slow = 0.0;

			m_isSlow = false;
		}
	}
}

void CPlayerManager::Free()
{
	
}
