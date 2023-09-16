#include "pch.h"
#include "..\Public\OptionManager.h"

#include "GameInstance.h"
#include "Player.h"

IMPLEMENT_SINGLETON(COptionManager)

COptionManager::COptionManager()
{
}

HRESULT COptionManager::Inititalize()
{
	return S_OK;
}

void COptionManager::Tick(_double dTimeDelta)
{

	if (m_Is_Option_On) {
		m_Option_Move -= (_float)dTimeDelta * 100.f;

		if (m_Option_Move < 0.f) {
			m_Option_Move = 0.f;
			m_Is_Move_Done = true;
		}
	}
	else {
		m_Is_Move_Done = false;

		m_Option_Move += (_float)dTimeDelta * 100.f;

		if (m_Option_Move > 40.f) {
			m_Option_Move = 40.f;
		}
	}

	/*CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_Light(0) != nullptr) {

		if (m_Is_Set_Origin_Light == false) {
			m_Light_Origin = Convert::ToVector(pGameInstance->Get_Light(0)->vLightDiffuse);
			m_Is_Set_Origin_Light = true;
		}
		
		_vector Light = m_Light_Origin;

		Light *= COptionManager::GetInstance()->Get_Graphic_Option(0) / 10.f;

		pGameInstance->Get_Light(0)->vLightDiffuse = Convert::ToFloat4(Light);
	}

	Safe_Release(pGameInstance);*/
	
}

void COptionManager::Set_Is_Option_On(_bool Is)
{
	m_Is_Option_On = Is;
}

_bool COptionManager::Get_Is_Option_On()
{
	return m_Is_Option_On;
}

_bool COptionManager::Get_Is_Move_Done()
{
	return m_Is_Move_Done;
}

_float COptionManager::Get_Option_Move()
{
	return m_Option_Move;
}

void COptionManager::Set_Is_Reset(_bool Is)
{
	m_Is_Reset = Is;
}

_bool COptionManager::Get_Is_Reset()
{

	return m_Is_Reset;
}

void COptionManager::Set_Select_Num(_int num)
{
	m_Select_Num = num;
}

_int COptionManager::Get_Select_Num()
{
	return m_Select_Num;
}

void COptionManager::Set_Is_Go_Lobby(_bool Is)
{
	m_Is_Go_Lobby = Is;
}

_bool COptionManager::Get_Is_Go_Lobby()
{
	return m_Is_Go_Lobby;
}

void COptionManager::Set_Camera_Option(_uint i , _int num)
{
	m_Camera_Option[i] = num;
}

_int COptionManager::Get_Camera_Option(_uint i)
{
	return m_Camera_Option[i];
}

void COptionManager::Set_Graphic_Option(_uint i, _int num)
{
	m_Graphic_Option[i] = num;
}

_int COptionManager::Get_Graphic_Option(_uint i)
{
	return m_Graphic_Option[i];
}

void COptionManager::Set_Sound_Option(_uint i, _int num)
{
	m_Sound_Option[i] = num;
}

_int COptionManager::Get_Sound_Option(_uint i)
{
	return m_Sound_Option[i];
}

void COptionManager::Set_Is_Set_Origin_Light(_bool Is)
{
	m_Is_Set_Origin_Light = Is;
}


void COptionManager::Free()
{
}
