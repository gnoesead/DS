#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)

END

BEGIN(Client)

class CMonster_Swamp final : public CMonster
{
public:
	enum STATE { STATE_IDLE, STATE_ATTACK, STATE_HIT, STATE_DOWN, STATE_END };

	enum PATTERN { 
		PATTERN_JUMPSTOMP = 0, PATTERN_SWAMP_SCREW = 1, PATTERN_SWAMP_IN = 2,
		PATTERN_COMBO = 3, PATTERN_SHORYU = 4, PATTERN_TELESHORYU = 5,
		PATTERN_SHOTSWAMP = 6, PATTERN_BIGSWAMP = 7, PATTERN_RAGE_PIOHYO = 8,
		PATTERN_STEP = 9, PATTERN_RAGE_DUDUGE = 10, PATTERN_RAGE_DUDUGE_ATK = 11,
		PATTERN_END 
	};

	enum ANIM {
		ANIM_ANGRY_CUTSCENE = 0, ANIM_BURST = 1,

		ANIM_ATK_SCREW = 2,
		ANIM_ATK_JUMPSTOMP = 3, //3~5
		ANIM_ATK_SWAMP_SCREW = 6, //6~8
		ANIM_SWAMP_GOMIN = 9,
		ANIM_ATK_CROSS = 10,
		ANIM_ATK_COMBO = 11, //11~14, screw8
		ANIM_ATK_SWAMP_SWIM = 15, // 15~17
		ANIM_ATK_SHOT = 18, 
		ANIM_ATK_JUMP_TO_SWAMP = 19, //19,21
		ANIM_ATK_SHORYU_TO_SWAMP_0 = 22, // to jump
		ANIM_ATK_SHORYU_TO_IDLE = 23,
		ANIM_ATK_SHORYU_TO_SWAMP_1 = 24, // to swamp_in

		ANIM_BATTLESTART = 25,
		ANIM_SWAMP_IN = 28,
		ANIM_GUARD = 29, // 29~30, 31guardend
		ANIM_GUARD_HIT = 32,
		ANIM_JUMP_IDLE = 33,
		ANIM_JUMP_GROUND = 34,

		ANIM_IDLE = 35,
		ANIM_SWAMP_IDLE = 36,
		ANIM_SWAMP_IDLE_IN = 37,
		
		ANIM_RUN = 40, ANIM_RUN_END = 41,
		ANIM_STEP_B = 42, ANIM_STEP_F = 43, ANIM_STEP_L = 46, ANIM_STEP_R = 50,

		ANIM_STEP_SWAMPIN = 44,
		ANIM_CROUCH = 45,
		ANIM_STEP_SWAMPOUT = 47,
		ANIM_STEP_SWAMPOUT_AIR = 48,
		
		ANIM_TURN_L_180 = 51, ANIM_TURN_L_90 = 52, ANIM_TURN_R_180 = 53, ANIM_TURN_R_90 = 54,
		ANIM_TURN_L_SWAMP_180 = 55, ANIM_TURN_L_SWAMP_90 = 56, ANIM_TURN_R_SWAMP_180 = 57, ANIM_TURN_R_SWAMP_90 = 58,
		
		ANIM_WALK_B = 59, ANIM_WALK_B_END = 61,
		ANIM_WALK_F = 62, ANIM_WALK_F_END = 64,
		ANIM_WALK_L = 65, ANIM_WALK_L_END = 67,
		ANIM_WALK_R = 68, ANIM_WALK_R_END = 70,

		ANIM_DEATH = 71, 
		ANIM_DEATH_IDLE = 72,
		ANIM_DEATH_GETUP = 73,

		ANIM_DMG_SMALL_F = 78, ANIM_DMG_SMALL_L = 79, ANIM_DMG_SMALL_R = 80, ANIM_DMG_SMALL_U = 81,
		ANIM_DMG_BIG = 82,
		ANIM_DMG_BLOW = 85, // 85~87
		ANIM_DMG_BLOW_GREAT = 88, //88,89
		ANIM_DMG_BOUND = 90, // 이후 fall로 연계
		
		ANIM_DOWN_GETUP = 92, //92~94
		ANIM_DOWN_IDLE = 95,
		ANIM_DOWN_GETUP_MOVE = 96, // 96~97

		ANIM_DMG_FALL = 103, //103~104end
		ANIM_DMG_FALL_END = 105,

		ANIM_DMG_GUARDBRK = 107,
		ANIM_DMG_BINGLEAIR = 108,
		ANIM_DMG_REPEL = 109,
		ANIM_DMG_SPIN = 110, // 110~111
		ANIM_DMG_SPIN_END = 112,
		ANIM_DMG_STRIKE = 113, //113~114
		ANIM_DMG_UPPER = 115, //115~116, 117end

		ANIM_END = 117
	};
private:
	CMonster_Swamp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Swamp(const CMonster_Swamp& rhs);
	virtual ~CMonster_Swamp() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth();


private: //애니메이션 제어용 함수

	void	EventCall_Control(_double dTimeDelta);

	void	Trigger();

	void	Animation_Control(_double dTimeDelta);
	void	Animation_Control_Idle(_double dTimeDelta);


	void	Animation_Control_Attack(_double dTimeDelta, _int AttackIndex);
	void	Navigation_Y_Control(_double dTimeDelta);
	//일반상태
	void	Animation_Control_JumpStomp(_double dTimeDelta);
	void	Animation_Control_Combo(_double dTimeDelta);
	void	Animation_Control_ShotSwamp(_double dTimeDelta);
	void	Animation_Control_Step(_double dTimeDelta);
	//반늪상태
	void	Animation_Control_BigSwamp(_double dTimeDelta);
	void	Animation_Control_Shoryu(_double dTimeDelta);
	void	Animation_Control_Teleport_Shoryu(_double dTimeDelta);
	void	Animation_Control_SwampScrew(_double dTimeDelta);
	void	Animation_Control_Swamp_In(_double dTimeDelta);
	//레이지상태
	void	Animation_Control_Piohyo(_double dTimeDelta);
	void	Animation_Control_Rage_Duduge(_double dTimeDelta);
	void	Animation_Control_Rage_Duduge_Atk(_double dTimeDelta);


	void	Animation_Control_Walk(_double dTimeDelta);
	_bool	Animation_Control_Dash(_double dTimeDelta, _float fDistance);

	void	Animation_Control_Hit(_double dTimeDelta);
	void	Animation_Control_Down(_double dTimeDelta);

private:
	void	Swamp_Create(_int iNumSwamp, _int iType);


private:
	_float	m_fScale = { 0.8f };

	_bool	m_isTeleporting = { false };
	_double m_dDelay_Teleporting = { 0.0 };
	_bool	m_isFirst_Teleporting = { true };

	_bool	m_isFirst_BattleOn = { true };

private: //애니메이션 제어용 변수들
	STATE	m_eCurState = { STATE_IDLE };
	PATTERN	m_eCurPattern = { PATTERN_END };
	_int	m_iPhase = { 0 };
	_bool	m_isSwamping = { false };

	_int	m_iIndex_Normal = { 0 };
	_int	m_iIndex_Swamping = { 0 };
	_int	m_iIndex_SwampShot = { 0 };
	_int	m_iIndex_Rage = { 0 };
	

	//walk
	_bool	m_isFirst_Walk_0 = { true };
	_bool	m_isFirst_Walk_1 = { true };
	_double	m_dDelay_Walk = { 0.0 };
	_bool	m_isWalk_Back = { false };
	
	_bool	m_isFirst_Walk_Side = { true };
	_bool	m_isWalk_Left = { false };
	
private:
	//attack pattern
	_bool	m_isFrist_Atk_Pattern = { true };
	_bool	m_isFirst_Atk_0 = { true };

	_double m_dCooltime_Atk_Pattern = { -1.0 };
	_bool	m_isAtkFinish = { false };
	_float4 m_SaveDir = { 0.0f, 0.0f, 0.0f ,0.0f };
	_double	m_dDelay_Atk = { 0.0 };

	//dash
	_bool	m_isFirst_Dash = { true };
	_bool	m_isOff_Dash = { false };

	//step
	_bool	m_isStepLeft = { true };

	//pattern swamp screw
	_float4	 m_ScrewPos[10];
	_int	m_iScrewPosIndex = { 0 };

	//pattern swamp rage duduge
	_double		m_dDelay_Rage_Duduge = { 0.0 };
	_bool		m_isDuduge_Atk = { false };
	_int		m_iCnt_Duduge = { 0 };

	//Hit_DMg_Combo
	_double		m_dDelay_ComboChain = { 0.0 };
	_int		m_iSmallHit_Index = { 0 };
	
	//Down
	_double		m_dDelay_Down = { 0.0 };

	//Death
	_bool		m_isSwamp_Deathing = { false };
	
private:
	/* 임시 코드 */
	_uint	m_iNumAnim = { 0 };
	// 렌더 확인용
	_uint	m_iMeshNum = { 0 };


private:
	// Outline Default
	_float	m_fOutlineThickness = 0.9f;
	// Outline Face
	_float	m_fOutlineFaceThickness = 0.3f;


private:
	HRESULT Add_Components();
	HRESULT Add_Components_Model();
	HRESULT	SetUp_ShaderResources();

public:
	static CMonster_Swamp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END