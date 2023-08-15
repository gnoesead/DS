#include "pch.h"
#include "..\Public\Battle_UI_Manager.h"

#include "GameInstance.h"
#include "Player.h"
#include "..\Public\Title_Manager.h"

IMPLEMENT_SINGLETON(CBattle_UI_Manager)

CBattle_UI_Manager::CBattle_UI_Manager()
{
}

HRESULT CBattle_UI_Manager::Inititalize()
{
	return S_OK;
}

void CBattle_UI_Manager::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (m_Skill_On == true) {
		m_Skill_On_TimeAcc += (_float)dTimeDelta;

		if (m_Skill_On_TimeAcc > 2.5f) {
			m_Skill_On_TimeAcc = 0.f;
			m_Skill_On = false;
		}
	}

	
	

	Safe_Release(pGameInstance);
}

void CBattle_UI_Manager::Free()
{
}

void CBattle_UI_Manager::Set_Player_Type(_uint Type)
{
	m_Player_Index = Type;
}

_uint CBattle_UI_Manager::Get_Player_Type()
{
	return m_Player_Index;
}

void CBattle_UI_Manager::Set_Player_Skill_Type(_uint Type)
{
	m_Skill_On = true;
	m_Skill_On_TimeAcc = 0.f;

	m_Player_Skill_Index = Type;
}

_uint CBattle_UI_Manager::Get_Player_Skill_Type()
{
	return m_Player_Skill_Index;
}

_bool CBattle_UI_Manager::Get_Skill_On()
{
	return m_Skill_On;
}


