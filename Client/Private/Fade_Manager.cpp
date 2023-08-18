#include "pch.h"
#include "..\Public\Fade_Manager.h"

#include "GameInstance.h"
#include "Player.h"
#include "..\Public\Fade_Manager.h"


IMPLEMENT_SINGLETON(CFadeManager)

CFadeManager::CFadeManager()
{
}

HRESULT CFadeManager::Inititalize()
{
	return S_OK;

}

void CFadeManager::Tick()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	

	Safe_Release(pGameInstance);
}

void CFadeManager::Free()
{

}


void CFadeManager::Fade_In()
{
	Set_Fade_In(true);
}

void CFadeManager::Fade_Out()
{
	Set_Fade_Out(true);
}

void CFadeManager::Fade_OutIn()
{
	Set_Fade_OutIn(true);
}

void CFadeManager::Fall_Ink()
{

}

void CFadeManager::Set_Fade_In(_bool Fade)
{
	m_Is_Fade_In = Fade;
}

void CFadeManager::Set_Fade_Out(_bool Fade)
{
	m_Is_Fade_Out = Fade;
}

void CFadeManager::Set_Fade_OutIn(_bool Fade, _float Time)
{
	m_Is_Fade_OutIn = Fade;

	if (Time != 0.f)
		m_Delay_Time = Time;
}

void CFadeManager::Set_Ink_In(_bool Fade)
{
	m_Is_Ink_In = Fade;

}

_bool CFadeManager::Get_Fade_In()
{
	return m_Is_Fade_In;
}

_bool CFadeManager::Get_Fade_Out()
{
	return m_Is_Fade_Out;
}

_bool CFadeManager::Get_Fade_OutIn()
{
	return m_Is_Fade_OutIn;
}

_bool CFadeManager::Get_Ink_In()
{
	return m_Is_Ink_In;
}

void CFadeManager::Set_Fade_In_Done(_bool Fade)
{
	m_Is_Fade_In_Done = Fade;
}

void CFadeManager::Set_Fade_Out_Done(_bool Fade)
{
	m_Is_Fade_Out_Done = Fade;
}

void CFadeManager::Set_Fade_OutIn_Done(_bool Fade)
{
	m_Is_Fade_OutIn_Done = Fade;
	
}

void CFadeManager::Set_Ink_In_Done(_bool Fade)
{
	m_Is_Ink_In_Done = Fade;
}

_bool CFadeManager::Get_Fade_In_Done()
{
	return m_Is_Fade_In_Done;
}

_bool CFadeManager::Get_Fade_Out_Done()
{
	return m_Is_Fade_Out_Done;
}

_bool CFadeManager::Get_Fade_OutIn_Done()
{
	return m_Is_Fade_OutIn_Done;
}

_bool CFadeManager::Get_Ink_In_Done()
{
	return m_Is_Ink_In_Done;
}

_bool CFadeManager::Get_Is_Battle()
{
	return m_Is_Battle;
}

void CFadeManager::Set_Is_Battle(_bool is)
{
	m_Is_Battle = is;
}

_float CFadeManager::Get_Delay_Time()
{
	return m_Delay_Time;
}

_bool CFadeManager::Get_Is_House_Boss_On()
{
	return m_Is_House_Boss_On;
}

void CFadeManager::Set_Is_House_Boss_On(_bool is)
{
	m_Is_House_Boss_On = is;
}
void CFadeManager::Set_Fade_Color(_bool Type)
{
	m_Fade_Color = Type;
}

_bool CFadeManager::Get_Fade_Color()
{
	return m_Fade_Color;
}

