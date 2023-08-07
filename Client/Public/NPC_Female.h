#pragma once

#include "Client_Defines.h"
#include "NPC.h"

BEGIN(Engine)

END

BEGIN(Client)

class CNPC_Female final : public CNPC
{
public:
	enum ANIM {
		ANIM_FLOOR_DMG_SIT = 0,
		ANIM_LISTEN_NORMAL = 1, ANIM_LISTEN_NORMAL_END = 3,
		ANIM_LISTE_HARD = 4, ANIM_LISTEN_HARD_END = 6,
		ANIM_SIT_IDLE = 7,
		ANIM_SIT_LISTEN = 9, ANIM_SIT_LISTEN_END = 11,
		ANIM_FLOOR_SIT_IDLE = 12, 
		ANIM_FLOOR_SIT_LISTEN = 14, ANIM_FLOOR_SIT_LISTE_END = 16,
		ANIM_SPEAK_STAND = 17, ANIM_SPEAK_STAND_END = 19,
		ANIM_SPEAK_SHY = 20, ANIM_SPEAK_SHY_END = 22,
		ANIM_SPEAK_HARD = 23,
		ANIM_SPEAK_AHA = 24,
		ANIM_SPEAK_GONGSON = 25,
		ANIM_STAGGER = 26,
		ANIM_STAND_IDLE = 27,
		ANIM_WALK_STANDARD = 32, ANIM_WALK_STANDARD_END =34,
		ANIM_WALK_GONGSON =35, ANIM_WALK_GONGSON_END = 37,
		ANIM_WORK = 38,
		ANIM_END = 39
	};

private:
	CNPC_Female(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_Female(const CNPC_Female& rhs);
	virtual ~CNPC_Female() = default;

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

private:
	


private: //애니메이션 제어용 변수들
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
	static CNPC_Female* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END