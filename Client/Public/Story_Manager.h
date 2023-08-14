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

	void Set_Line_Move_Done(_bool b);
	_bool Get_Line_Move_Done();
	
private:
	_uint    m_Select = { 0 };
	_bool    m_Line_Move_Done = { false };

public:
	virtual void Free() override;
};

END