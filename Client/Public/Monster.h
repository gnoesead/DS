#pragma once

#include "Client_Defines.h"
#include "Character.h"

BEGIN(Engine)

END

BEGIN(Client)

class CMonster : public CCharacter
{
public:


protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth();
public:
	void Set_Attack_Success(_bool bHit) { m_bAttack = bHit; }

protected:
	void Get_PlayerComponent();

protected: // Calculate
	void  Calculate_To_Player();

	_bool Check_Distance(_float fDistance);
	_bool Check_Distance_FixY(_float fDistance);
	_bool Check_Player_Y();
	_bool	Event_Time(_double dTimeDelta, _double dTime, _double dTimeAcc) {
		return dTime < dTimeAcc&& dTimeAcc <= dTime + dTimeDelta;
	}

	_vector Calculate_PlayerPos();
	_float Calculate_Distance();
	_vector Calculate_Dir();
	_vector Calculate_Dir_FixY();
	_vector Calculate_Dir_ZeroY();
	_vector Calculate_Dir_Cross();

	_vector Random_Dir(_fvector vDir, _float fMinY = -30.f, _float fMaxY = 30.f, _float fMinX = -20.f, _float fMaxX = 20.f);
	_vector Rotation_Dir(_fvector vDir, _float fAngleX = 0.f, _float fAngleY = 0.f);

	

protected:
	//플레이어 정보 calculate

	_float4		m_PlayerPos;
	_float4		m_Dir_To_Monster;
	_float4		m_Dir_To_Player;
	_float4		m_Dir_To_Player_FixY;


protected:
	_float		m_fDistance_To_Player = { 0.0f };
	_float		m_fSmallDmg = { 1.f };
	_float		m_fBigDmg = { 5.f };
	_float		m_fBlowDmg = { 3.f };
	_float		m_fBigBlowDmg = { 5.f };
	_float		m_fSpinDmg = { 5.f };
	_float		m_fUpperDmg = { 1.f };
	_float		m_fBoundDmg = { 1.f };
	_float		m_fSpeed = { 5.f };


protected: //애니메이션 제어용 함수들
	void	Dir_Setting(_bool Reverse);
	void	Pos_FixY();


protected: // 애니메이션 제어용 변수들
	_bool	m_isDeath_Motion = { false };
	_bool	m_bAir_Motion = { false };
		
	_bool	m_bAttack = { false };
	_bool	m_bStart = { false };
	_bool	m_bHeal = { false };
	_bool	m_bTrigger = { false };
	_bool	m_bPatternStart = { false };
	_bool	m_bAnimFinish = { false };
	_bool	m_bAwake = { false };
	_bool	m_bFirstAwake = { false };
	_bool	m_bSecondAwake = { false };
	_bool	m_bNoDmg = { false };
	_bool	m_bSuperArmor = { false };
	_bool	m_bMove = { false };

protected:
	_double m_dAwakeTime = { 0.0 };
	_double m_dTriggerTime = { 0.0 };


protected:
	_uint	m_iSmallHit_Index = { 0 };
	_uint	m_iHekirekiHit_Index = { 0 };
	_uint	m_iIdleCnt = { 0 };
	_uint	m_iTriggerCnt = { 0 };
	_uint	m_iRandomPatternNum = { 0 };
	_uint	m_iRandomDirNum = { 0 };
	_uint	m_iLoopCount = { 0 };
protected:
	/* 임시 코드 */
	_uint	m_iNumAnim = { 0 };
	// 렌더 확인용
	_uint	m_iMeshNum = { 0 };


protected:
	// Outline Default
	//_float	m_fOutlineThickness = 0.9f;
	_float	m_fOutlineThickness = 2.0f;
	// Outline Face
	//_float	m_fOutlineFaceThickness = 0.3f;
	_float	m_fOutlineFaceThickness = 0.3f;
protected:
	CTransform* m_pPlayerTransformCom = { nullptr };

protected:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END