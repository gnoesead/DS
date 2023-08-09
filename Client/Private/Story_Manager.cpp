#include "pch.h"
#include "..\Public\Story_Manager.h"

#include "GameInstance.h"
#include "Player.h"
#include "..\Public\Title_Manager.h"

IMPLEMENT_SINGLETON(CStoryManager)

CStoryManager::CStoryManager()
{
}

HRESULT CStoryManager::Inititalize()
{
	return S_OK;
}

void CStoryManager::Tick()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (pGameInstance->Get_DIKeyDown(DIK_LEFT)) {

		if (m_Select >= 1)
			m_Select--;

		if (m_Select <= 0) {
			m_Select = 0;
		}
	}

	if (pGameInstance->Get_DIKeyDown(DIK_RIGHT)) {
		m_Select++;

		if (m_Select >= 4) {
			m_Select = 4;
		}
	}

	Safe_Release(pGameInstance);
}

void CStoryManager::Free()
{
}

void CStoryManager::Set_Select_Type(_uint Type)
{
	m_Select = Type;

}

_uint CStoryManager::Get_Select_Type()
{
	return m_Select;
}
