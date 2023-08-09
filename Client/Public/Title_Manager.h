#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CTitleManager final : public CBase
{
	DECLARE_SINGLETON(CTitleManager)

private:
	CTitleManager();
	virtual ~CTitleManager() = default;

public:
	HRESULT Inititalize();
	void Tick();
	void Set_Select_Type(_bool Type);
	_bool Get_Select_Type();

	
private:
	_bool    m_Select = { 0 };
	

public:
	virtual void Free() override;
};

END