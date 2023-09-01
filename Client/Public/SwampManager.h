#pragma once
#include "Client_Defines.h"
#include "Base.h"

#include "AtkCollider.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CSwampManager final : public CBase
{
	DECLARE_SINGLETON(CSwampManager)

private:
	CSwampManager();
	virtual ~CSwampManager() = default;

public:
	_int	Get_Phase1_MainIndex() { return m_iPhase1_MainIndex; }
	_float	Get_Hp() { return m_fSwamp_Hp; }
	_float	Get_Hp_Max() { return m_fSwamp_Hp_Max; }
	_int	Get_Hp_Phase() { return m_iHp_Phase; }
	_int	Find_Alive() {
		if (m_iDead_1 == false)
			return 1;
		else if (m_iDead_2 == false)
			return 2;
		else if (m_iDead_3 == false)
			return 3;
		return 0;
	}
	

	void	Set_Phase1_MainIndex(_int index) { m_iPhase1_MainIndex = index; }
	void	Set_Dmg(_float fDmg) { m_fSwamp_Hp -= fDmg; }
	void	Set_Hp_Phase_Up() { m_iHp_Phase++; }
	void	Set_Dead(_int index) {
		if (index == 1)
			m_iDead_1 = true;
		else if (index == 2)
			m_iDead_2 = true;
		else if (index == 3)
			m_iDead_3 = true;
	}

private:
	_int	m_iPhase1_MainIndex = { 2 };
	_bool	m_iDead_1 = { false };
	_bool	m_iDead_2 = { false };
	_bool	m_iDead_3 = { false };

	//Ã¼·Â
	_int	m_iHp_Phase = { 0 };
	_float	m_fSwamp_Hp = { 150.0f };
	_float	m_fSwamp_Hp_Max = { 150.0f };

public:
	virtual void Free() override;
};

END