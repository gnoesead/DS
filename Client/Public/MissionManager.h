#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CMissionManager final : public CBase
{
	DECLARE_SINGLETON(CMissionManager)

private:
	CMissionManager();
	virtual ~CMissionManager() = default;

public:
	HRESULT Inititalize();

	void Set_Main_Mission_Type(_uint Type);
	_uint Get_Main_Mission_Type();

	void Set_Sub_Mission_Type(_uint Type);
	_uint Get_Sub_Mission_Type();

	void Set_Is_Dialog_On(_bool Is_On);
	_bool Get_Is_Dialog_On();
	
private:
	_uint    m_Main_Mission_Type = { 0 };
	_uint    m_Sub_Mission_Type = { 0 };

	_bool    m_Is_Dialog_On = { false };


public:
	virtual void Free() override;
};

END