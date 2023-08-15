#pragma once

#include "Client_Defines.h"
#include "LandObject.h"
#include "Navigation.h"
#include "Transform.h"
#include "Collider.h"

#include "AtkCollider.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
END

BEGIN(Client)

class CCharacter abstract : public CLandObject
{
public:
	enum DIR { DIR_UP, DIR_DOWN, DIR_RIGHT, DIR_LEFT };
	enum COLLIDER { COLL_AABB, COLL_OBB, COLL_SPHERE, COLL_END };
public:
	typedef struct tagCharacterDesc
	{
		CGameObject::WORLDINFO		WorldInfo;
		CCollider::COLLIDERDESC		ColliderDesc[COLL_END];
		CTransform::TRANSFORMDESC	TransformDesc;
		CNavigation::NAVIDESC		NaviDesc;
		_float						Land_Y;
		NAVI_TYPE					eCurNavi;
	}CHARACTERDESC;

	typedef struct tagCharacterStatusDesc
	{
		_float  fHp = { 100.0f };
		_float	fHp_Max = { 100.0f };
		_float	fMp = { 100.0f };
		_float	fMp_Max = { 100.0f };

		_int	iSpecial_Cnt = { 0 };
		_float  fSpecial = { 0.0f };
		_float	fSpecial_Max = { 100.0f };
		_int    iCombo = { 0 };
	}CHAR_STATUS;

protected:
	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

public:
	void Add_HitCollider(CGameObject* pAtkColl);

public:
	//virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CTransform* Get_TransformCom();
	_bool	Get_IsJumpOn() { return m_isJumpOn; }

	// Get_Status
	CHAR_STATUS Get_Status() {
		return m_StatusDesc;
	}


protected:
	HRESULT	Read_Animation_Control_File(const char* szBinfilename);
	void	RootAnimation(_double dTimeDelta);
	_bool	EventCallProcess( );
	void	Reset_Decleration(_float fResetSpeed);

	//이동 관련
	void	Go_Straight_Deceleration(_double dTimeDelta, _int AnimIndex, _float ResetSpeed, _float fDecrease);
	void	Go_Backward_Deceleration(_double dTimeDelta, _int AnimIndex, _float ResetSpeed, _float fDecrease);
	void	Go_Left_Deceleration(_double dTimeDelta, _int AnimIndex, _float ResetSpeed, _float fDecrease);
	void	Go_Right_Deceleration(_double dTimeDelta, _int AnimIndex, _float ResetSpeed, _float fDecrease);
	void	Go_Dir_Deceleration(_double dTimeDelta, _int AnimIndex, _float ResetSpeed, _float fDecrease, _float4 Dir);
	void	Go_Dir_Constant(_double dTimeDelta, _int AnimIndex, _float constantSpeed, _float4 Dir, _bool bIsJumpOn = false);
	void	Go_Straight_Constant(_double dTimeDelta, _int AnimIndex, _float constantSpeed , _bool bIsJumpOn = false);
	void	Go_Backward_Constant(_double dTimeDelta, _int AnimIndex, _float constantSpeed);
	void	Go_Left_Constant(_double dTimeDelta, _int AnimIndex, _float constantSpeed);
	void	Go_Right_Constant(_double dTimeDelta, _int AnimIndex, _float constantSpeed);

	void	Go_Straight_Deceleration_Common(_double dTimeDelta, _float ResetSpeed, _float fDecrease);

	void	Go_Dir_Constant(_double dTimeDelta, DIR Dir, _uint iAnimindex, _float fSpeed, _double dStartRatio = 0.0, _double dEndRatio = 1.0 );


	void	Navigation_To_Ground(_double dTimeDelta);
	void	Gravity(_double dTimeDelta);
	void	Ground_Animation_Play(_int CurAnim, _int GroundAnim);
	void	Jumping( _float ResetSpeed, _float fFallDecrease);
	void	JumpStop(_double dDuration);

	//콜라이더 관련`
	void	Make_AttackColl(const _tchar* pLayerTag, _float3 Size, _float3 Pos, _double DurationTime, CAtkCollider::ATK_TYPE AtkType, _vector vDir, _float fDmg);
	void	Check_HitCollDead();
	void	Check_HitType();


	

protected:
	void	Set_FallingStatus(_float fFallSpeed, _float fGravityAcc) { m_fJump_Acc = -fFallSpeed; m_fGravity_Fall = fGravityAcc; }

protected:	 
	// 네비매쉬 높이설정(안원추가)
	void	Set_Height();
protected: // 카메라 쉐이크
	void Camera_Shake(_double dShakeTime = 0.5, _uint iShakePower = 100);
	
protected:
	CHARACTERDESC	m_CharacterDesc;
	list<class CAtkCollider*>	m_HitCollider; 

protected:
	CModel* m_pModelCom = { nullptr };		
	CShader* m_pShaderCom = { nullptr };	
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pColliderCom[CCollider::TYPE_END] = { nullptr };
	CTransform* m_pTransformCom = { nullptr };


protected:
	CHAR_STATUS  m_StatusDesc;



protected:
	_float4		m_Save_RootPos = { 0.0f, 0.0f, 0.0f, 1.0f };

	

	//Attack MoveControl
	_float	m_fAtk_MoveControl = { 0.0f };
	_bool	m_isReset_Atk_MoveControl = { false };

	//Jump 
	_float	m_fGravity_Fall = { 0.1f};
	_double m_dDelay_Fall = { 0.0 };
	_bool	m_isJumpOn = { false };
	_float	m_fJump_Acc = { 0.0f };
	
	_float	m_fLand_Y = { 0.0f }; // 땅의 y위치임.
	_bool	m_isLand_Roof = { false };

	_bool	m_isJumpStop = { false };
	_double m_dTime_JumpStop = { 0.0 };
	_double	m_dTime_JumpStop_Duration = { 0.0 };

	_bool	m_isFirst_JumpAtk = { true };

	//EventCallIndex
	_int	m_iEvent_Index = { 0 };

protected:
	HRESULT Add_Components();
	void	SetUp_Height();
	void	Tick_Collider(_double dTimeDelta);

public:
	//virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;

};

END