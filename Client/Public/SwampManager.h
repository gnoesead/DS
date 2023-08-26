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

	void	Set_Phase1_MainIndex(_int index) { m_iPhase1_MainIndex = index; }

private:
	_int	m_iPhase1_MainIndex = { 2 };

public:
	virtual void Free() override;
};

END