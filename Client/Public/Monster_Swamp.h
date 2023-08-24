#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)

END

BEGIN(Client)

class CMonster_Swamp final : public CMonster
{
public:
	enum STATE { STATE_IDLE, STATE_ATTACK, STATE_HIT, STATE_DOWN, STATE_END};
	enum PATTERN { 
		PATTERN_END };

	enum ANIM {
		ANIM_IDLE = 35,
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


	void	Animation_Control_Hit(_double dTimeDelta);
	void	Animation_Control_Down(_double dTimeDelta);

private:
	_float	m_fScale = { 0.8f };


private: //애니메이션 제어용 변수들
	STATE  m_eCurState = { STATE_IDLE };
	PATTERN	   m_eCurPattern = { PATTERN_END };
	
	



private:
	//Hit_DMg_Combo
	_double		m_dDelay_ComboChain = { 0.0 };
	_int		m_iSmallHit_Index = { 0 };
	
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
	static CMonster_Swamp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END