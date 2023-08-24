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

			// 바로 해제 X 원래 위치로 보간해서 이동 후 false로 바꿈
			m_Is_Focus_On = false;
		}

	}

	if (m_Is_Zoom_In == true) {

		m_Zoom_TimeAcc += (_float)dTimeDelta;

		if (m_Zoom_TimeAcc > m_Zoom_Time) {
			m_Zoom_TimeAcc = 0.f;
			m_Zoom = 0.f;
			m_Is_Zoom_In = false;
		}

	}

	if (m_Is_Zoom_Out == true) {

		m_Zoom_TimeAcc += (_float)dTimeDelta;

		if (m_Zoom_TimeAcc > m_Zoom_Time) {
			m_Zoom_TimeAcc = 0.f;
			m_Zoom = 0.f;
			m_Is_Zoom_Out = false;
		}

	}


	if (m_Is_Zoom_In == false && m_Is_Zoom_Out == false) {
		m_Zoom = 0.f;
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

void CCameraManager::Zoom_In(_float Zoom, _float Time)
{
	m_Is_Zoom_In = true;
	m_Is_Zoom_Out = false;
	m_Zoom_TimeAcc = 0.f;

	m_Zoom -= Zoom;
	m_Zoom_Time = Time;
}

void CCameraManager::Zoom_Out(_float Zoom, _float Time)
{
	m_Is_Zoom_In = false;
	m_Is_Zoom_Out = true;
	m_Zoom_TimeAcc = 0.f;

	m_Zoom += Zoom;
	m_Zoom_Time = Time;
}

void CCameraManager::Zoom_Fix(_float Zoom)
{
	
	m_Zoom = Zoom;
}



_float CCameraManager::Get_Zoom()
{
	return m_Zoom;
}

void CCameraManager::Set_Is_Battle_LockFree(_bool Is)
{
	m_Is_Battle_LockFree = Is;
}

_bool CCameraManager::Get_Is_Battle_LockFree()
{
	return m_Is_Battle_LockFree;
}

void CCameraManager::Free()
{
	
}


