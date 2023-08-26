#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)
END

BEGIN(Client)

class CCamera_Free final : public CCamera
{

	enum CUT_IN_FINISH { TANJIRO_FINISH, ZENITSU_FINISH, AKAZA_FINISH, PLAYER_AWAKE , BOSS_AWAKE , END_FINISH };

	typedef struct Cut_In_Camera_Desc
	{
		_bool bTarget_Is_Player = {};
		_bool bLookTarget_Is_Player = {};
		_float Angle_Hori = {};
		_float Angle_Verti = {};
		_float fDistance = {};
		_float fLifeTime = {};
		_vector vOffSet = {};
		_bool bIs_Lerp = {};

	}CutInCamDesc;


private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& rhs);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual			HRESULT Render() override;


public:
	_float4		Get_CameraLook() { return m_fCameraLook; }
	
	_vector		Get_Battle_Target_Pos() { return m_vBattleTargetPos_Offer; }

	_bool       Get_Is_Battle() { return m_Is_Battle; }
	_bool       Get_Is_Cut_In() { return m_Is_Cut_In; }
	_bool       Get_Lock_On_Is_Boss() { return  m_Lock_On_Is_Boss; }


private:
	void FreeCamera(_double dTimeDelta);
	void AdventureCamera(_double dTimeDelta);
	void NewAdventureCamera(_double dTimeDelta);

	void BattleCamera(_double dTimeDelta);
	void FocusCamera(_double dTimeDelta);
	void SideCamera(_double dTimeDelta);
	void CutInCamera(_double dTimeDelta);
	void CutInFinish(_double dTimeDelta, const CutInCamDesc& Desc);
	void Ready_CutInFinish();

private:
	void Turn_Camera(_double TimeDelta);
	void New_Turn_Camera(_double TimeDelta);
	void LockMouse();

private:
	_bool			m_bCamChange = { true };
	_bool			m_bLockMouse = { true };
	_bool           m_Is_Battle = { false };
	_bool           m_Is_Cut_In = { false };
	_bool           m_Is_Focus_On = { false };
	_bool           m_bIs_Combo_On = { false };
	_bool           m_bIs_LockFree = { false };
	_bool           m_bIs_Battle_LockFree = { false };


	_bool           m_bIs_Pos_Set= { false };

	_bool           m_bIs_Dist_Update = { true };
	_bool           m_bIs_Side_Off = { false };

	_int            m_Hekireki_Dir = { 1 };

	_bool           m_Lock_On_Is_Boss = { false };
private:
	_float4			m_fCameraLook = {};
	_float			m_fDistance = {};
	_vector         m_vOffSet = {};
	_vector         m_vLookOffSet = {};
	_vector         m_vDist = { 0.f, 0.f, -1.f, 0.f };

	_vector         m_vTargetPos = {};
	_vector         m_vPreTargetPos = {};
	_vector         m_vLookPos = {};
	_vector         m_vLerpPos = {};


	_vector         m_vFocusPos = {};
	_vector			m_vMonsterPos = {};
	_vector         m_vBattleTargetPos = {};
	_vector         m_vBattleTargetPos_Offer = {};
	_vector         m_vBattleCenter = {};

	_float          m_vCameraAngle = { 10.f };
	_float          m_vAdventureCameraAngle = { 0.f };
	
	_float          m_fDamping = { 5.f };
	_float          m_fLookDamping = { 7.f };

	_uint           m_Battle_Target_Num = { 0 };
	_int            m_Battle_Target_MaxNum = { 0 };

	_float          m_Zoom = { 0.f };

	_float          m_Village_Cam_TimeAcc = { 0.f };

	_float          m_Swap_TimeAcc = { 2.f };
	_uint           m_Player_Index = { 0 };
	_uint           m_Pre_Player_Index = { 0 };

	_float          m_fLandY = { 0.f };

// Cut_In_Cam	
private:
	vector<CutInCamDesc> m_Cut_In_Finish[END_FINISH];

	_uint m_Cut_In_Finish_Type = { 0 };

	_uint m_Cut_In_Finish_Cam_Num = { 0 };

	_float m_Cut_In_Finish_TimeAcc = { 0.f };

	_bool  m_Cut_In_IsDone = { false };

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END