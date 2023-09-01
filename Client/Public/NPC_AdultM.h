#pragma once

#include "Client_Defines.h"
#include "NPC.h"

BEGIN(Engine)

END

BEGIN(Client)

class CNPC_AdultM final : public CNPC
{
public:
	enum ANIM {
		ANIM_BAKSU = 0, ANIM_SLEEP = 1, ANIM_WALLSTAY = 2,

		ANIM_LISTEN_GOMIN = 3, ANIM_LISTEN_NORMAL = 4, ANIM_LISTEN_PALZZANG = 5, 

		ANIM_SIT_LISTEN_NORMAL = 6, ANIM_SIT_LISTEN_PALZZANG = 7, 

		ANIM_SIT_SPEAK_NORMAL = 8, ANIM_SIT_SPEAK_YUPJARI = 9, // 9~11

		ANIM_SIT_KNEE = 12, ANIM_SIT_KNEE_LISTEN = 13, ANIM_SIT_KNEE_SPEAK = 14, // 14~16
		ANIM_SIT_NORMAL = 17, 

		ANIM_SPEAK_GUNBANG = 18, // 18~20
		ANIM_SPEAK_NORMAL = 21, ANIM_SPEAK_PALZZANG = 22, 
		ANIM_SPEAK_YUPJARI = 23, // 23~25

		ANIM_STAND_GONGSON = 26, ANIM_STAND_GUNBANG = 27, ANIM_STNAD_NORMAL = 28, ANIM_STAND_PALZZANG = 29,

		ANIM_WALK_NORMAL = 30, // 30~32
		ANIM_WALK_PALZZANG = 33, // 33~35

		ANIM_WORK = 36,
		ANIM_END = 37
	};

private:
	CNPC_AdultM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC_AdultM(const CNPC_AdultM& rhs);
	virtual ~CNPC_AdultM() = default;

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
	static CNPC_AdultM* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END