#pragma once

#include "Client_Defines.h"
#include "Character.h"

BEGIN(Engine)

END

BEGIN(Client)

class CPlayer : public CCharacter
{
public:
	enum PLAYERSTATE { PLAYER_ADVENTURE, PLAYER_BATTLE, PLAYER_END };
	enum PLAYERTYPE { PLAYER_TANJIRO, PLAYER_ZENITSU, PLAYER_RENGOKU, PLAYERTYPE_END };

	typedef struct tagPlayerMoveset
	{
		//입력 방향
		_float4	 m_Input_Dir = { 0.f, 0.f, 0.f, 0.f };

		//각성 상태 파악  0:노말, 1:개방, 2:완전개방
		_int	m_iAwaken = { 0 };
		_double		m_dTime_Awaken_Duration = { 10.0 };

		// 피격처리
		_bool	m_Down_Dmg_Small = { false };
		_bool	m_Down_Dmg_ConnectSmall = { false };
		_bool	m_Down_Dmg_Big = { false };
		_bool	m_Down_Dmg_Blow = { false };
		_bool	m_Down_Dmg_Spin = { false };
		_bool	m_Down_Dmg_Bound = { false };
		_bool	m_Down_Dmg_Great_Blow = { false };

		// 제한
		_bool	m_isRestrict_KeyInput = { false };
		_bool	m_isRestrict_Move = { false };
		_bool	m_isRestrict_Jump = { false };
		_bool	m_isRestrict_JumpCombo = { false };
		_bool	m_isRestrict_Throw = { false };
		_bool	m_isRestrict_Charge = { false };
		_bool	m_isRestrict_Dash = { false };
		_bool	m_isRestrict_Step = { false };
		_bool	m_isRestrict_DoubleStep = { false };
		_bool	m_isRestrict_Special = { false };

		_bool	m_isRestrict_Adventure = { false };

		//히트모션 제한
		_bool	m_isHitMotion = { false };
		_bool	m_isDownMotion = { false };
		_bool	m_isGetUpMotion = { false };

		//기상 키인풋
		_bool	m_Down_GetUp_Move = { false };
		_bool	m_Down_GetUp = { false };


		//달리기 키인풋
		_bool	m_State_Battle_Run = { false };
		_bool	m_Down_Battle_Run = { false };
		_bool	m_Up_Battle_Run = { false };

		//점프 키인풋
		_bool	m_Down_Battle_Jump = { false };
		_bool	m_Down_Battle_JumpMove = { false };

		//점프 공격 키인풋
		_bool	m_Down_Battle_Jump_Attack = { false };
		_bool	m_Down_Battle_Jump_TrackAttack = { false };

		//콤보공격 키인풋
		_bool	m_Down_Battle_Combo = { false };
		_bool	m_Down_Battle_Combo_Up = { false };
		_bool	m_Down_Battle_Combo_Down = { false };
		_bool	m_isPressing_While_Restrict = { false };
		_bool	m_Down_Battle_Combo_Surge = { false };

		//차지 공격 키인풋
		_bool	m_Down_Battle_Charge = { false };
		_bool	m_Up_Battle_Charge = { false };

		//스킬공격 키인풋
		_bool	m_Down_Skill_Normal = { false };
		_bool	m_Down_Skill_Move = { false };
		_bool	m_Down_Skill_Guard = { false };

		//가드 키인풋
		_bool	m_Down_Battle_Guard = { false };
		_bool	m_State_Battle_Guard = { false };
		_bool	m_Up_Battle_Guard = { false };

		//잡기공격 키인풋
		_bool	m_Down_Battle_Throw = { false };
		//밀치기 키인풋
		_bool	m_Down_Battle_Push = { false };


		// 대시 키인풋
		_bool	m_Down_Battle_Dash = { false };

		// 스텝 키인풋
		_bool	m_Down_Battle_Step = { false };

		// 개방 키인풋
		_bool	m_Down_Battle_Awaken = { false };

		//스페셜 오의 키인풋 
		_bool	m_Down_Battle_Special = { false };

		//어드벤처 모드
		_bool	m_Down_ADV_Jump_To_Object = { false };

	}PLAYERMOVESET;

	typedef struct tagBoxJump
	{
		_float4		BoxPos;
		_bool		RoofOn = { false };
		_float4		Dir_SecondJump = { 0.0f, 1.0f ,0.0f, 0.0f };
		NAVI_TYPE	eNextNavi = { NAVI_END };
		NAVI_TYPE	eNextNavi2 = { NAVI_END };
	}BOXJUMP;

protected:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth();


public:
	PLAYERMOVESET Get_Moveset() {
		return m_Moveset;
	}

public:
	void	Dir_Setting(_bool Reverse);
	_bool	Get_LockOn_MonPos();
	_float	Get_Distance_To_LockOnPos();
	_vector Get_Dir_To_LockOnPos();

	//히트 관련 시그널 트리거
	void	Set_Hit_SurgeCutScene(_bool Hit) { m_isHit_SurgeCutScene = Hit; }
	void	Set_Hit_Success(_bool Hit) { m_isHit_Success = Hit; }

protected: //애니메이션 제어용 함수들

	void	Trigger_Hit(_double dTimeDelta);
	void	Key_Input(_double dTimeDelta);

	void	Key_Input_Battle_Move(_double dTimeDelta);
	void	Key_Input_Battle_Jump(_double dTimeDelta);
	void	Key_Input_Battle_Attack(_double dTimeDelta);
	void	Key_Input_Battle_ChargeAttack(_double dTimeDelta);
	void	Key_Input_Battle_Skill(_double dTimeDelta);
	void	Key_Input_Battle_Guard(_double dTimeDelta);
	void	Key_Input_Battle_Dash(_double dTimeDelta);
	void	Key_Input_Battle_Awaken(_double dTimeDelta);
	void	Key_Input_Battle_Special(_double dTimeDelta);
	void	Key_Input_Down(_double dTimeDelta);

	void	Key_Input_Adventure(_double dTimeDelta);

protected:
	void	Add_BoxJump_Info();		// 상호작용(박스)(안원추가)
	void	Check_Change_Position(_double TimeDelta);	// 캐릭터 이동 (안원추가)



protected:
	PLAYERSTATE		m_ePlayerState = { PLAYER_ADVENTURE };
	PLAYERTYPE		m_ePlayerType = { PLAYER_TANJIRO };


protected: // 애니메이션 제어용 변수들
	PLAYERMOVESET  m_Moveset;

	_bool	m_isSpecialHit = { false };

	//각성모드 평타서지 가능
	_bool	m_isCan_Surge = { false };
	_double m_dDelay_Surge = { 0.0 };
	_double m_dDelay_Surge_Attack = { 0.0 };

	_bool	m_isHit_SurgeCutScene = { false };
	_double m_dDelay_SurgeCutScene = { 0.0 };

	//히트모션 인덱스 스몰
	_int m_iSmallHit_Index = { 0 };

	//쿨타임 적용
	_bool		m_isCool_MoveKey = { false };
	_double		m_dTime_MoveKey = { 0.0 };

	//기상 딜레이 
	_double		m_dDelay_GetUp = { 0.0 };

	//스페셜 
	_bool	m_isFirst_SpecialReady = { true };

	//콤보 도중
	_bool	m_isComboing = { false };
	_bool		m_isCan_AirDash = { false };
	_bool		m_isAirDashing = { false };
	_double		m_dDelay_Can_AirDash = { 0.0 };

	// 잡기 용
	_bool		m_isThrowing = { false };
	_bool		m_isMaintain_Guard = { false };

	//charge 딜레이
	_double		m_dDelay_Charge = { 0.0 };

	//Step 용
	_float4		m_vLook = { 0.0f, 0.0f, 0.0f, 0.0f };
	_bool	m_isForward = { false };
	_bool	m_isBack = { false };
	_bool	m_isLeft = { false };
	_bool	m_isRight = { false };

	//젠이츠 벽력용
	_bool	m_isCan_Air_Hekireki = { false };

	//Dash
	_double		m_dDelay_Dash = { 0.0 };

protected:
	/* 임시 코드 */
	_uint	m_iNumAnim = { 0 };
	// 렌더 확인용
	_uint	m_iMeshNum = { 0 };

	//히트용
	_bool	m_isTestHit = { false };

	_bool	m_isCanNavi = { true };

	//LockOn_MonPos
	_float4		m_LockOnPos = { 0.0f, 0.0f, 0.0f, 1.0f };

	//상자 위치
	vector<BOXJUMP> m_vecBoxPos;
	_int	m_iBoxIndex = { 0 };
	_bool	m_isCan_Jump_To_Box = { false };

	_bool	m_isFirst_Jump2_To_Box = { true };

	_bool	m_isCan_Jump_RoofOn = { false };
	_bool	m_isPlayerStatus_OnRoof = { false };

	_float4		m_vPlayerToBoxDir = { 0.0f, 0.0f, 0.0f, 0.0f };
	_float		m_fDistanceTo_Box = { 0.0f };

	_float4		m_Dir_ScondJump_Box = { 0.0f, 0.0f, 0.0f, 0.0f };



protected:
	// Outline Default
	_float	m_fOutlineThickness = 0.9f;
	// Outline Face
	_float	m_fOutlineFaceThickness = 0.3f;

protected:
	// 맵 이동 변수(안원)
	enum CHANGE_POSITONTYPE { CHANGE_POSITON_HOUSE_1A, CHANGE_POSITON_HOUSE_1B, CHANGE_POSITON_HOUSE_2A, CHANGE_POSITON_HOUSE_2B,
							CHANGE_POSITON_VILLAGE_1A, CHANGE_POSITON_VILLAGE_1B, CHANGE_POSITON_END };

	_bool	m_bChangePositionTrigger[CHANGE_POSITON_END] = { false };
	_bool	m_bChangePosition[CHANGE_POSITON_END] = { false };
	_bool	m_bChangePositionFinish[CHANGE_POSITON_END] = { false };

	_double	m_dChangePositionAccTime = { 0.0 };
	

protected:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END