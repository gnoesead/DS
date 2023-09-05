#pragma once

#include "Client_Defines.h"
#include "Player.h"

#include "Sword.h"
#include "SwordHome.h"

BEGIN(Engine)

END

BEGIN(Client)

class CPlayer_Rengoku final : public CPlayer
{
public:
	enum ANIM {
		ANIM_ATK_COMBO = 0, // 0~2
		ANIM_ATK_COMBO_DOWN = 3, ANIM_ATK_COMBO_NORMAL = 4, ANIM_ATK_COMBO_UP = 5,
		ANIM_ATK_COMBO_SURGE = 6, ANIM_ATK_COMBO_SURGE_CUTSCENE = 7,
		ANIM_ATK_AIR_COMBO = 8, // 8~9
		ANIM_ATK_CHARGE = 10, // 10~12
		ANIM_ATK_SKILL_GUARD = 13, //13~15
		ANIM_ATK_SKILL_MOVE = 16, //16~17
		ANIM_ATK_SKILL_MOVE_AIR = 18, //18~19
		ANIM_ATK_SKILL_NORMAL = 20, ANIM_ATK_SKILL_NORMAL_AIR = 21,
		ANIM_ATK_AIRTRACK = 24, //24~26
		ANIM_AWAKE = 28, ANIM_AWAKE_COMPLETE = 29,
		ANIM_GUARD_PUSH = 30, ANIM_GUARD_HIT_SMALL = 31, ANIM_GUARD_HIT_BIG = 32,
		ANIM_JUMP = 33, //33~36 35idle
		ANIM_BASE_CHARGE = 37, //37~39
		ANIM_DASH_RUN = 40, //40~42
		ANIM_DMG_BIG_RETURN = 43, ANIM_DMG_SMALL_RETURN = 44,
		ANIM_GUARD = 45, //45~47 46idle
		ANIM_DMG_BLOW = 48, //48~50
		ANIM_DEATH = 51, ANIM_DMG_BLOW_GREAT_0 = 52, ANIM_DMG_BLOW_GREAT_1 = 53,
		ANIM_DMG_BOUND = 54, 
		ANIM_GETUP = 55, // 55~57
		ANIM_DOWN_IDLE = 58,
		ANIM_GETUP_MOVE = 59, // 59~60
		ANIM_DEATH_IDLE = 61, ANIM_GETUP_DEATH = 62,
		ANIM_DEATHMOTION = 68,
		ANIM_DMG_FALL = 70, // 70~72, 71idle
		ANIM_DMG_GUARDBREAK = 74, ANIM_DMG_AIR_BANGLE = 75, ANIM_DMG_AIR_REPEL = 76,
		ANIM_DMG_SPIN = 77, // 77~79
		ANIM_DMG_STRIKE = 80, // 80~81
		ANIM_DMG_UPPER = 82, //82~84
		ANIM_DMG_SMALL_AIR_B = 85, ANIM_DMG_SMALL_AIR_F = 86, ANIM_DMG_SMALL_AIR_L = 87, ANIM_DMG_SMALL_AIR_R = 88, ANIM_DMG_SMALL_AIR_U = 89,
		ANIM_DMG_SMALL_B = 90, ANIM_DMG_SMALL_F = 92, ANIM_DMG_SMALL_L = 93, ANIM_DMG_SMALL_R = 94, ANIM_DMG_SMALL_U = 95,
		ANIM_DMG_BIG = 91,
		ANIM_IDLE = 98,
		ANIM_RUN = 104, ANIM_RUN_END = 105,
		ANIM_SPECIAL_END = 106, ANIM_SPECIAL_READY = 107, // 107~109 109start
		ANIM_SPECIAL_CUTSCENE = 110, // 110~119
		ANIM_STEP_B = 124, ANIM_STEP_F = 125,
		ANIM_STEP_L_0 = 126, ANIM_STEP_L_1 = 127,
		ANIM_STEP_R_0 = 128, ANIM_STEP_R_1 = 129,
		ANIM_VSBATTLE_END = 130, //130~131
		ANIM_VSBATTLE_START = 133,
		ANIM_END = 134
	};

private:
	CPlayer_Rengoku(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Rengoku(const CPlayer_Rengoku& rhs);
	virtual ~CPlayer_Rengoku() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth();


private: //애니메이션 제어용 함수들
	void	EventCall_Control(_double dTimeDelta);

	void	Animation_Control(_double dTimeDelta);
	void	Animation_Control_Battle_Move(_double dTimeDelta);
	void	Animation_Control_Battle_Jump(_double dTimeDelta);
	void	Animation_Control_Battle_Attack(_double dTimeDelta);
	void	Animation_Control_Battle_Charge(_double dTimeDelta);
	void	Animation_Control_Battle_Skill(_double dTimeDelta);
	void	Animation_Control_Battle_Guard(_double dTimeDelta);
	void	Animation_Control_Battle_Dash(_double dTimeDelta);
	void	Animation_Control_Battle_Awaken(_double dTimeDelta);
	void	Animation_Control_Battle_Special(_double dTimeDelta);

	void	Animation_Control_Battle_Dmg(_double dTimeDelta);

	void	Moving_Restrict();


private:
	CSword* m_pSword = { nullptr };
	CSwordHome* m_pSwordHome = { nullptr };

	//스케일 조정
	_float m_fScaleChange = { 0.8f };
	

private: //애니메이션 제어용 변수들
	//Move
	_float	m_fMove_Speed = { 20.0f  }; 

	//skillnormal
	_bool	m_isAir_Skill_Normal = { false };
	_double m_dDelay_Skill_Normal = { 0.0 };
	_bool	m_isAir_Skill_Normal_1 = { false };

	//skillGuar
	_bool	m_isSkill_Guard = { false };
	_double m_dDelay_Skill_Guard = { 0.0 };

	//charge move
	_double m_dDelay_Charge_Move = { 0.0 };

	//오의 용
	_double		m_dTime_Special_Ready = { 0.0 };
	_double		m_dTime_Special_CutScene = { 0.0 };

	//오의 컷씬용
	vector<_double>		m_vecTime;
	_bool		m_isSpecialOn = { false };

	_bool		m_isFirst_Special_Jump = { true };
	_bool		m_isSecond_Special_Jump = { true };
	

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
	static CPlayer_Rengoku* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END