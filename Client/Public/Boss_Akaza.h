#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)

END

BEGIN(Client)

class CBoss_Akaza final : public CMonster
{
public:

	enum  PHASE
	{
		BEGIN, PHASE_1, PHASE_2, PHASE_3
	};
	enum  STATE
	{
		STATE_IDLE, STATE_BEGIN,
		STATE_ESCAPE, STATE_DASHPUNCH, STATE_GUARD, STATE_AIRGUN, STATE_PUSHAWAY, STATE_COMBO_PUNCH,
		STATE_JUMPSTOMP, STATE_DASHKICK, STATE_JUMPAIRGUN, STATE_DASH_COMBO_PUNCH, STATE_UPPERKICK,
		STATE_NEXTPHASE2, STATE_NEXTPHASE3,
		STATE_HEAL,
		STATE_AWAKE, STATE_AWAKE_COMBOPUNCH,

		STATE_NACHIM, STATE_NACHIM_COMBOPUNCH, STATE_NACHIM_AIRGUN,
		STATE_CINEMATIC,

		STATE_HIT_SMALL, STATE_HIT_CONNECTSMALL, STATE_HIT_BIG, STATE_HIT_BLOW, STATE_HIT_BIGBLOW,
		STATE_HIT_BOUND, STATE_SPIN, STATE_GETUP, STATE_BIGGETUP, STATE_ROLLGETUP,
		STATE_HIT_UPPER, STATE_HIT_CUTSCENE


	};

#pragma region AnimIndex
	enum  ANIM {
		////////////////// �⺻ MOVE///////////////////

		ANIM_IDLE = 0,
		ANIM_BATTLE_START = 4, // �ڷ� �ѹ��� ���� ������
		ANIM_BATTLE_END = 1, // 2,3
		ANIM_RUN = 66,
		ANIM_RUN_END = 67,
		ANIM_JUMP_BEHIND = 68, // ����
		ANIM_JUMP_FRONT = 69, // ����
		ANIM_JUMP_LEFT = 70, // ����
		ANIM_JUMP_RIGHT = 71, // ����
		ANIM_STEP_BEHIND = 72, // �޽���
		ANIM_STEP_FRONT = 73, // �ս���
		ANIM_STEP_LEFT = 74, // �½���
		ANIM_STEP_LEFT2 = 75, // �½���2
		ANIM_STEP_RIGHT = 76, // �콺��
		ANIM_STEP_RIGHT2 = 77, // �콺��2
		ANIM_DASH = 38, // �뽬
		ANIM_RUNDASH = 50, // ���� �޸���?
		ANIM_JUMPLOOP = 61, // ��������
		ANIM_JUMPLAND = 44, // ÷������

		////////////////// ��ų ///////////////////////

		ANIM_HEAL = 5, // ü�� 0 �ǰ� �ٽ� ����
		ANIM_CHARGE = 6, // 7,8 //�������
		ANIM_CHARGE_LOOP = 7,
		ANIM_CHARGE_END = 8,
		ANIM_COMBO1 = 12,
		ANIM_COMBO2 = 13,
		ANIM_COMBO3 = 14,
		ANIM_COMBO_DOWN = 15, // �߷� �� �������
		ANIM_COMBO_PIST = 16, // 18 �ָ� �ʹٴٴ� -> COMBO1234
		ANIM_COMBO_UP = 17, // �ϴܹ����� �� �÷�����
		ANIM_COMBO_PIST2 = 18,

		ANIM_AWAKE_COMBO_PIST = 28, // 28~32 ��������? ���� ��

		ANIM_AREA_COMBO = 20, // 21 ���������� �� �ȿ� ������ ����

		ANIM_AIRGUN = 22, // 23,24 ��ǳ
		ANIM_AIRGUN2 = 23,
		ANIM_AIRGUN3 = 24,

		ANIM_JUMPAIRGUN = 25, // 26,27
		ANIM_JUMPAIRGUN2 = 26,
		ANIM_JUMPAIRGUN3 = 27,

		ANIM_NACHIM = 63, // ������ħ

		ANIM_SKILL_UP = 34, // ���۵��� �ö�
		ANIM_SKILL_DOWN = 35, // 34�� ���� �������� ����
		ANIM_SKILL_DOWNEND = 36, // 35�� ���� �������� ���̵鵿�� �̾���

		ANIM_DASH_PUNCH = 39, // �뽬�� �����

		ANIM_SUPERARMOR = 40, //41,42 �ڼ��� ��� �ָ� �ѹ�
		ANIM_SUPERARMOR2 = 41,
		ANIM_SUPERARMOR3 = 42,

		ANIM_LANDING = 44, // 43�� ������� ���� �� ����

		ANIM_AWAKE_PUSHAWAY = 47, // ȸõ

		////////////////// ��Ʈ��� ///////////////////

		ANIM_HIT_BLOW = 79, // 80 �ڷ� ���ư� 
		ANIM_HIT_BLOW_LOOP = 80,
		ANIM_HIT_BLOW_END = 81, // �ڷ� ���ư� ��

		ANIM_HIT_POWER_BLOW = 82, // 105 ������ ���� ����
		ANIM_HIT_MIDDLE_BLOW = 83, // 105 ����
		ANIM_HIT_BOUND = 84, // 85 105 �ٿ��
		ANIM_HIT_BOUND2 = 85, // �ٿ�� �ϰ� ������ ����
		ANIM_HIT_DMGFALL = 86, // U�� ������� ������
		ANIM_HIT_DMGFALL_LOOP = 87, // 86��Ʈ �������� ������ ������
		ANIM_HIT_DMGFALL_BOUND = 88, //? �ٿ�尡 �� �� ���� 
		ANIM_HIT_FALL = 89, // �𸣰��� HIT�� �³�? ���߿��� ��ũ���� ���� IDLE����

		ANIM_HIT_GUARDBREAK = 90, // ����극��ũ -> �������Ʈ��� -> ���Ͻ���
		ANIM_HIT_SPIN = 91, // 92, 93
		ANIM_HIT_POWER_GUARDBREAK = 94, // �ް����� ��
		ANIM_HIT_RETURN_BIG = 95, // ���� �� ���� ���¿��� ���ƿ�
		ANIM_HIT_RETURN_SMALL = 96, // �۰���? ���ƿ�

		ANIM_HIT_SMALL_AIR = 97, // 79,80 C�ڰ��� ������ ���ư�
		ANIM_HIT_AIR_UPPER = 98, // ���� �°� ������

		ANIM_HIT_SMALL = 99, // �������� -> 96�̶� �̾���
		ANIM_HIT_UPPER = 100, // ���� -> ���ƿ��°� 96

		ANIM_HIT_STUN = 101, // 102 ����
		ANIM_HIT_STUN2 = 102, // ����
		ANIM_HIT_GETUP_DIZZY = 103, // 104�� ���� ������ �ٵ� 95�� ������ �´µ� -> ȸõ�̶� �̾ ����

		ANIM_HIT_DOWNLOOP = 105, // ��������
		ANIM_HIT_GETUP_SPIN = 106, // 107
		ANIM_HIT_GETUP_SPIN2 = 107, // ���Ͻ����̶� �̾���

		ANIM_HIT_BACK = 121, // ���� ����� ���� ��
		ANIM_HIT_FRONT = 122,
		ANIM_HIT_LEFT = 123,
		ANIM_HIT_RIGHT = 124,
		ANIM_HIT_SMALLUPPER = 125,
		ANIM_HIT_GETUP = 126, // ó ��������
		ANIM_HIT_GETUP2 = 127,

		ANIM_GUARD_0 = 53,
		ANIM_GUARD_1 = 54,
		ANIM_GUARD_2 = 55,

		ANIM_GUARD_HIT_SMALL = 57,
		ANIM_GUARD_HIT_BIG = 56,


		////////////////// ������ ///////////////////

		////////////////// �ƽŸ�� ///////////////////
		ANIM_AWAKE_COMBO_PIST_CUT = 19, // ���ǰ��� ���� �� ������ ��Ÿ �� �ƽŸ�� ����
		ANIM_FINAL_CUTSCENE = 111, // ���̳� �ƽ� 
		ANIM_AWAKE_START = 48, // ���� �ƽ�
		ANIM_AWAKE_END = 49,

		ANIM_AWAKE_COMBOPUNCH_READY = 28,
		ANIM_AWAKE_COMBOPUNCH_Start = 29,
		ANIM_AWAKE_COMBOPUNCH = 30,
		ANIM_AWAKE_COMBOPUNCH_LOOP = 31,
		ANIM_AWAKE_COMBOPUNCH_END = 32,

		ANIM_CINEMATIC1 = 111,
		ANIM_CINEMATIC2 = 112,
		ANIM_CINEMATIC3 = 113,
		ANIM_CINEMATIC4 = 114,
		ANIM_CINEMATIC5 = 115,
		ANIM_CINEMATIC6 = 116,
		ANIM_CINEMATIC7 = 117,
		ANIM_CINEMATIC8 = 118,
		ANIM_CINEMATIC9 = 119,
		ANIM_CINEMATIC10 = 120,

		ANIM_DEATH = 78, // ���
		ANIM_JUMP_PIST = 33, // �Ⱦ���


	};
#pragma endregion
private:
	CBoss_Akaza(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Akaza(const CBoss_Akaza& rhs);
	virtual ~CBoss_Akaza() = default;
#pragma region Tick
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth();
#pragma endregion
#pragma region Trigger
#ifdef _DEBUG
	void Debug_State(_double dTimeDelta);
#endif //_DEBUG
	void EventCall_Control(_double dTimeDelta);

	void Update_AnimIndex(_uint iAnimIndex);
	void Update_Hit_Messenger(_double dTimeDelta);
	void Update_Trigger(_double dTimeDelta);
	void Update_TriggerTime(_double dTimeDelta);

	//	void Update_Begin(_double dTimeDelta);
	void Update_Phase_1(_double dTimeDelta);
	void Update_Phase_2(_double dTimeDelta);
	void Update_Phase_3(_double dTimeDelta);

	void Trigger_Interact_Phase_1(_double dTimeDelta);
	void Trigger_Interact_Phase_2(_double dTimeDelta);
	void Trigger_Interact_Phase_3(_double dTimeDelta);

	void Trigger_Interact();
	void Trigger_Begin();
	void Trigger_Escape();
	void Trigger_DashPunch();
	void Trigger_Guard();
	void Trigger_AirGun();
	void Trigger_PushAway();
	void Trigger_Nachim();
	void Trigger_ComboPunch();
	void Trigger_JumpStomp();
	void Trigger_DashKick();
	void Trigger_JumpAirGun();
	void Trigger_Dash_ComboPunch();
	void Trigger_UpperKick();
	void Trigger_NextPhase2();
	void Trigger_NextPhase3();
	void Trigger_Heal();
	void Trigger_Awake();
	void Trigger_Awake_ComboPunch();

	void Trigger_Nachim_ComboPunch();
	void Trigger_Nachim_AirGun();
	void Trigger_Awake_Cinematic();

	void Trigger_Hit_Small();
	void Trigger_Hit_ConnectSmall();
	void Trigger_Hit_Upper();
	void Trigger_Hit_Big();
	void Trigger_Hit_Blow();
	void Trigger_Hit_BigBlow();
	void Trigger_Hit_Bound();
	void Trigger_Hit_Spin();
	void Trigger_Hit_CutScene();
	void Trigger_Hit_GetUp();
	void Trigger_Hit_RollGetUp();
	void Trigger_Hit_BigGetUp();

#pragma endregion

#pragma region Pattern
private: //���� �Լ���
	void Update_State(_double dTimeDelta);
	void Update_Interact(_double dTimeDelta);
	void Update_Begin(_double dTimeDelta);
	void Update_Escape(_double dTimeDelta);
	void Update_DashPunch(_double dTimeDelta);
	void Update_Guard(_double dTimeDelta);
	void Update_AirGun(_double dTimeDelta);
	void Update_PushAway(_double dTimeDelta);
	void Update_Nachim(_double dTimeDelta);
	void Update_ComboPunch(_double dTimeDelta);
	void Update_JumpStomp(_double dTimeDelta);
	void Update_DashKick(_double dTimeDelta);
	void Update_JumpAirGun(_double dTimeDelta);
	void Update_Dash_ComboPunch(_double dTimeDelta);
	void Update_UpperKick(_double dTimeDelta);
	void Update_NextPhase2(_double dTimeDelta);
	void Update_NextPhase3(_double dTimeDelta);
	void Update_Heal(_double dTimeDelta);
	void Update_Awake(_double dTimeDelta);
	void Update_Awake_ComboPunch(_double dTimeDelta);

	void Update_Nachim_ComboPunch(_double dTimeDelta);
	void Update_Nachim_AirGun(_double dTimeDelta);
	void Update_Awake_Cinematic(_double dTimeDelta);

	void Update_Hit_Small(_double dTimeDelta);
	void Update_Hit_Upper(_double dTimeDelta);
	void Update_Hit_Big(_double dTimeDelta);
	void Update_Hit_Blow(_double dTimeDelta);
	void Update_Hit_BigBlow(_double dTimeDelta);
	void Update_Hit_Bound(_double dTimeDelta);
	void Update_Hit_Spin(_double dTimeDelta);
	void Update_Hit_CutScene(_double dTimeDelta);
	void Update_Hit_GetUp(_double dTimeDelta);
	void Update_Hit_RollGetUp(_double dTimeDelta);
	void Update_Hit_BigGetUp(_double dTimeDelta);


private: /* Calculate */
	void	Land_Anim_Play(ANIM CurAnim, ANIM LandAnim);



#pragma endregion
private: // _bool
	_bool	m_bStart = { false };
	_bool	m_bDashPunch = { false };
	_bool	m_bCharge = { false };
	_bool	m_bStep_B = { false };
	_bool	m_bRunPunch = { false };
	_bool	m_bDashOn = { false };
	_bool	m_bJump = { false };
	_bool	m_bTrigger = { false };


private: // time
	_double	m_dJumpStompTime = { 0.0 };
	_double m_dTimeAcc = { 0.0 };

private:



private:
	PHASE   m_eCurPhase = PHASE_1;
	STATE	m_eCurstate = STATE_IDLE;
	ANIM    m_eCurAnimIndex = ANIM_IDLE;
	ANIM	m_ePreAnimIndex = ANIM_IDLE;

private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CBoss_Akaza* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END