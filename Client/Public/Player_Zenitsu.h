#pragma once

#include "Client_Defines.h"
#include "Player.h"

#include "Sword.h"
#include "SwordHome.h"

BEGIN(Engine)

END

BEGIN(Client)

class CPlayer_Zenitsu final : public CPlayer
{
public:
	enum ANIM {
		ANIM_ATK_AIRTRACK = 0,
		ANIM_ATK_COMBO = 3, ANIM_ATK_COMBO_DOWN = 6, ANIM_ATK_COMBO_UP = 8,
		ANIM_ATK_AIRCOMBO = 17,
		ANIM_ATK_CHARGE = 19,
		ANIM_ATK_SKILL_HEKIREKI_AIR = 24, ANIM_ATK_SKILL_HEKIREKI_AIR_END = 25,
		ANIM_ATK_SKILL_HEKIREKI = 26, ANIM_ATK_SKILL_HEKIREKI_END = 27, 
		ANIM_ATK_SKILL_GUARD = 35,
		ANIM_ATK_THROW = 37, ANIM_ATK_THROW_CUTSCENE = 38,
		ANIM_BATTLE_AWAKEN = 39, ANIM_BATTLE_AWAKEN_COMPLETE_CUTSCENE = 40,
		ANIM_BATTLE_IDLE = 41,
		ANIM_BATTLE_DASH = 45,
		ANIM_BATTLE_GUARD = 50, ANIM_BATTLE_GUARD_HIT_BIG = 53, ANIM_BATTLE_GUARD_HIT_SMALL = 54, ANIM_BATTLE_GUARD_PUSH = 55,
		ANIM_BATTLE_JUMP = 56,
		ANIM_JUMP_IDLE = 58,
		ANIM_BATTLE_RUN = 62, ANIM_BATTLE_RUN_END = 63,
		ANIM_BATTLE_STEP_AB = 64, ANIM_BATTLE_STEP_AF = 65, ANIM_BATTLE_STEP_AL = 66, ANIM_BATTLE_STEP_AR = 67,
		ANIM_BATTLE_STEP_B = 68, ANIM_BATTLE_STEP_F = 69, ANIM_BATTLE_STEP_L = 70, ANIM_BATTLE_STEP_R = 72,
		ANIM_DMG_SMALL = 107,
		ANIM_ATK_SPECIAL_CUTSCENE = 117, ANIM_ATK_SPECIAL_READY = 127,
		ANIM_END = 137
	};

private:
	CPlayer_Zenitsu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Zenitsu(const CPlayer_Zenitsu& rhs);
	virtual ~CPlayer_Zenitsu() = default;

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


	vector<_double> m_vecTest;
	
	//벽력일섬용 임시좌표
	_vector m_vBattleTargetPos = { 140.f, 2.f,120.f,1.f }; 
	_bool	m_bGround_Enemy = { true };


private: //애니메이션 제어용 변수들
	//Move
	_float	m_fMove_Speed = { 2.f };

	_bool	m_isJump_Move = { false };

	//평타 용
	_bool	m_isFirst_Combo_Down = { true };
	_bool	m_isComboing_Down = { false };
	_double m_dTime_Comboing_Down = { 0.0 };

	_bool  m_isComboing_Upper = { false };
	_bool	m_isFirst_Combo_Upper = { true };
	_double	 m_dTime_Comboing_Upper = { 0.0 };


	//스킬용 벽력일섬
	_bool		m_isHekireki = { false };
	_double		m_dTime_Hekireki = { 0.0 };
	_bool		m_isHekireki_Hit = { false };
	_double		m_dDelay_Hekireki_Hit = { 0.0 };

	_bool		m_isHekireki_After = { false };
	_double		m_dTime_Hekireki_After = { 0.0 };

	_bool		m_isAir_Hekireki = { false };

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
	static CPlayer_Zenitsu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END