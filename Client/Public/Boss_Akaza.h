#pragma once

#include "Client_Defines.h"
#include "Character.h"


BEGIN(Engine)

END

BEGIN(Client)

class CBoss_Akaza final : public CCharacter
{
public:
#pragma region STATE
	enum PHASE
	{
		BEGIN, PHASE_1, PHASE_2, PHASE_3
	};
	enum STATE
	{
		STATE_IDLE, STATE_BEGIN,
		STATE_ESCAPE, STATE_DASHPUNCH, STATE_GUARD, STATE_AIRGUN, STATE_PUSHAWAY, STATE_COMBO_PUNCH,
		STATE_JUMPSTOMP, STATE_DASHKICK, STATE_JUMPAIRGUN, STATE_DASH_COMBO_PUNCH, STATE_UPPERKICK,
		STATE_NEXTPHASE2,STATE_NEXTPHASE3,
		STATE_HEAL,
		STATE_AWAKE,
		STATE_NACHIM, STATE_NACHIM_COMBOPUNCH, STATE_NACHIM_AIRGUN
		
	};
#pragma endregion
#pragma region AnimIndex
	enum ANIM {
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
		ANIM_STEP_RIGHT = 76, // 우스텝
		ANIM_STEP_LEFT2 = 75, // 좌스텝2
		ANIM_STEP_RIGHT2 = 77, // 우스텝2

		ANIM_DASH = 38, // 대쉬

		ANIM_RUNDASH = 50, // 개방 달리기?

		////////////////// 스킬 ///////////////////////

		ANIM_HEAL = 5, // 체력 0 되고 다시 찰때
		ANIM_CHARGE = 6, // 7,8 //기모으기
		ANIM_CHARGE_ROOP = 7,
		ANIM_CHARGE_END = 8,
		ANIM_COMBO_DOWN = 15, // 발로 땅 내려찍기
		ANIM_COMBO_PIST = 16, // 18 주먹 와다다닥
		ANIM_COMBO_PIST2 = 18,

		ANIM_AWAKE_COMBO_PIST = 28, // 28~32 완전개방? 했을 때
		ANIM_COMBO_UP = 17, // 하단발차기 후 올려차기
		ANIM_AREA_COMBO = 20, // 21 영역펼쳤을 때 안에 있으면 써짐
		ANIM_AIRGUN = 22, // 23,24 장풍
		ANIM_AIRGUN2 = 23,
		ANIM_AIRGUN3 = 24,
		ANIM_JUMPAIRGUN = 25, // 26,27
		ANIM_NACHIM = 63, // 영역나침

		ANIM_SKILL_UP = 34, // 빙글돌고 올라감
		ANIM_SKILL_DOWN = 35, // 34랑 연계 내려오는 동작

		ANIM_DASH_PUNCH = 39, // 대쉬랑 연계기

		ANIM_SUPERARMOR = 40, //41,42 자세딱 잡고 주먹 한방
		ANIM_LANDING = 44, // 43번 발차기라 연결 돼 있음

		ANIM_AWAKE_PUSHAWAY = 47, // 회천

		////////////////// 히트모션 ///////////////////

		ANIM_HIT_BLOW = 79, // 80 뒤로 날아감 
		ANIM_HIT_BLOW_END = 81, // 뒤로 날아감 끝
		ANIM_HIT_POWER_BLOW = 82, // 105 종잇장 마냥 날라감
		ANIM_HIT_MIDDLE_BLOW = 83, // 105 날라감
		ANIM_HIT_BOUNCD = 84, // 85 105 바운드
		ANIM_HIT_DMGFALL = 86, // 87= IDLE
		ANIM_HIT_FALL = 89, // 모르겟음 HIT가 맞나? 공중에서 웅크리며 있음 IDLE상태

		ANIM_HIT_GUARDBREAK = 90,
		ANIM_HIT_SPIN = 91, // 92, 93
		ANIM_HIT_POWER_GUARDBREAK = 94, // 뒷걸음질 함
		ANIM_HIT_RETURN_BIG = 95, // 반쯤 고개 숙인 상태에서 돌아옴
		ANIM_HIT_RETURN_SMALL = 96, // 작공격? 돌아옴

		ANIM_HIT_SMALL_AIR = 97, // 79,80 C자고역ㄱ 들어오고 날아감
		ANIM_HIT_AIR_UPPER = 98, // 어퍼 맞고 떨어짐
		ANIM_HIT_SMALL = 99,
		ANIM_HIT_UPPER = 100, // 어퍼

		ANIM_HIT_STUN = 101, // 102
		ANIM_HIT_GETUP_DIZZY = 103, // 104랑 연결 돼있음 근데 95랑 연결이 맞는듯
		ANIM_HIT_DOWNSTATE = 105, // 누워있음
		ANIM_HIT_GETUP_SPIN = 106, // 107

		////////////////// 엔드모션 ///////////////////

		////////////////// 컷신모션 ///////////////////
		ANIM_AWAKE_COMBO_PIST_CUT = 19, // 오의개방 했을 때 마지막 평타 때 컷신모드 나옴
		ANIM_FINAL_CUTSCENE = 111, // 파이널 컷신 
		ANIM_AWAKE_START = 48, // 개방 컷신

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
	void Update_AnimIndex(_uint iAnimIndex);
	void Update_Trigger(_double dTimeDelta);
	void Update_TriggerTime(_double dTimeDelta);
	void Update_Reset(_double dTimeDelta);
//	void Update_Begin(_double dTimeDelta);
	void Update_Phase_1(_double dTimeDelta);
	void Update_Phase_2(_double dTimeDelta);
	void Update_Phase_3(_double dTimeDelta);

#pragma endregion

#pragma region Pattern
private: //패턴 함수들
	void Update_State(_double dTimeDelta);
	void Update_Idle(_double dTimeDelta);
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

	void Update_Nachim_ComboPunch(_double dTimeDelta);
	void Update_Nachim_AirGun(_double dTimeDelta);

#pragma endregion
private:
	
	_bool	m_bStart = { false };
	_bool	m_bDashPunch = { false };
	_bool	m_bHit = { false };

	_bool	m_bStep_B = { false };
	_bool	m_bRunPunch = { false };
	_bool	m_bDashOn = { false };

	_bool	Test = { false };
private:
	//PHASE   m_eCurPhase = PAHSE_1;
	STATE	m_eCurstate = STATE_IDLE;
	ANIM    m_eCurAnimIndex = ANIM_IDEL;
	ANIM	m_ePreAnimIndex = ANIM_IDEL;
private: //애니메이션 제어용 변수들
	//Move
	_float	m_fMove_Speed = { 2.f };

	_bool	m_isJump_Move = { false };


	//오의 용
	_double		m_dTime_Special_Ready = { 0.0 };
	_double		m_dTime_Special_CutScene = { 0.0 };

	//오의 컷씬용
	vector<_double>		m_vecTime;
	_bool		m_isSpecialOn = { false };

	_bool		m_isFirst_Special_Jump = { true };
	_bool		m_isSecond_Special_Jump = { true };

	// 
	_bool  m_bFirstAnim = { false };

	// 태훈
	_bool	m_bFirst_Charge = { true };
	_bool	m_bFirst_Punch = { true };
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
	CTransform* m_pPlayerTransformCom = { nullptr };
	
private:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CBoss_Akaza* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END