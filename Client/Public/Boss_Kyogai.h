#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)

END

BEGIN(Client)

class CBoss_Kyogai final : public CMonster
{
public:

	enum  PHASE
	{
		BEGIN, PHASE_1, PHASE_2
	};
	enum  STATE
	{
		STATE_INTERACT, STATE_BEGIN, STATE_NEXTPHASE, STATE_HEAL, STATE_AWAKE,
		STATE_GUARD,
		STATE_IDLE

	};

#pragma region AnimIndex
	enum  ANIM {
		////////////////// 기본 MOVE///////////////////
		ANIM_IDLE = 41, // 아이들
		ANIM_RUN = 43, // 런
		ANIM_STEP_FRONT =  46, // 대시
		ANIM_STEP_FRONT2 = 47, // 점프
		ANIM_STEP_BACK = 45, // 뒷대시
		ANIM_STEP_LEFT = 48, // 좌대시
		ANIM_STEP_LEFT2 = 49, // 좌점프
		ANIM_STEP_RIGHT = 50, // 우대시
		ANIM_STEP_RIGHT2 = 51, // 우점프

		ANIM_WALK_LEFT_START = 60, // 좌 걷기
		ANIM_WALK_LEFT_LOOP = 61, // 좌 걸어
		ANIM_WALK_LEFT_END = 62, // 좌 걸어 끝
		ANIM_WALK_RIGHT_START = 63, // 우 걷기
		ANIM_WALK_RIGHT_LOOP = 61, // 좌 걸어
		ANIM_WALK_RIGHT_END = 62, // 좌 걸어 끝
		
		////////////////// 스킬 ///////////////////////
		ANIM_ATKCMB1 = 1, // 배꼽에 있는 북 3연방 -> 톱날 세번나오는듯?
		ANIM_ATKCMB2 = 2, // 두방침
		ANIM_ATKCMB3 = 3, // 라스트

		//페이즈 2 배꼽 북 연속치기
		ANIM_ATKCMB_01READY = 4, // 연속 배꼽치기 준비자세
		ANIM_ATKCMB_02LOOP = 5, // 배꼽 연속으로 치기
		ANIM_ATKCMB_03END = 6, // 배꼽 연속치기 마무리 오른손 아이들자세로 돌아옴

		ANIM_ATKSKCMB = 8, // 으으 기모으고 배꼽치기 와다다다닥
		ANIM_ATKPUNCH = 9, // 강펀치~

		ANIM_KICKDOWN = 11, // 왼발 내려찍기
		ANIM_STOMPKICK = 13, // 내려찍기 준비자세하고 함 이거쓰자

		ANIM_ATKSK_READY = 18, // 갈매기 레디
		ANIM_ATKSK_LOOP = 19, // 레디 루프
		ANIM_ATKSK_LB = 20, // 왼쪽 엉덩이 빵
		ANIM_ATKSK_RB = 21, // 오른쪽 엉덩이 빵
		ANIM_ATKSK_LF = 22, // 왼쪽 가슴
		ANIM_ATKSK_RF = 23, // 오른쪽 가슴
		ANIM_ATKSK_END = 24, // 끝나고 42 IDLE로 가야함

		ANIM_ROOMCHANGE_READY = 25, // 만세 시작 북와다다다다다ㅏ 준비자세
		ANIM_ROOMCHANGE_START = 26, // 북 와다다다다다ㅏ 방바꾸고스킬 마구마구 씀
		ANIM_ROOMCHANGE_END = 27, // 북 와다다다다다 끝남

		ANIM_ATKSTEP_BACK = 28, // 뒤돌면서 북치기
		ANIM_ATKSTEP_FRONT = 29, // 앞돌면서 북치기

		
		////////////////// 히트모션 ///////////////////
		ANIM_GUARD_READY = 32, // 가드 준비자세
		ANIM_GUARD_LOOP = 33, // 가드 루프
		ANIM_GUARD_END = 34, // 가드 풀었다
		ANIM_GUARD_BREAK = 36, // 가드 풀림

		ANIM_BASETURN_L = 52, // 왼쪽 어깨 살짝 히트모션 아닌데 쓸만할듯?
		ANIM_BASETURN_R = 53, // 오른쪽 어깨 살짝
		
		////////////////// 엔드모션 ///////////////////
		ANIM_HEAL,
		ANIM_DEATH = 66, // 데스
		ANIM_DEATH_GETUP = 69, // 죽음에서 살아나기
		ANIM_HIT = 67,
		ANIM_GETUP = 68, // 일어나기
		ANIM_HITDOWN = 70, // 무릎꿇으면서 앉음
		ANIM_HITDOWN_LOOP = 71, // 무릎꿇는 자세 루프
		ANIM_HITDOWN_GETUP = 72, // 머리털고 일어남
		ANIM_HITDOWN_RETURN = 31, // 연결

		////////////////// 컷신모션 ///////////////////
		ANIM_AWAKE = 0 // 개방신


	};
#pragma endregion
private:
	CBoss_Kyogai(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Kyogai(const CBoss_Kyogai& rhs);
	virtual ~CBoss_Kyogai() = default;
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
	void Update_AnimIndex(_uint iAnimIndex);
	void Update_Trigger(_double dTimeDelta);
	void Update_TriggerTime(_double dTimeDelta);
	
	//	void Update_Begin(_double dTimeDelta);
	void Update_Phase_1(_double dTimeDelta);
	void Update_Phase_2(_double dTimeDelta);	

	void Trigger_Interact_Phase_1(_double dTimeDelta);
	void Trigger_Interact_Phase_2(_double dTimeDelta);	

	void Trigger_Interact();
	void Trigger_Begin();
	void Trigger_Guard();	
	void Trigger_PushAway();	
	void Trigger_NextPhase();	
	void Trigger_Heal();
	void Trigger_Awake();
	void Trigger_Awake_Cinematic();

#pragma endregion

#pragma region Pattern
private: //패턴 함수들
	void Update_State(_double dTimeDelta);
	void Update_Interact(_double dTimeDelta);
	void Update_Begin(_double dTimeDelta);
	void Update_PushAway(_double dTimeDelta);
	
	void Update_Guard(_double dTimeDelta);
	
	void Update_NextPhase(_double dTimeDelta);
	
	void Update_Heal(_double dTimeDelta);
	void Update_Awake(_double dTimeDelta);


#pragma endregion
private: // _bool
	
	_bool	m_bAnimFinish = { false };

	_bool	m_bHit = { false };
	_bool	m_bHeal = { false };
	
	_bool	m_bTrigger = { false };
	_bool	m_bPatternStart = { false };

	_bool	m_bAwake = { false };
	_bool	m_bFirstAwake = { false };
	_bool	m_bSecondAwake = { false };
	//_bool	m_bNextPhase = { false };

private: // time
	_double	m_dJumpStompTime = { 0.0 };
	_double m_dAwakeTime = { 0.0 };

	_double m_dTriggerTime = { 0.0 };

private:
	_uint	m_iRandomDirNum = { 0 };
	_uint	m_iRandomPatternNum = { 0 };
	_uint	m_iPunchCount = { 0 };
	_uint	m_iIdleCnt = { 0 };

	_uint	m_iTriggerCnt = { 0 };

	_uint	m_iHp = { 100 }; // 임시

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
	static CBoss_Kyogai* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END