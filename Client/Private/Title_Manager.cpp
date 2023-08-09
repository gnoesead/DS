#include "pch.h"
#include "..\Public\Title_Manager.h"

#include "GameInstance.h"
#include "Player.h"
#include "..\Public\Title_Manager.h"

IMPLEMENT_SINGLETON(CTitleManager)

CTitleManager::CTitleManager()
{
}

HRESULT CTitleManager::Inititalize()
{
	return S_OK;
}

void CTitleManager::Tick()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (pGameInstance->Get_DIKeyDown(DIK_UP)) {
		m_Select = 0;
	}

	if (pGameInstance->Get_DIKeyDown(DIK_DOWN)) {
		m_Select = 1;
	}

	Safe_Release(pGameInstance);
}

void CTitleManager::Free()
{
}

void CTitleManager::Set_Select_Type(_bool Type)
{
	m_Select = Type;

}

_bool CTitleManager::Get_Select_Type()
{
	return m_Select;
}
