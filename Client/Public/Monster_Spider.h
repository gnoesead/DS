#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)

END

BEGIN(Client)

class CMonster_Spider final : public CMonster
{
public:
	enum STATE { STATE_IDLE, STATE_ATTACK, STATE_HIT, STATE_DOWN, STATE_END};

	enum ANIM {
		ANIM_ADV_IDLE = 0,
		ANIM_ADV_WALK = 1, ANIM_ADV_WALK_END = 3,
		ANIM_ATK_0 = 4, 
		ANIM_ATK_1 = 6,
		ANIM_JUMP = 9,
		ANIM_IDLE = 12,
		ANIM_RUN = 13,
		ANIM_DMG_SMALL = 14,
		ANIM_DMG_BIG = 15,
		ANIM_BLOW = 16,
		ANIM_DOWN = 19,
		ANIM_RETURN = 20,
		ANIM_END = 21
	};

private:
	CMonster_Spider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_Spider(const CMonster_Spider& rhs);
	virtual ~CMonster_Spider() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth();


private: //애니메이션 제어용 함수
	void	MonsterManaging();

	void	EventCall_Control(_double dTimeDelta);

	void	Trigger();

	void	Animation_Control(_double dTimeDelta);
	void	Animation_Control_Crawling(_double dTimeDelta);
	void	Animation_Control_Idle(_double dTimeDelta);
	void	Animation_Control_Attack(_double dTimeDelta);
	void	Animation_Control_Hit(_double dTimeDelta);
	void	Animation_Control_Down(_double dTimeDelta);

private:
	_float	m_fScale = { 1.f };

	_float	m_fAttack = { 0.0f };


private: //애니메이션 제어용 변수들
	STATE  m_eCurState = { STATE_IDLE };

	//기차 기어오르기
	_bool	m_isSpider_Start = { false };

	_float	m_fCrawlingSpeed = { 1.0f };
	_bool	m_isCrawlingOn = { true };
	_float	m_fCrawling_Y = { 0.0f };
	_double m_dDelay_Crawling = { 0.0 };
	_bool	m_isLeft = { false };

	//Hit
	_bool	m_isUpperHit = { false };


	//Move
	_double		m_dDelay_Spider = { 0.0 };
	_bool	m_isFirst_Spider_1 = { true };
	_bool	m_isFirst_Spider_0 = { true };
	
	_vector m_vPos = { 140.f, 0.f,120.f,1.f };

	//Attack
	_double m_dDelay_ComboChain = { 0.0 };
	
	_double		m_dCoolTime_Attack = { 0.0 };
	_bool	m_isFirst_Attack = { true };
	_double		m_dDelay_Attack = { 0.0 };

	_float4		m_Hit_AtkDir = { 0.0f, 0.0f, 0.0f ,0.0f };


	//Monstermanager
	_bool	m_isFirst_AttackOn = { true };
	_bool	m_isFirst_AttackOff = { false };

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
	static CMonster_Spider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END