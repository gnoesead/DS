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

void CFadeManager::Set_Fade_OutIn(_bool Fade)
{
	m_Is_Fade_OutIn = Fade;
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
