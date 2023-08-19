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

		STATE_HIT_SMALL, STATE_HIT_CONNECTSMALL, STATE_HIT_BIG, STATE_HIT_BLOW, STATE_HIT_BIGBLOW ,
		STATE_HIT_BOUND, STATE_SPIN, STATE_GETUP, STATE_BIGGETUP, STATE_ROLLGETUP,
		STATE_HIT_UPPER, STATE_HIT_CUTSCENE
		

	};

#pragma region AnimIndex
	enum  ANIM {
		////////////////// 기본 MOVE///////////////////

		ANIM_IDEL = 0,
		ANIM_BATTLE_START = 4, // 뒤로 한바퀴 돌고 시작함
		ANIM_BATTLE_END = 1, // 2,3
		ANIM_RUN = 66,
		ANIM_RUN_END = 67,
		ANIM_JUMP_BEHIND = 68, // 뒷점
		ANIM_JUMP_FRONT = 69, // 앞점
		ANIM_JUMP_LEFT = 70, // 좌점
		ANIM_JUMP_RIGHT = 71, // 우점
		ANIM_STEP_BEHIND = 72, // 뒷스텝
		ANIM_STEP_FRONT = 73, // 앞스텝
		ANIM_STEP_LEFT = 74, // 좌스텝
		ANIM_STEP_LEFT2 = 75, // 좌스텝2
		ANIM_STEP_RIGHT = 76, // 우스텝
		ANIM_STEP_RIGHT2 = 77, // 우스텝2
		ANIM_DASH = 38, // 대쉬
		ANIM_RUNDASH = 50, // 개방 달리기?
		ANIM_JUMPLOOP = 61, // 점프루프
		ANIM_JUMPLAND = 44, // 첨프착지

		////////////////// 스킬 ///////////////////////

		ANIM_HEAL = 5, // 체력 0 되고 다시 찰때
		ANIM_CHARGE = 6, // 7,8 //기모으기
		ANIM_CHARGE_LOOP = 7,
		ANIM_CHARGE_END = 8,
		ANIM_COMBO1 = 12,
		ANIM_COMBO2 = 13,
		ANIM_COMBO3 = 14,
		ANIM_COMBO_DOWN = 15, // 발로 땅 내려찍기
		ANIM_COMBO_PIST = 16, // 18 주먹 와다다닥 -> COMBO1234
		ANIM_COMBO_UP = 17, // 하단발차기 후 올려차기
		ANIM_COMBO_PIST2 = 18,

		ANIM_AWAKE_COMBO_PIST = 28, // 28~32 완전개방? 했을 때

		ANIM_AREA_COMBO = 20, // 21 영역펼쳤을 때 안에 있으면 써짐

		ANIM_AIRGUN = 22, // 23,24 장풍
		ANIM_AIRGUN2 = 23,
		ANIM_AIRGUN3 = 24,

		ANIM_JUMPAIRGUN = 25, // 26,27
		ANIM_JUMPAIRGUN2 = 26,
		ANIM_JUMPAIRGUN3 = 27,

		ANIM_NACHIM = 63, // 영역나침

		ANIM_SKILL_UP = 34, // 빙글돌고 올라감
		ANIM_SKILL_DOWN = 35, // 34랑 연계 내려오는 동작
		ANIM_SKILL_DOWNEND = 36, // 35랑 연계 떨어지고 아이들동작 이어짐

		ANIM_DASH_PUNCH = 39, // 대쉬랑 연계기

		ANIM_SUPERARMOR = 40, //41,42 자세딱 잡고 주먹 한방
		ANIM_SUPERARMOR2 = 41,
		ANIM_SUPERARMOR3 = 42,

		ANIM_LANDING = 44, // 43번 발차기라 연결 돼 있음

		ANIM_AWAKE_PUSHAWAY = 47, // 회천

		////////////////// 히트모션 ///////////////////

		ANIM_HIT_BLOW = 79, // 80 뒤로 날아감 
		ANIM_HIT_BLOW_LOOP = 80,
		ANIM_HIT_BLOW_END = 81, // 뒤로 날아감 끝

		ANIM_HIT_POWER_BLOW = 82, // 105 종잇장 마냥 날라감
		ANIM_HIT_MIDDLE_BLOW = 83, // 105 날라감
		ANIM_HIT_BOUND = 84, // 85 105 바운드
		ANIM_HIT_BOUND2 = 85, // 바운드 하고 옆으로 누움
		ANIM_HIT_DMGFALL = 86, // U자 모양으로 떨어짐
		ANIM_HIT_DMGFALL_LOOP = 87, // 86히트 다음으로 나오며 루프임
		ANIM_HIT_DMGFALL_BOUND = 88, //? 바운드가 왜 또 있지 
		ANIM_HIT_FALL = 89, // 모르겟음 HIT가 맞나? 공중에서 웅크리며 있음 IDLE상태

		ANIM_HIT_GUARDBREAK = 90, // 가드브레이크 -> 빅공격히트모션 -> 리턴스몰
		ANIM_HIT_SPIN = 91, // 92, 93
		ANIM_HIT_POWER_GUARDBREAK = 94, // 뒷걸음질 함
		ANIM_HIT_RETURN_BIG = 95, // 반쯤 고개 숙인 상태에서 돌아옴
		ANIM_HIT_RETURN_SMALL = 96, // 작공격? 돌아옴

		ANIM_HIT_SMALL_AIR = 97, // 79,80 C자고역ㄱ 들어오고 날아감
		ANIM_HIT_AIR_UPPER = 98, // 어퍼 맞고 떨어짐

		ANIM_HIT_SMALL = 99, // 스몰공격 -> 96이랑 이어짐
		ANIM_HIT_UPPER = 100, // 어퍼 -> 돌아오는건 96

		ANIM_HIT_STUN = 101, // 102 스턴
		ANIM_HIT_STUN2 = 102, // 스턴
		ANIM_HIT_GETUP_DIZZY = 103, // 104랑 연결 돼있음 근데 95랑 연결이 맞는듯 -> 회천이랑 이어서 쓰셈

		ANIM_HIT_DOWNLOOP = 105, // 누워있음
		ANIM_HIT_GETUP_SPIN = 106, // 107
		ANIM_HIT_GETUP_SPIN2 = 107, // 리턴스몰이랑 이어짐

		ANIM_HIT_BACK = 121, // 누가 뒷통수 때림 ㅋ
		ANIM_HIT_FRONT = 122,
		ANIM_HIT_LEFT = 123,
		ANIM_HIT_RIGHT = 124,
		ANIM_HIT_SMALLUPPER = 125,
		ANIM_HIT_GETUP = 126, // 처 누워있음
		ANIM_HIT_GETUP2 = 127,


		////////////////// 엔드모션 ///////////////////

		////////////////// 컷신모션 ///////////////////
		ANIM_AWAKE_COMBO_PIST_CUT = 19, // 오의개방 했을 때 마지막 평타 때 컷신모드 나옴
		ANIM_FINAL_CUTSCENE = 111, // 파이널 컷신 
		ANIM_AWAKE_START = 48, // 개방 컷신
		ANIM_AWAKE_END = 49,
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

		ANIM_DEATH = 78, // 사망
		ANIM_JUMP_PIST = 33, // 안쓸듯


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
private: //패턴 함수들
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

	_bool	m_bAnimFinish = { false };

	_bool	m_bHit = { false };

	_bool	m_bDashPunch = { false };
	_bool	m_bCharge = { false };
	_bool	m_bStep_B = { false };
	_bool	m_bRunPunch = { false };
	_bool	m_bDashOn = { false };
	_bool	m_bMove = { false };

	_bool	m_bHeal = { false };

	_bool	m_bJump = { false };

	_bool	m_bTrigger = { false };
	_bool	m_bPatternStart = { false };

	_bool	m_bAwake = { false };
	_bool	m_bFirstAwake = { false };
	_bool	m_bSecondAwake = { false };

	_bool	m_bNoDmg = { false };

	//_bool	m_bNextPhase = { false };

private: // time
	_double	m_dJumpStompTime = { 0.0 };
	_double m_dAwakeTime = { 0.0 };
	_double m_dTriggerTime = { 0.0 };
	_double m_dDelay_ComboChain = { 0.0 };

private:
	_uint	m_iRandomDirNum = { 0 };
	_uint	m_iRandomPatternNum = { 0 };
	_uint	m_iPunchCount = { 0 };
	_uint	m_iIdleCnt = { 0 };
	_uint	m_iTriggerCnt = { 0 };
	_uint	m_iSmallHit_Index = { 0 };

private:
	PHASE   m_eCurPhase = PHASE_1;
	STATE	m_eCurstate = STATE_IDLE;
	ANIM    m_eCurAnimIndex = ANIM_IDEL;
	ANIM	m_ePreAnimIndex = ANIM_IDEL;

#pragma region Render
private: // 렌더 아웃라인
	// Outline Default
	_float	m_fOutlineThickness = 0.9f;
	// Outline Face
	_float	m_fOutlineFaceThickness = 0.3f;
	/* 임시 코드 */
	_uint	m_iNumAnim = { 0 };
	// 렌더 확인용
	_uint	m_iMeshNum = { 0 };
#pragma endregion
private:
	//CTransform* m_pPlayerTransformCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CBoss_Akaza* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END