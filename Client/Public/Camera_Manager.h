#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CCameraManager final : public CBase
{
	DECLARE_SINGLETON(CCameraManager)

private:
	CCameraManager();
	virtual ~CCameraManager() = default;

public:
	HRESULT Inititalize();
	void Tick(_double dTimeDelta);


// Shake
public:
	void Camera_Shake(const _double& ShakeTime, const _int& iShakePower);

	_bool Get_Is_Shake_On();
	void Set_Is_Shake_On(_bool Is_On);

	_double Get_Shake_Time();
	_int Get_Shake_Power();

	void Set_Is_Shake_Done(_bool Is_Done);
	_bool Get_Is_Shake_Done();


// Focus
public:
	void Focus_On(_vector Pos , _float Time);
	_bool Get_Is_Focus_On();
	_vector Get_Focus_Pos();

// Zoom_In/Out
public:




// Special_View
public:




private: // Shake
	_bool m_Is_Shake_On = { false };
	_bool m_Is_Shake_Done = { false };

	_double	m_ShakeTime = { 0.0 };
	_int	m_iShakePower = { 0 };


private: // Focus
	_bool m_Is_Focus_On = { false };
	_vector m_FocusPos = {};

	_float m_Focus_TimeAcc = { 0.f };
	_float m_Focus_Time = { 0.f };




public:
	virtual void Free() override;
};

END