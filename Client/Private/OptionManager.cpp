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


void COptionManager::Free()
{
}
