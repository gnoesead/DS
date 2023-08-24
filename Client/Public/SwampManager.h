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
	

private:
	

public:
	virtual void Free() override;
};

END