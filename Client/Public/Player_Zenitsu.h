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
		ANIM_DOWN = 98,
		ANIM_FALL = 99, //99, 100 idle, 101 ����
		ANIM_DMG_AIR_CONNECT_0 = 105, ANIM_DMG_AIR_CONNECT_1 = 106,
		ANIM_DMG_SMALL = 107,
		ANIM_DMG_BIG = 82,
		ANIM_DMG_SPIN = 109, //109~111
		ANIM_DMG_BLOW = 84, // 84~86
		ANIM_GETUP = 115, 
		ANIM_DOWN_GETUP_MOVE = 116,
		ANIM_ATK_SPECIAL_CUTSCENE = 117, ANIM_ATK_SPECIAL_READY = 127,
		ANIM_BATTLE_START = 137,
		ANIM_DMG_S_B = 138, ANIM_DMG_S_F = 139, //140,141,142 LRU
		ANIM_DOWN_DEATH_GETUP = 143,
		ANIM_ADV_STEALTH_IDLE = 145,
		ANIM_ADV_STEALTH_WALK = 146, //146~148
		ANIM_ADV_STEALTH_WALK_L180 = 149, ANIM_ADV_STEALTH_WALK_L90 = 150,
		ANIM_ADV_STEALTH_WALK_R180 = 151, ANIM_ADV_STEALTH_WALK_R90 = 152,

		ANIM_END = 153
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


private: //�ִϸ��̼� ����� �Լ���
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

	void	Player_Change(_double dTimeDelta);

	void	Moving_Restrict();


private:
	CSword* m_pSword = { nullptr };
	CSwordHome* m_pSwordHome = { nullptr };

	_float m_fScaleChange = { 0.8f };

	vector<_double> m_vecTest;
	
	//�����ϼ��� �ӽ���ǥ
	_vector m_vBattleTargetPos = { 140.f, 2.f,120.f,1.f }; 

	//�����ϼ� ī�޶��
	_bool m_isHekireki_Camera_On = { false };
	

private: //�ִϸ��̼� ����� ������
	//Move
	_float	m_fMove_Speed = { 2.f };

	

	//��Ÿ ��
	_bool	m_isFirst_Combo_Down = { true };
	_bool	m_isComboing_Down = { false };
	_double m_dTime_Comboing_Down = { 0.0 };

	_bool  m_isComboing_Upper = { false };
	_bool	m_isFirst_Combo_Upper = { true };
	_double	 m_dTime_Comboing_Upper = { 0.0 };


	//��ų�� �����ϼ�
	_bool		m_isHekireki = { false };
	_double		m_dTime_Hekireki = { 0.0 };
	_bool		m_isHekireki_Hitting = { false };
	_double		m_dDelay_Hekireki_Hit = { 0.0 };

	_bool		m_isHekireki_After = { false };
	_double		m_dTime_Hekireki_After = { 0.0 };

	_bool		m_isAir_Hekireki = { false };

	_double		m_dDelay_Hekireki_AnotherCan = { 0.0 };

	_bool	m_isFirst_Hekireki_AirEnd = { false };

	//���� ��
	_double		m_dTime_Special_Ready = { 0.0 };
	_double		m_dTime_Special_CutScene = { 0.0 };

	//���� �ƾ���
	vector<_double>		m_vecTime;
	_bool		m_isSpecialOn = { false };

	_bool		m_isFirst_Special_Jump = { true };
	_bool		m_isSecond_Special_Jump = { true };

private:
	/* �ӽ� �ڵ� */
	_uint	m_iNumAnim = { 0 };
	// ���� Ȯ�ο�
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