#pragma once
#include "Client_Defines.h"
#include "Base.h"

#include "AtkCollider.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CPlayerManager final : public CBase
{
	DECLARE_SINGLETON(CPlayerManager)

private:
	CPlayerManager();
	virtual ~CPlayerManager() = default;
	
private: // ½ºÅÈ°ü·Ã °øÀ¯
	_float	fHp = { 100.0f };
	_float	fMp = { 100.0f };
	_int	iSpecial_Cnt = { 0 };
	_float	fSpecial = { 0.0f };
	_float	fSupport = { 100.0f };
public:
	_float	Get_Hp() { return fHp; }
	_float	Get_Mp() { return fMp; }
	_int	Get_Special_Cnt() { return iSpecial_Cnt; }
	_float	Get_Special() { return fSpecial; }
	_float	Get_Support() { return fSupport; }

	void	Set_Hp(_float hp) { fHp = hp; }
	void	Set_Mp(_float mp) { fMp = mp; }
	void	Set_Special_Cnt(_int cnt) { iSpecial_Cnt = cnt; }
	void	Set_Special(_float special) { fSpecial = special; }
	void	Set_Support(_float support) { fSupport = support; }

private: //Ä³¸¯ÅÍ ½º¿Ò Ã¼ÀÎÁö °ü·Ã º¯¼ö
	_int	iPlayerIndex = { 0 }; // 0 : ÅºÁö·Î, 1: Á¨ÀÌÃ÷, 2:·»°íÄí'

	_bool	isFirst_Player_Change = { true };

	_bool	isFirst_Setting_Status = { true };

	_float4		PlayerPos_Change = { 0.0f, 0.0f, 0.0f,  1.0f };

	_float4		Swaping_Pos = { 0.0f, 0.0f, 0.0f, 1.0f };

public:
	_int	Get_PlayerIndex() { return iPlayerIndex; }
	_bool	Get_First_Player_Change() { return isFirst_Player_Change; }
	_bool	Get_First_Setting_Status() { return isFirst_Setting_Status; }

	_float4 Get_PlayerPos_Change() { return PlayerPos_Change; }
	_float4 Get_Swaping_Pos() { return Swaping_Pos; }


	void	Set_PlayerIndex(_int index) { iPlayerIndex = index; }
	void	Set_First_Player_Change(_bool first) { isFirst_Player_Change = first; }
	void	Set_First_Setting_Status(_bool first) { isFirst_Setting_Status = first; }

	void	Set_PlayerPos_Change(_float4 Pos) { PlayerPos_Change = Pos; }
	void	Set_PlayerPos_Change(_vector vPos) {
		XMStoreFloat4(&PlayerPos_Change, vPos);
	}

	void	Set_Swaping_Pos(_float4 Pos) { Swaping_Pos = Pos; }
	void	Set_Swaping_Pos(_vector vPos) { XMStoreFloat4(&Swaping_Pos, vPos); }

	void Reset_PlayerManager() {
		iPlayerIndex =  0 ; 
		isFirst_Player_Change = true ;
		isFirst_Setting_Status =  true ;
		PlayerPos_Change = { 0.0f, 0.0f, 0.0f,  1.0f };
	}

private:
	_bool	m_isSlow = { false };
	_double m_dDelay_Slow = { 0.0 };
	_double m_dLimit_Slow = { 0.0 };

	_int	m_iCnt_Slow = { 0 };
	_int	m_iCnt_Limit_Slow = { 0 };

public:
	_bool	Get_Slow() { 
		m_iCnt_Slow++;
		if (m_isSlow)
		{
			if (m_iCnt_Slow >= m_iCnt_Limit_Slow)
			{
				m_iCnt_Slow = 0;
			}
			else
			{
				return true;
			}
		}
		return m_isSlow;
	}
	void	Set_Slow( _double dTime, _int Cnt) { 
		m_isSlow = true;
		m_dLimit_Slow = dTime;
		m_iCnt_Limit_Slow = Cnt;
	}

	void	Slow_Acc(_double dTimeDelta);

public:
	virtual void Free() override;
};

END