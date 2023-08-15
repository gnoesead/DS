#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CBattle_UI_Manager final : public CBase
{
	DECLARE_SINGLETON(CBattle_UI_Manager)

private:
	CBattle_UI_Manager();
	virtual ~CBattle_UI_Manager() = default;

public:
	HRESULT Inititalize();
	void Tick(_double dTimeDelta);

	void Set_Player_Type(_uint Type);
	_uint Get_Player_Type();

	void Set_Player_Skill_Type(_uint Type);
	_uint Get_Player_Skill_Type();

	_bool Get_Skill_On();
	
private:
	_uint m_Player_Index = { 0 };
	_uint m_Player_Skill_Index = { 0 };

	_bool m_Skill_On = { false };
	_float m_Skill_On_TimeAcc = { 0.f };


public:
	virtual void Free() override;
};

END