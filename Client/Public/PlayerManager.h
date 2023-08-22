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

public:
	_int	Get_PlayerIndex() { return iPlayerIndex; }
	_bool	Get_First_Player_Change() { return isFirst_Player_Change; }
	_bool	Get_First_Setting_Status() { return isFirst_Setting_Status; }

	_float4 Get_PlayerPos_Change() { return PlayerPos_Change; }


	void	Set_PlayerIndex(_int index) { iPlayerIndex = index; }
	void	Set_First_Player_Change(_bool first) { isFirst_Player_Change = first; }
	void	Set_First_Setting_Status(_bool first) { isFirst_Setting_Status = first; }

	void	Set_PlayerPos_Change(_float4 Pos) { PlayerPos_Change = Pos; }
	void	Set_PlayerPos_Change(_vector vPos) {
		XMStoreFloat4(&PlayerPos_Change, vPos);
	}
public:
	virtual void Free() override;
};

END