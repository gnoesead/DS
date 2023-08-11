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

	enum ANIM {
		ANIM_ATK_CLAWS = 2, 
		ANIM_ATK_TACKLE = 7, //7~10
		ANIM_ATK_JUMPKICK = 11, // 11~15, 13idle
		ANIM_ATK_SPINKICK = 16, // 16~21
		ANIM_ATK_CLAW_FROG = 22, // 22~26
		ANIM_ATK_BUTTERFLY = 27, //27~32
		ANIM_POSE_MUT = 33,
		ANIM_ATK_FROG_TACKLE = 35, //35~37
		ANIM_ATK_CLAW_CROSS = 38,
		ANIM_ATK_SURPRISE_HUG = 40, //40~41
		ANIM_POSE_HOWLING = 42,
		ANIM_ATK_SPINPUNCH = 43, //43~45
		ANIM_JUMP_IDLE = 54,
		ANIM_IDLE = 56,
		ANIM_RUN = 57, //57~58end
		ANIM_WALK_BACK = 65, //65~67
		ANIM_WALK_FRONT = 68, // 68~70
		ANIM_WALK_LEFT = 71, // 71~73
		ANIM_WALK_RIGHT = 74, // 74~76
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

	void	Animation_Control_Attack(_double dTimeDelta, _int AttackIndex);
	void	Animation_Control_Attack_Claws(_double dTimeDelta);

	void	Animation_Control_Hit(_double dTimeDelta);
	void	Animation_Control_Down(_double dTimeDelta);

private:
	_float	m_fScale = { 0.8f };


private: //애니메이션 제어용 변수들
	STATE  m_eCurState = { STATE_IDLE };
	
	_vector m_vPos = { 140.f, 0.f,120.f,1.f };

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