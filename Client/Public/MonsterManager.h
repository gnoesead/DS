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
	void	Cnt_AttackOn_Up() { m_iCnt_AttackOn++; }
	void	Cnt_AttackOn_Down() { m_iCnt_AttackOn--; }
	_bool	Can_Attack();

private:
	_int	m_iCnt_AttackOn = { 0 };

public:
	virtual void Free() override;
};

END