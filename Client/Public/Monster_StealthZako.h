#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)

END

BEGIN(Client)

class CMonster_StealthZako final : public CMonster
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
		ANIM_ATK_BUTTERFLY = 27, //27~32  28�ڷ�����,  29���߾��̵�, 30,31 ���ư���
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
	CMonster_StealthZako(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_StealthZako(const CMonster_StealthZako& rhs);
	virtual ~CMonster_StealthZako() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth();


private: //�ִϸ��̼� ����� �Լ�
	void	EventCall_Control(_double dTimeDelta);
	void	Calculate_SpotIndex();

	void	Animation_Control(_double dTimeDelta);
	void	Animation_Control_Move(_double dTimeDelta);
	void	Animation_Control_Search(_double dTimeDelta);

private:
	_float	m_fScale = { 0.8f };


	_int	m_iSpot_Index = { 0 };
	

	_double m_dDelay_SpotWalk = { 0.0 };
	_float	m_fTime_Stay = { 3.0f };

	_bool	m_isGo = { false };
	_bool	m_isFirst_Go = { true };


private:
	_bool	m_isQuestioning = { false };
	_bool	m_isFirst_Questioning = { true };
	_double m_dDelay_Questioning = { 0.0 };


	_bool	m_isFinding = { false };
	_bool	m_isFirst_Finding = { true };


	_bool	m_isAttacking = { false };
	_bool	m_isFirst_Attacking = { true };
	_double m_dDelay_Attacking = { 0.0 };


	_bool	m_isPlayerBack = { false };
	_double m_dDelay_PlayerBack = { 0.0 };


	_bool	m_isBattleStart_Stealth = { false };
	_double m_dDelay_BattleStart_Stealth = { 0.0 };

	_bool	m_isDeath_Stealth = { false };
	_double m_dDelay_Dead_Stealth = { 0.0 };
	

private: //�ִϸ��̼� ����� ������
	STATE  m_eCurState = { STATE_IDLE };
	PATTERN	   m_eCurPattern = { PATTERN_END };
	

	
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
	static CMonster_StealthZako* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END