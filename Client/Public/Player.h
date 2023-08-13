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
	typedef struct tagPlayerMoveset
	{
		//입력 방향
		_float4	 m_Input_Dir = { 0.f, 0.f, 0.f, 0.f };

		//각성 상태 파악  0:노말, 1:개방, 2:완전개방
		_int	m_iAwaken = { 0 };
		_double		m_dTime_Awaken_Duration = { 10.0 };

		// 피격처리
		_bool	m_Down_Dmg_Small = { false };
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

		//차지 공격 키인풋
		_bool	m_Down_Battle_Charge = { false };
		_bool	m_State_Battle_Charge = { false };

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


protected: //애니메이션 제어용 함수들
	void	Dir_Setting(_bool Reverse);

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

protected:
	PLAYERSTATE		m_ePlayerState = { PLAYER_BATTLE };

protected: // 애니메이션 제어용 변수들
	PLAYERMOVESET  m_Moveset;

	_bool	m_isSpecialHit = { false };

	//쿨타임 적용
	_bool		m_isCool_MoveKey = { false };
	_double		m_dTime_MoveKey = { 0.0 };

	//기상 딜레이 
	_double		m_dDelay_GetUp = { 0.0 };


	//콤보 도중
	_bool	m_isComboing = { false };

	// 잡기 용
	_bool		m_isThrowing = { false };
	_bool		m_isMaintain_Guard = { false };

	//charge 딜레이
	_double		m_dDelay_Charge_J = { 0.0 };
	_double		m_dDelay_Charge_W = { 0.0 };
	_bool		m_isCan_Charge = { false };
	_bool		m_isCharging = { false };

	//Step 용
	_float4		m_vLook = { 0.0f, 0.0f, 0.0f, 0.0f };
	_bool	m_isForward = { false };
	_bool	m_isBack = { false };
	_bool	m_isLeft = { false };
	_bool	m_isRight = { false };

	//젠이츠 벽력용
	_bool	m_isCan_Air_Hekireki = { false };

protected:
	/* 임시 코드 */
	_uint	m_iNumAnim = { 0 };
	// 렌더 확인용
	_uint	m_iMeshNum = { 0 };

	//히트용
	_bool	m_isTestHit = { false };

	_bool	m_isCanNavi = { true };

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
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END