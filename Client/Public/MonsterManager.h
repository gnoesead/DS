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

public:
	_bool	Check_Can_Attack();

	void	Attack_Cnt_Up() { m_iCnt_Attack++; }
	void	Attack_Cnt_Down() { 
		m_iCnt_Attack--; 
		if (m_iCnt_Attack <= 0)
			m_iCnt_Attack = 0;
	}

	

private:
	_int				m_iCnt_Attack = { 0 };

public:
	virtual void Free() override;
};

END