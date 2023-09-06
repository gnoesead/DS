#pragma once
#include "Client_Defines.h"
#include "Base.h"

#include "AtkCollider.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CMonsterManager final : public CBase
{
	DECLARE_SINGLETON(CMonsterManager)

private:
	CMonsterManager();
	virtual ~CMonsterManager() = default;

public:
	//다수 몬스터 어택관련
	void	Cnt_AttackOn_Up() { m_iCnt_AttackOn++; }
	void	Cnt_AttackOn_Down() { m_iCnt_AttackOn--; }
	_bool	Can_Attack();

	//배틀 시작 끝 관련
	_bool	Get_BattleOn() { return m_isBattleOn; }
	void	Set_BattleOn(_bool battle) { m_isBattleOn = battle; }

	_int	Get_ThreeCnt() { return m_iThreeCnt; }
	void	Set_ThreeCnt(_int Cnt) { m_iThreeCnt = Cnt; }
	void	Plus_ThreeCnt() { m_iThreeCnt++; }

	//잠입모드용
	_int	Get_ResetIndex_Player() { return m_iResetIndex_ForPlayer; }
	void	Set_ResetIndeX_Player(_int index) { m_iResetIndex_ForPlayer = index; }

	_bool	Get_PlayerBack() { return m_isPlayerBack; }
	void	Set_PlayerBack(_bool back) { m_isPlayerBack = back; }
	_bool	Get_ZenitsuBack() { return m_isZenitsuBack; }
	void	Set_ZenitsuBack(_bool back) { m_isZenitsuBack = back; }

	_float4	Get_DirStealthAtk() { return m_Dir_StealthAtk; }
	void	Set_DirStealthAtk(_float4 Dir) { m_Dir_StealthAtk = Dir; }

	_bool	Get_StealthAttack() { return m_isStealthAttack; }
	void	Set_StealthAttack(_bool Atk) { m_isStealthAttack = Atk; }

	_bool	Get_StealthEnd_BattleStart() { return m_isStealthEnd_BattleStart; }
	void	Set_StealthEnd_BattleStart(_bool start) { m_isStealthEnd_BattleStart = start; }
	_bool	Get_StealthEnd_BattleStart_Fade() { return m_isStealthEnd_BattleStart_Fade; }
	void	Set_StealthEnd_BattleStart_Fade(_bool start) { m_isStealthEnd_BattleStart_Fade = start; }


	_bool	Get_Zenitsu_IndexPlus() { return m_isZenitsu_IndexPlus; }
	void	Set_Zenitsu_IndexPlus(_bool up) { m_isZenitsu_IndexPlus = up; }

private:
	//다수 몬스터 어택관련
	_int	m_iCnt_AttackOn = { 0 };


	//배틀 시작 끝 관련
	_bool	m_isBattleOn = { false };
	_int	m_iThreeCnt = { 0 };


	//잠입 모드용
	_int	m_iResetIndex_ForPlayer = { 0 };

	_bool	m_isPlayerBack = { false };
	_bool	m_isZenitsuBack = { false };
	_float4		m_Dir_StealthAtk = { 0.0f, 0.0f, 0.0f, 0.0f };

	_bool	m_isStealthAttack = { false };

	_bool	m_isStealthEnd_BattleStart = { false };
	_bool	m_isStealthEnd_BattleStart_Fade = { false };

	_bool	m_isZenitsu_IndexPlus = { false };

public:
	virtual void Free() override;
};

END