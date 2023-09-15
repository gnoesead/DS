#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)

END

BEGIN(Client)

class CMonster_Zako final : public CMonster
{
public:
	enum STATE { STATE_IDLE, STATE_ATTACK, STATE_HIT, STATE_DOWN, STATE_END};
	enum PATTERN { PATTERN_CLAWS, PATTERN_TACKLE, PATTERN_SPINKICK, PATTERN_JUMPKICK, 
		PATTERN_BUTTERFLY, PATTERN_CLAWCROSS, 
		PATTERN_SPINMOVE, PATTERN_MOVE,
		PATTERN_END };

	enum ANIM {
		ANIM_ATK_CLAWS = 2, 
		ANIM_ATK_TACKLE = 7, //7~10
		ANIM_ATK_JUMPKICK = 11, // 11~15, 13idle
		ANIM_ATK_SPINKICK = 16, // 16~21
		ANIM_ATK_CLAW_FROG = 22, // 22~26
		ANIM_ATK_BUTTERFLY = 27, //27~32  28뒤로점프,  29공중아이들, 30,31 날아가기
		ANIM_POSE_MUT = 33,
		ANIM_ATK_FROG_TACKLE = 35, //35~37
		ANIM_ATK_CLAW_CROSS = 38,
		ANIM_ATK_SURPRISE_HUG = 40, //40~41
		ANIM_POSE_HOWLING = 42,
		ANIM_ATK_SPINPUNCH = 43, //43~45
		ANIM_JUMP_IDLE = 54,
		ANIM_IDLE = 56,
		ANIM_RUN = 57, //57~58end
		ANIM_STEP_BACK = 59,
		ANIM_STEP_FRONT = 60,
		ANIM_STEP_LEFT = 61,
		ANIM_STEP_RIGHT = 62,
		ANIM_WALK_BACK = 65, //65~67
		ANIM_WALK_FRONT = 68, // 68~70
		ANIM_WALK_LEFT = 71, // 71~73
		ANIM_WALK_RIGHT = 74, // 74~76
		ANIM_DEATH = 79, 
		ANIM_DEATH_IDLE = 98,
		ANIM_DMG_AIR_BACK = 80, ANIM_DMG_AIR_FRONT = 81,
		ANIM_DMG_SMALL_BACK = 82, ANIM_DMG_SMALL_FRONT = 83, 
		ANIM_DMG_SMALL_LEFT = 84, ANIM_DMG_SMALL_RIGHT = 85, ANIM_DMG_SMALL_UPPER = 86,
		ANIM_DMG_BIG_BACK = 87, ANIM_DMG_BIG_FRONT = 88, 
		ANIM_DMG_BIG_G = 89, ANIM_DMG_BIG_UPPER = 90,
		ANIM_DMG_BLOW = 91, // 91~93
		ANIM_DMG_BLOW_GREAT = 94, //94~95
		ANIM_DMG_BOUND = 96, //96~97
		ANIM_DMG_DIZZY = 99, // 99~101
		ANIM_DOWN_IDLE = 102,
		ANIM_DOWN_GETUP_MOVE = 103, //103~104
		ANIM_DOWN_GETUP = 106, //106~109
		ANIM_FALL = 110, //110,  111 fallingidle, 112
		ANIM_DMG_GUARDBRK = 113,
		ANIM_BACKDUMBLING = 114,
		ANIM_DMG_SPIN = 116, //116~117idle 118
		ANIM_DEATH_GETUP = 119,
		ANIM_END = 120
	};

private:
	CMonster_Zako(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Zako(const CMonster_Zako& rhs);
	virtual ~CMonster_Zako() = default;

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

	// 어택패턴 컨트롤러
	void	Idle_ATK_Pattern_Controler(_double dTimeDelta);

	void	Animation_Control_Attack(_double dTimeDelta, _int AttackIndex);
	void	Animation_Control_Attack_Claws(_double dTimeDelta);
	void	Animation_Control_Attack_Tackle(_double dTimeDelta);
	void	Animation_Control_Attack_Spinkick(_double dTimeDelta);
	void	Animation_Control_Attack_Jumpkick(_double dTimeDelta);
	void	Animation_Control_Attack_ButterFly(_double dTimeDelta);
	void	Animation_Control_Attack_Cross(_double dTimeDelta);
	void	Animation_Control_Attack_SpinMove(_double dTimeDelta);

	void	Animation_Control_Hit(_double dTimeDelta);
	void	Animation_Control_Down(_double dTimeDelta);

	//사운드용
	void	Play_Sound_Dmg(_int iType, _double vol); // 0:small, 1:medium, 2:big


private:
	_float	m_fScale = { 0.8f };

	_bool	m_isCan_Tutorial = { false };

	_bool	m_isFirst_BattleOn = { true };

	_bool	m_isFirst_Death_For_Stealth = { true };

private: //애니메이션 제어용 변수들
	STATE  m_eCurState = { STATE_IDLE };
	PATTERN	   m_eCurPattern = { PATTERN_END };
	

	//Moving
	_bool	m_isFirst_Move_0 = { true };
	_bool	m_isFirst_Move_1 = { true };
	_double		m_dDelay_Move = { 0.0 };

	_double		m_dCoolTime_SideMove = { 0.0 };
	_bool		m_isSideMoveOn = { false };
	_bool		m_isSideMove_Left = { false };
	_bool		m_isFirst_SideMove = { true };

	//Attack
	_double m_dCoolTime_AtkPattern = { 0.0 };
	_bool	m_isFirst_AtkPattern = { true };
	_int	m_iAtkPattern_Index = { 9 };


	//Pattern 0 : ANIM_ATK_CLAWS
	_bool	m_isFirst_ATK_0 = { true };
	_bool	m_isFirst_ATK_1 = { true };

	_bool		m_isTime_ATK_0 = { false };
	_double		m_dTime_ATK_0 = { 0.0 };

	//Attackfinish
	_int	m_iAttackIndex = { 0 };
	_bool	m_isAtkFinish = { false };
	_bool	m_isCoolTime_On = { true };


	//Hit_DMg_Combo
	_double		m_dDelay_ComboChain = { 0.0 };
	_int		m_iSmallHit_Index = { 0 };
	

	//hit_Hekireki
	_double		m_dHekireki_Hit = { 0.0 };
	_bool		m_isHekireki_Hit = { false };

	//Down
	_double		m_dDelay_Down = { 0.0 };
	
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
	HRESULT	SetUp_ShaderResources();

public:
	static CMonster_Zako* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END