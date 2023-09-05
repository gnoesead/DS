#pragma once

#include "Client_Defines.h"
#include "Character.h"

BEGIN(Engine)

END

BEGIN(Client)

class CPlayer : public CCharacter
{
public:
	enum PLAYERSTATE { PLAYER_ADVENTURE, PLAYER_BATTLE, PLAYER_END };
	enum PLAYERTYPE { PLAYER_TANJIRO, PLAYER_ZENITSU, PLAYER_RENGOKU, PLAYERTYPE_END };

	typedef struct tagPlayerMoveset
	{
		//�Է� ����
		_float4	 m_Input_Dir = { 0.f, 0.f, 0.f, 0.f };

		//���� ���� �ľ�  0:�븻, 1:����, 2:��������
		_int	m_iAwaken = { 0 };
		_double		m_dTime_Awaken_Duration = { 10.0 };

		// �ǰ�ó��
		_bool	m_Down_Dmg_Small = { false };
		_bool	m_Down_Dmg_ConnectSmall = { false };
		_bool	m_Down_Dmg_Big = { false };
		_bool	m_Down_Dmg_Blow = { false };
		_bool	m_Down_Dmg_BigBlow = { false };
		_bool	m_Down_Dmg_Upper = { false };
		_bool	m_Down_Dmg_Spin = { false };
		_bool	m_Down_Dmg_Bound = { false };
		_bool	m_Down_Dmg_Great_Blow = { false };
		_bool	m_Down_Dmg_Swamp = { false };
		_bool	m_Down_Dmg_Web = { false };

		// ����
		_bool	m_isRestrict_KeyInput = { false };
		_bool	m_isRestrict_Move = { false };
		_bool	m_isRestrict_Jump = { false };
		_bool	m_isRestrict_JumpCombo = { false };
		_bool	m_isRestrict_Throw = { false };
		_bool	m_isRestrict_Charge = { false };
		_bool	m_isRestrict_Dash = { false };
		_bool	m_isRestrict_Step = { false };
		_bool	m_isRestrict_DoubleStep = { false };
		_bool	m_isRestrict_Special = { false };

		_bool	m_isRestrict_Adventure = { false };

		//��Ʈ��� ����
		_bool	m_isHitMotion = { false };
		_bool	m_isDownMotion = { false };
		_bool	m_isGetUpMotion = { false };

		//��� Ű��ǲ
		_bool	m_Down_GetUp_Move = { false };
		_bool	m_Down_GetUp = { false };

		//ü���� Ű��ǲ
		_bool	m_Down_PlayerChange = { false };

		//�޸��� Ű��ǲ
		_bool	m_State_Battle_Run = { false };
		_bool	m_Down_Battle_Run = { false };
		_bool	m_Up_Battle_Run = { false };

		//���� Ű��ǲ
		_bool	m_Down_Battle_Jump = { false };
		_bool	m_Down_Battle_JumpMove = { false };

		//���� ���� Ű��ǲ
		_bool	m_Down_Battle_Jump_Attack = { false };
		_bool	m_Down_Battle_Jump_TrackAttack = { false };

		//�޺����� Ű��ǲ
		_bool	m_Down_Battle_Combo = { false };
		_bool	m_Down_Battle_Combo_Up = { false };
		_bool	m_Down_Battle_Combo_Down = { false };
		_bool	m_isPressing_While_Restrict = { false };
		_bool	m_Down_Battle_Combo_Surge = { false };

		//���� ���� Ű��ǲ
		_bool	m_Down_Battle_Charge = { false };
		_bool	m_Up_Battle_Charge = { false };

		//��ų���� Ű��ǲ
		_bool	m_Down_Skill_Normal = { false };
		_bool	m_Down_Skill_Move = { false };
		_bool	m_Down_Skill_Guard = { false };

		//���� Ű��ǲ
		_bool	m_Down_Battle_Guard = { false };
		_bool	m_State_Battle_Guard = { false };
		_bool	m_Up_Battle_Guard = { false };

		//������ Ű��ǲ
		_bool	m_Down_Battle_Throw = { false };
		//��ġ�� Ű��ǲ
		_bool	m_Down_Battle_Push = { false };


		// ��� Ű��ǲ
		_bool	m_Down_Battle_Dash = { false };

		// ���� Ű��ǲ
		_bool	m_Down_Battle_Step = { false };

		// ���� Ű��ǲ
		_bool	m_Down_Battle_Awaken = { false };

		//����� ���� Ű��ǲ 
		_bool	m_Down_Battle_Special = { false };

		//��庥ó ���
		_bool	m_Down_ADV_Jump_To_Object = { false };

	}PLAYERMOVESET;

	typedef struct tagBoxJump
	{
		_float4		BoxPos;
		_bool		RoofOn = { false };
		_float4		Dir_SecondJump = { 0.0f, 1.0f ,0.0f, 0.0f };
		NAVI_TYPE	eNextNavi = { NAVI_END };
		NAVI_TYPE	eNextNavi2 = { NAVI_END };
	}BOXJUMP;

protected:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth();


public:
	PLAYERMOVESET Get_Moveset() {
		return m_Moveset;
	}

public:
	void	Dir_Setting(_bool Reverse);
	_bool	Get_LockOn_MonPos();
	_float	Get_Distance_To_LockOnPos();
	_vector Get_Dir_To_LockOnPos();
	_int	Get_Section() { return m_iSection; }
	_int	Get_Section_Sub() { return m_iSection_Sub; }

	//��Ʈ ���� �ñ׳� Ʈ����
	void	Set_Hit_SurgeCutScene(_bool Hit) { m_isHit_SurgeCutScene = Hit; }
	void	Set_Hit_Success(_bool Hit) { m_isHit_Success = Hit; }
	void	Set_Hit_Success_Hekireki(_bool Hit) { m_isHit_Hekireki = Hit; }

protected: //�ִϸ��̼� ����� �Լ���

	void	Trigger_Hit(_double dTimeDelta);
	void	Key_Input(_double dTimeDelta);

	void	Key_Input_Battle_Move(_double dTimeDelta);
	void	Key_Input_Battle_Jump(_double dTimeDelta);
	void	Key_Input_Battle_Attack(_double dTimeDelta);
	void	Key_Input_Battle_ChargeAttack(_double dTimeDelta);
	void	Key_Input_Battle_Skill(_double dTimeDelta);
	void	Key_Input_Battle_Guard(_double dTimeDelta);
	void	Key_Input_Battle_Dash(_double dTimeDelta);
	void	Key_Input_Battle_Awaken(_double dTimeDelta);
	void	Key_Input_Battle_Special(_double dTimeDelta);
	void	Key_Input_Down(_double dTimeDelta);

	void	Key_Input_Adventure(_double dTimeDelta);

	void	Key_Input_PlayerChange(_double dTimeDelta);
protected:
	void	Add_BoxJump_Info();		// ��ȣ�ۿ�(�ڽ�)(�ȿ��߰�)
	void	Check_Change_Position(_double TimeDelta);	// ĳ���� �̵� (�ȿ��߰�)

	//ĳ�� ����
	void	Player_Change_Setting_Status(_double dTimeDelta);

protected:
	
	_double m_dDelay_Player_Change = { 0.0 };

	_double		m_dDelay_Swapping_Pos = { 0.0 };

	_int	m_iSection = { 1 }; // 1:FirstStreet, 2:SecondStreet, 3:ThirdStreet ,4:Inside
	_int	m_iSection_Sub = { 2 };
	_bool	m_isSection_RoofOn = { false };


	//Level�Ǹ�
	_uint m_iLevelCur = { 0 };

	//��ų����
	_bool	m_isSkilling = { false };

protected:
	PLAYERSTATE		m_ePlayerState = { PLAYER_ADVENTURE };

	//���ҽ�
	_bool		m_isSwap_OnSky = { false };
	
	//���ݽ� �Ÿ�����.
	_float	m_fAtk_Move_Ratio = { 0.7f };
	_float	m_fDmg_Move_Ratio = { 0.65f };
	_float	m_fMoving_Ratio = { 0.85f };

protected: // �ִϸ��̼� ����� ������
	PLAYERMOVESET  m_Moveset;

	_bool	m_isSpecialHit = { false };
	_bool	m_isJump_Move = { false };

	//������� ��Ÿ���� ����
	_bool	m_isCan_Surge = { false };
	_double m_dDelay_Surge = { 0.0 };
	_double m_dDelay_Surge_Attack = { 0.0 };

	_bool	m_isHit_SurgeCutScene = { false };
	_double m_dDelay_SurgeCutScene = { 0.0 };

	//��Ʈ��� �ε��� ����
	_int m_iSmallHit_Index = { 0 };

	//���� ��Ʈ���
	_bool	m_isGuardHit = { false };
	_int	m_iGuardHit_Index = { 0 };

	//hit swamp
	_bool m_isSwampHit = { false };
	_double m_dSwampHit = { 0.0 };
	_bool	m_isSwamp_Escape = { false };


	//��Ÿ�� ����
	_bool		m_isCool_MoveKey = { false };
	_double		m_dTime_MoveKey = { 0.0 };

	//��� ������ 
	_double		m_dDelay_GetUp = { 0.0 };

	//����� 
	_bool	m_isFirst_SpecialReady = { true };

	//�޺� ����
	_bool	m_isComboing = { false };
	_bool		m_isCan_AirDash = { false };
	_bool		m_isAirDashing = { false };
	_double		m_dDelay_Can_AirDash = { 0.0 };

	// ��� ��
	_bool		m_isThrowing = { false };
	_bool		m_isMaintain_Guard = { false };


	//Step ��
	_float4		m_vLook = { 0.0f, 0.0f, 0.0f, 0.0f };
	_bool	m_isForward = { false };
	_bool	m_isBack = { false };
	_bool	m_isLeft = { false };
	_bool	m_isRight = { false };

	//������ ���¿�
	_bool	m_isCan_Air_Hekireki = { false };
	_bool	m_isHit_Hekireki = { false };

	//skill ��
	_double m_dDelay_CanSkill = { 0.0 };

	//Dash
	_double		m_dDelay_Dash = { 0.0 };

protected:
	/* �ӽ� �ڵ� */
	_uint	m_iNumAnim = { 0 };
	// ���� Ȯ�ο�
	_uint	m_iMeshNum = { 0 };

	//��Ʈ��
	//_bool	m_isTestHit = { false };

	_bool	m_isCanNavi = { true };

	//LockOn_MonPos
	_float4		m_LockOnPos = { 0.0f, 0.0f, 0.0f, 1.0f };

	//���� ��ġ
	vector<BOXJUMP> m_vecBoxPos;
	_int	m_iBoxIndex = { 0 };
	_bool	m_isCan_Jump_To_Box = { false };

	_bool	m_isFirst_Jump2_To_Box = { true };

	_bool	m_isCan_Jump_RoofOn = { false };
	_bool	m_isPlayerStatus_OnRoof = { false };

	_float4		m_vPlayerToBoxDir = { 0.0f, 0.0f, 0.0f, 0.0f };
	_float		m_fDistanceTo_Box = { 0.0f };

	_float4		m_Dir_ScondJump_Box = { 0.0f, 0.0f, 0.0f, 0.0f };

	//��Ʋ��ŸƮ
	_bool	m_isBattleStart = { false };
	_double m_dDelay_BattleStart = { 0.0 };

	//Npc
	_bool m_Is_Npc_Near = { false };

protected:
	// Outline Default
	_float	m_fOutlineThickness = 1.5f;
	// Outline Face
	_float	m_fOutlineFaceThickness = 0.6f;

protected:
	// �� �̵� ����(�ȿ�)
	enum CHANGE_POSITONTYPE { CHANGE_POSITON_HOUSE_1A, CHANGE_POSITON_HOUSE_1B, CHANGE_POSITON_HOUSE_2A, CHANGE_POSITON_HOUSE_2B,
							CHANGE_POSITON_VILLAGE_1A, CHANGE_POSITON_VILLAGE_1B, CHANGE_POSITON_END };

	_bool	m_bChangePositionTrigger[CHANGE_POSITON_END] = { false };
	_bool	m_bChangePosition[CHANGE_POSITON_END] = { false };
	_bool	m_bChangePositionFinish[CHANGE_POSITON_END] = { false };

	_double	m_dChangePositionAccTime = { 0.0 };

protected:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END