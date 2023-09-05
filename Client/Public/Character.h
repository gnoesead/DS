#pragma once

#include "Client_Defines.h"
#include "LandObject.h"
#include "Navigation.h"
#include "Transform.h"
#include "Collider.h"

#include "AtkCollider.h"
#include "EffectPlayer.h"
#include "GroundSmoke.h"
#include "StoneParticle.h"

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

	//NPC
	enum NPC_TYPE { NPC_QUEST, NPC_STAND, NPC_TALK, NPC_LISTEN, NPC_WALK, NPC_WALKTALK, NPC_SIT, NPC_SITTALK, NPC_DOWN, NPC_DOWNTALK,
		NPC_WORK, NPC_CRY, NPC_END };
public:
	typedef struct tagNPC
	{
		NPC_TYPE		eNPC = { NPC_END };
		_bool			isNPC_NaviOff = { false };
		_float4			DirNPC = { 0.0f, 0.0f, -1.0f, 0.0f };
		_uint           Icon_Type = { 99 };
		_uint           Dialog_Type = { 99 };
		_bool           Interaction = { false };

		_uint			iSection = { 0 }; // 1:first street , 2:Second Street , 3:inside

		_float4			WalkSpot[4];
	}NPCDESC;

	typedef struct tagCharacterDesc
	{
		CGameObject::WORLDINFO		WorldInfo;
		CCollider::COLLIDERDESC		ColliderDesc[COLL_END];
		CTransform::TRANSFORMDESC	TransformDesc;
		CNavigation::NAVIDESC		NaviDesc;
		_float						Land_Y;
		NAVI_TYPE					eCurNavi;

		NPCDESC				NPCDesc;

		_int	SwampHorn = { 0 };
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
		_float	fSpecial_Save = { 0.0f };

		_int	iAwaken = { 0 };
		_double	dAwaken_Duration = { 15.0 };
		_double	dAwaken_TimeAcc = { 15.0 };
		_bool	isAwaken_First = { true };
		_bool	isNormal_First = { true };

		_float	fSupport = { 100.0f };
		_float	fSupport_Max = { 100.0f };

		_int    iAttackCombo = { 0 };
		_int    iHitCombo = { 0 };

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
	CCollider* Get_ColliderCom() { return m_pColliderCom[CCollider::TYPE_SPHERE]; }
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
	void	Go_Dir_Deceleration(_double dTimeDelta, _int AnimIndex, _float ResetSpeed, _float fDecrease, _float4 Dir, _bool	bIsNaviOff = false);

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
	void	Make_AttackColl(const _tchar* pLayerTag, _float3 Size, _float3 Pos, _double DurationTime, CAtkCollider::ATK_TYPE AtkType, _vector vDir, _float fDmg, CAtkCollider::BULLET_TYPE eBulletType = CAtkCollider::TYPE_DEFAULT);
	void	Make_AtkBulletColl(const _tchar* pLayerTag, _float3 Size, _float3 Pos, _double DurationTime, CAtkCollider::ATK_TYPE AtkType, _vector vAtkDir, _float fDmg, CTransform* pTransform, _double Speed = 5.f, CAtkCollider::BULLET_TYPE eBulletType = CAtkCollider::TYPE_BULLET, const char* pEffectTag = { "" }, CEffectPlayer::EFFECTWORLDDESC* pEffectWorldDesc = nullptr);
	void	Check_HitCollDead();
	void	Check_HitType();


	//스테이터스 hp,mp등등 게이지 관련
	void	Status_Work(_double dTimeDelta);
	void	Use_Mp_Skill(); 
	
	//calculate
	_float4	Calculate_Dir_From_Pos(_float4 Pos);
	_float	Calculate_Distance_From_Pos(_float4 Pos);

protected:
	void	Set_FallingStatus(_float fFallSpeed, _float fGravityAcc) { m_fJump_Acc = -fFallSpeed; m_fGravity_Fall = fGravityAcc; }

protected:	 
	// 네비매쉬 높이설정(안원추가)
	void	Set_Height();

public:
	_float  Get_LandY() {
		return m_fLand_Y;
	}
protected: // 카메라 쉐이크
	void Camera_Shake(_double dShakeTime = 0.5, _uint iShakePower = 100);

protected:
	void Create_GroundSmoke(CGroundSmoke::SMOKE_TYPE eSmokeType, _fvector vOffsetPos = { 0.f,0.f ,0.f,0.f });
	void Create_StoneParticle(_fvector vOffsetPos = { 0.f,0.f ,0.f,0.f });
	void Create_SmeshStone(_fvector vOffsetPos = { 0.f,0.f ,0.f,0.f });

	void Play_HitEffect();
	
protected:
	CHARACTERDESC	m_CharacterDesc;
	list<class CAtkCollider*>	m_HitCollider; 

protected:
	CModel* m_pModelCom = { nullptr };		
	CShader* m_pShaderCom = { nullptr };	
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pColliderCom[CCollider::TYPE_END] = { nullptr };
	CTransform* m_pTransformCom = { nullptr };


protected: //status 게이지 관련
	CHAR_STATUS  m_StatusDesc;

	_bool	m_isCan_Mp_Skill = { true };
	_double m_dDelay_Mp_Used = { 0.0 };

	_bool  m_isHit_Success = { false };

	_double m_dDelay_ComboReset = { 0.0 };
	_double m_dDelay_ComboReset_2 = { 0.0 };


protected:
	_float4		m_Save_RootPos = { 0.0f, 0.0f, 0.0f, 1.0f };

	//navi
	_bool	m_isNavi_Y_Off = { false };

	//First
	_bool	m_isFirst_Anim = { true };

	//Eventcall관련 
	_int m_iPreAnimIndex_ForEvent = { 0 };
	

	//Death
	_double		m_dDelay_Die = { 0.0 };

	//Hit관련
	_bool	m_isBounding = { false };
	_bool	m_isConnectHitting = { false };

	_bool	m_isStrictUpper = { false };
	_bool	m_isNonHitState = { false };

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
	_bool	m_bMotionBlur = { false };
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