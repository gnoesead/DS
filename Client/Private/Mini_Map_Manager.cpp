#include "pch.h"
#include "..\Public\Mini_Map_Manager.h"

#include "GameInstance.h"
#include "Player.h"
#include "..\Public\Fade_Manager.h"


IMPLEMENT_SINGLETON(CMiniMapManager)

CMiniMapManager::CMiniMapManager()
{
}

HRESULT CMiniMapManager::Inititalize()
{
	return S_OK;

}

void CMiniMapManager::Tick()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	

	Safe_Release(pGameInstance);
}

void CMiniMapManager::Set_MiniMap_X(_float x)
{
	m_MiniMap_X = x;
}

void CMiniMapManager::Set_MiniMap_Y(_float y)
{
	m_MiniMap_Y = y;
}

_float CMiniMapManager::Get_MiniMap_X()
{
	return  m_MiniMap_X;
}

_float CMiniMapManager::Get_MiniMap_Y()
{
	return m_MiniMap_Y;
}

void CMiniMapManager::Free()
{

}



