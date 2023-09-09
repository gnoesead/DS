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


	if (m_Battle_Start_Time) {
		m_Battle_Start_TimeAcc += (_float)dTimeDelta;

		if (m_Battle_Start_TimeAcc > 1.f) {

			m_Battle_Start_TimeAcc = 0.f;
			m_Battle_Start_Off = true;
			m_Battle_Start_Time = false;
		}

	}

	if (m_Battle_Finish_Time) {
		m_Battle_Finish_TimeAcc += (_float)dTimeDelta;

		if (m_Battle_Finish_TimeAcc > 1.f) {

			m_Battle_Finish_TimeAcc = 0.f;
			m_Battle_Finish_Off = true;
			m_Battle_Finish_Time = false;
		}

	}

	if (m_Battle_Result_Time) {
		m_Battle_Result_TimeAcc += (_float)dTimeDelta;

		if (m_Battle_Result_TimeAcc > 2.f) {

			m_Battle_Result_TimeAcc = 0.f;
			m_Battle_Result_Off = true;
			m_Battle_Result_Time = false;
		}

	}

	if (m_Battle_Result_Time_2) {
		m_Battle_Result_TimeAcc_2 += (_float)dTimeDelta;

		if (m_Battle_Result_TimeAcc_2 > 2.f) {

			m_Battle_Result_TimeAcc_2 = 0.f;
			m_Battle_Result_Off_2 = true;
			m_Battle_Result_Time_2 = false;
		}

	}

	if (m_Timing_Success == true) {

		if (m_Timing_Sprite1 == false) {
			m_Timing_Sprite1 = true;
		}
		if (m_Timing_Sprite2 == false) {
			m_Timing_Sprite2 = true;
		}
		if (m_Timing_Sprite3 == false) {
			m_Timing_Sprite3 = true;
		}
		if (m_Timing_Light == false) {
			m_Timing_Light = true;
		}
	}

	/*if (pGameInstance->Get_DIKeyDown(DIK_NUMPAD6)) {
		m_Timing_On = true;
	}*/


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

void CBattle_UI_Manager::Set_Battle_Start_On(_bool On)
{
	m_Battle_Start_TimeAcc = 0.f;
	m_Battle_Start_Time = true;
	m_Battle_Start_On = On;
}

_bool CBattle_UI_Manager::Get_Battle_Start_On()
{
	return m_Battle_Start_On;
}

void CBattle_UI_Manager::Set_Battle_Start_Off(_bool Off)
{

	m_Battle_Start_Off = Off;

}

_bool CBattle_UI_Manager::Get_Battle_Start_Off()
{
	return m_Battle_Start_Off;
}

void CBattle_UI_Manager::Set_Battle_Finish_On(_bool On)
{
	m_Battle_Finish_TimeAcc = 0.f;
	m_Battle_Finish_Time = true;
	m_Battle_Finish_On = On;
}

_bool CBattle_UI_Manager::Get_Battle_Finish_On()
{
	return m_Battle_Finish_On;
}

void CBattle_UI_Manager::Set_Battle_Finish_Off(_bool Off)
{
	m_Battle_Finish_Off = Off;
}

_bool CBattle_UI_Manager::Get_Battle_Finish_Off()
{
	return m_Battle_Finish_Off;
}

void CBattle_UI_Manager::Set_Battle_Result_On(_bool On)
{
	if (m_Battle_Result_Time == false) {

		m_Battle_Result_TimeAcc = 0.f;
		m_Battle_Result_Time = true;
		m_Battle_Result_On = On;

	}
}

_bool CBattle_UI_Manager::Get_Battle_Result_On()
{
	return m_Battle_Result_On;
}

void CBattle_UI_Manager::Set_Battle_Result_Off(_bool Off)
{
	m_Battle_Result_Off = Off;
}

_bool CBattle_UI_Manager::Get_Battle_Result_Off()
{
	return m_Battle_Result_Off;
}

void CBattle_UI_Manager::Set_Battle_Result_Size_Reset(_bool Is)
{
	m_Battle_Result_Size_Reset = Is;
}

_bool CBattle_UI_Manager::Get_Battle_Result_Size_Reset()
{

	return m_Battle_Result_Size_Reset;
}

void CBattle_UI_Manager::Set_Battle_Result_On_2(_bool On)
{
	if (m_Battle_Result_Time_2 == false) {
		m_Battle_Result_TimeAcc_2 = 0.f;
		m_Battle_Result_Time_2 = true;
		m_Battle_Result_On_2 = On;
	}
}

_bool CBattle_UI_Manager::Get_Battle_Result_On_2()
{
	return m_Battle_Result_On_2;
}

void CBattle_UI_Manager::Set_Battle_Result_Off_2(_bool Off)
{
	m_Battle_Result_Off_2 = Off;
}

_bool CBattle_UI_Manager::Get_Battle_Result_Off_2()
{
	return m_Battle_Result_Off_2;
}

void CBattle_UI_Manager::Set_Zen_UI_Num(_int On)
{
	m_Zen_UI_Num = On;
}

_int CBattle_UI_Manager::Get_Zen_UI_Num()
{
	return m_Zen_UI_Num;
}

void CBattle_UI_Manager::Set_Zen_Elc_On(_bool On)
{
	m_Zen_Elc_On = On;
}

_bool CBattle_UI_Manager::Get_Zen_Elc_On()
{
	return m_Zen_Elc_On;
}

void CBattle_UI_Manager::Set_Zen_Trans_On(_bool On)
{
	m_Zen_Trans_On = On;
}

_bool CBattle_UI_Manager::Get_Zen_Trans_On()
{
	return m_Zen_Trans_On;
}

void CBattle_UI_Manager::Set_Timing_On(_bool On)
{
	m_Timing_On = On;
}

_bool CBattle_UI_Manager::Get_Timing_On()
{
	return m_Timing_On;
}

void CBattle_UI_Manager::Set_Timing_Success(_bool Is)
{
	m_Timing_Success = Is;
}

_bool CBattle_UI_Manager::Get_Timing_Success()
{
	return m_Timing_Success;
}

void CBattle_UI_Manager::Set_Timing_Failed(_bool Is)
{
	m_Timing_Failed = Is;
}

_bool CBattle_UI_Manager::Get_Timing_Failed()
{
	return m_Timing_Failed;
}

void CBattle_UI_Manager::Set_Timing_Sprite1(_bool Is)
{
	m_Timing_Sprite1 = Is;
}

_bool CBattle_UI_Manager::Get_Timing_Sprite1()
{
	return  m_Timing_Sprite1;
}

void CBattle_UI_Manager::Set_Timing_Sprite2(_bool Is)
{
	m_Timing_Sprite2 = Is;
}

_bool CBattle_UI_Manager::Get_Timing_Sprite2()
{
	return m_Timing_Sprite2;
}

void CBattle_UI_Manager::Set_Timing_Sprite3(_bool Is)
{
	m_Timing_Sprite3 = Is;
}

_bool CBattle_UI_Manager::Get_Timing_Sprite3()
{
	return m_Timing_Sprite3;
}

void CBattle_UI_Manager::Set_Timing_Light(_bool Is)
{
	m_Timing_Light = Is;
}

_bool CBattle_UI_Manager::Get_Timing_Light()
{
	return m_Timing_Light;
}



