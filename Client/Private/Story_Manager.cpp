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

		_tchar szSoundFile[MAX_PATH] = TEXT("UI_Story.ogg");
		CSoundMgr::Get_Instance()->Play_Sound_Channel(szSoundFile, CSoundMgr::UI_CHANGE, 0.55f);

		if (m_Select >= 1)
			m_Select--;

		if (m_Select <= 0) {
			m_Select = 0;
		}
	}

	if (pGameInstance->Get_DIKeyDown(DIK_RIGHT)) {

		_tchar szSoundFile[MAX_PATH] = TEXT("UI_Story.ogg");
		CSoundMgr::Get_Instance()->Play_Sound_Channel(szSoundFile, CSoundMgr::UI_CHANGE, 0.55f);

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

void CStoryManager::Set_Line_Move_Done(_bool b)
{
	m_Line_Move_Done = b;
}

_bool CStoryManager::Get_Line_Move_Done()
{
	return m_Line_Move_Done;
}
