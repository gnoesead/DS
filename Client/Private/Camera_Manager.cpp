#include "pch.h"
#include "..\Public\Camera_Manager.h"

#include "GameInstance.h"
#include "Player.h"
#include "..\Public\Fade_Manager.h"


IMPLEMENT_SINGLETON(CCameraManager)

CCameraManager::CCameraManager()
{
}

HRESULT CCameraManager::Inititalize()
{
	return S_OK;

}

void CCameraManager::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (m_Is_Focus_On == true) {

		m_Focus_TimeAcc += (_float)dTimeDelta;

		if (m_Focus_TimeAcc > m_Focus_Time) {
			m_Focus_TimeAcc = 0.f;

			m_Is_Focus_On = false;
		}

	}

	
	Safe_Release(pGameInstance);
}

void CCameraManager::Camera_Shake(const _double& ShakeTime, const _int& iShakePower)
{
	m_ShakeTime = ShakeTime;
	m_iShakePower = iShakePower;

	if (m_Is_Shake_On == false)
		m_Is_Shake_On = true;
}

_bool CCameraManager::Get_Is_Shake_On()
{
	return m_Is_Shake_On;
}

void CCameraManager::Set_Is_Shake_On(_bool Is_On)
{
	m_Is_Shake_On = Is_On;
}

_double CCameraManager::Get_Shake_Time()
{
	return m_ShakeTime;
}

_int CCameraManager::Get_Shake_Power()
{
	return m_iShakePower;
}

void CCameraManager::Set_Is_Shake_Done(_bool Is_Done)
{
	m_Is_Shake_Done = Is_Done;
}

_bool CCameraManager::Get_Is_Shake_Done()
{
	return m_Is_Shake_Done;
}

void CCameraManager::Focus_On(_vector Pos, _float Time)
{
	m_Is_Focus_On = true;

	m_FocusPos = Pos;

	m_Focus_Time = Time;
}

_bool CCameraManager::Get_Is_Focus_On()
{
	return m_Is_Focus_On;
}

_vector CCameraManager::Get_Focus_Pos()
{
	return m_FocusPos;
}

void CCameraManager::Free()
{
	
}


