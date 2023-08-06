#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CStoryManager final : public CBase
{
	DECLARE_SINGLETON(CStoryManager)

private:
	CStoryManager();
	virtual ~CStoryManager() = default;

public:
	HRESULT Inititalize();
	void Tick();
	void Set_Select_Type(_uint Type);
	_uint Get_Select_Type();

	
private:
	_uint    m_Select = { 0 };
	

public:
	virtual void Free() override;
};

END