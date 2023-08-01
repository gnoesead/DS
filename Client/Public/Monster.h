//#pragma once
//
//#include "Client_Defines.h"
//#include "Character.h"
//
//BEGIN(Engine)
//
//END
//
//BEGIN(Client)
//
//class CMonster : public CCharacter
//{
//public:
//	typedef struct tagMonsterMoveset
//	{
//		//입력 방향
//		_float4	 m_Input_Dir = { 0.f, 0.f, 0.f, 0.f };
//
//		// 제한
//		_bool	m_isRestrict_KeyInput = { false };
//		_bool	m_isRestrict_Move = { false };
//		_bool	m_isRestrict_Jump = { false };
//		_bool	m_isRestrict_JumpCombo = { false };
//		_bool	m_isRestrict_Throw = { false };
//		_bool	m_isRestrict_Charge = { false };
//		_bool	m_isRestrict_Dash = { false };
//		_bool	m_isRestrict_Step = { false };
//			
//	}MONSTERMOVESET;
//
//protected:
//	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	CMonster(const CMonster& rhs);
//	virtual ~CMonster() = default;
//
//public:
//	virtual HRESULT Initialize_Prototype();
//	virtual HRESULT Initialize(void* pArg) override;
//	virtual void	Tick(_double dTimeDelta) override;
//	virtual void	LateTick(_double dTimeDelta) override;
//	virtual HRESULT Render() override;
//	virtual HRESULT Render_ShadowDepth();
//
//
//protected: //애니메이션 제어용 함수들
//	void	Dir_Setting(_bool Reverse);
//
//	void	Key_Input(_double dTimeDelta);
//	void	Key_Input_Battle_Move(_double dTimeDelta);
//	void	Key_Input_Battle_Jump(_double dTimeDelta);
//	void	Key_Input_Battle_Attack(_double dTimeDelta);
//	void	Key_Input_Battle_ChargeAttack(_double dTimeDelta);
//	void	Key_Input_Battle_Skill(_double dTimeDelta);
//	void	Key_Input_Battle_Guard(_double dTimeDelta);
//	void	Key_Input_Battle_Dash(_double dTimeDelta);
//
//
//
//protected: // 애니메이션 제어용 변수들
//	MONSTERMOVESET  m_Moveset;
//
//	//쿨타임 적용
//	_bool		m_isCool_MoveKey = { false };
//	_double		m_dTime_MoveKey = { 0.0 };
//
//
//	//콤보 도중
//	_bool	m_isComboing = { false };
//
//	// 잡기 용
//	_bool		m_isThrowing = { false };
//	_bool		m_isMaintain_Guard = { false };
//
//	//charge 딜레이
//	_double		m_dDelay_Charge_J = { 0.0 };
//	_double		m_dDelay_Charge_W = { 0.0 };
//	_bool		m_isCan_Charge = { false };
//	_bool		m_isCharging = { false };
//
//	//Step 용
//	_float4		m_vLook = { 0.0f, 0.0f, 0.0f, 0.0f };
//	_bool	m_isForward = { false };
//	_bool	m_isBack = { false };
//	_bool	m_isLeft = { false };
//	_bool	m_isRight = { false };
//
//protected:
//	/* 임시 코드 */
//	_uint	m_iNumAnim = { 0 };
//	// 렌더 확인용
//	_uint	m_iMeshNum = { 0 };
//
//
//protected:
//	// Outline Default
//	_float	m_fOutlineThickness = 0.9f;
//	// Outline Face
//	_float	m_fOutlineFaceThickness = 0.3f;
//
//protected:
//	HRESULT Add_Components();
//	HRESULT	SetUp_ShaderResources();
//
//public:
//	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
//	virtual CGameObject* Clone(void* pArg) override;
//	virtual void Free() override;
//};
//
//END