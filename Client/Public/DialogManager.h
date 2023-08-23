#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CDialogManager final : public CBase
{
	DECLARE_SINGLETON(CDialogManager)

private:
	CDialogManager();
	virtual ~CDialogManager() = default;

public:
	HRESULT Inititalize();
	void Set_Dialog_Type(_uint Type);
	_uint Get_Dialog_Type();

	
private:
	_uint    m_Dialog_Type = { 99 };
	




public:
	virtual void Free() override;
};

END