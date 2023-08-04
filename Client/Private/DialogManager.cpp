#include "pch.h"
#include "..\Public\DialogManager.h"

#include "GameInstance.h"
#include "Player.h"

IMPLEMENT_SINGLETON(CDialogManager)

CDialogManager::CDialogManager()
{
}

HRESULT CDialogManager::Inititalize()
{
	return S_OK;
}

void CDialogManager::Set_Dialog_Type(_uint Type)
{

	m_Dialog_Type = Type;

}

_uint CDialogManager::Get_Dialog_Type()
{
	return m_Dialog_Type;
}



void CDialogManager::Free()
{
}
