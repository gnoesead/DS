#pragma once

#include "Client_Defines.h"
#include "NPC.h"

BEGIN(Engine)

END

BEGIN(Client)

class CNPC_ChildF final : public CNPC
{
public:
	enum ANIM {
		ANIM_CRY = 0,
		ANIM_LISTEN_NORMAL = 1, ANIM_LISTEN_GOMIN = 3,

		ANIM_SIT = 6,  ANIM_SIT_KNEE = 7,

		ANIM_SIT_LISTEN = 8, ANIM_SIT_KNEE_LISTEN = 11,

		ANIM_SIT_SPEAK = 12, ANIM_SIT_KNEE_SPEAK = 15,

		ANIM_SPEAK = 18,
		ANIM_SPEAK_NORLAM = 21,

		ANIM_STAGGER_B = 24,
		ANIM_STAGGER_F = 25,

		ANIM_STAND = 26, ANIM_STAND_GONGSON = 27,

		ANIM_WALK = 28,
		
		ANIM_END = 31
	};

private:
	CNPC_ChildF(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_ChildF(const CNPC_ChildF& rhs);
	virtual ~CNPC_ChildF() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth();


private: //애니메이션 제어용 함수
	void	EventCall_Control(_double dTimeDelta);

	void	Animation_Control(_double dTimeDelta);
	void	Animation_Control_Quest(_double dTimeDelta);
	void	Animation_Control_Stand(_double dTimeDelta);
	void	Animation_Control_Walk(_double dTimeDelta);
	

private:
	void	Sway(_double dTimeDelta);


private: //애니메이션 제어용 변수들
	//뒷걸음
	_bool		m_isFirst_Sway = { true };
	_double		m_isCoolTime_Sway = { 0.0 };
	


	//Move
	_float	m_fMove_Speed = { 2.f };

	_bool	m_isJump_Move = { false };

	_vector m_vPos = { 140.f, 0.f,120.f,1.f };


	//걷기
	_double		m_dCoolTime_Walk = { 0.0 };
	_double		m_dTime_Walking = { 0.0 };


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
	static CNPC_ChildF* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END