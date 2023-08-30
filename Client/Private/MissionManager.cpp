#include "pch.h"
#include "..\Public\MissionManager.h"

#include "GameInstance.h"
#include "Player.h"

IMPLEMENT_SINGLETON(CMissionManager)

CMissionManager::CMissionManager()
{
}

HRESULT CMissionManager::Inititalize()
{
	return S_OK;
}

void CMissionManager::Set_Main_Mission_Type(_uint Type)
{
	m_Main_Mission_Type = Type;
}

_uint CMissionManager::Get_Main_Mission_Type()
{
	return m_Main_Mission_Type;
}

void CMissionManager::Set_Sub_Mission_Type(_uint Type)
{
	m_Sub_Mission_Type = Type;
}

_uint CMissionManager::Get_Sub_Mission_Type()
{
	return m_Sub_Mission_Type;
}

void CMissionManager::Set_Is_Dialog_On(_bool Is_On)
{
	m_Is_Dialog_On = Is_On;
}

_bool CMissionManager::Get_Is_Dialog_On()
{
	return m_Is_Dialog_On;
}

void CMissionManager::Set_Main_Sub_Num(_uint Type)
{
	m_Main_Sub_Num = Type;
}

_uint CMissionManager::Get_Main_Sub_Num()
{
	return m_Main_Sub_Num;
}

void CMissionManager::Set_Sub_Num(_uint Type)
{
	m_Sub_Num = Type;
}

_uint CMissionManager::Get_Sub_Num()
{
	return m_Sub_Num;
}

void CMissionManager::Plus_Main_Sub_Num()
{
	m_Main_Sub_Num++;
}

void CMissionManager::Plus_Sub_Num()
{
	m_Sub_Num++;
}

void CMissionManager::Free()
{
}
