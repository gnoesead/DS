#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class COptionManager final : public CBase
{
	DECLARE_SINGLETON(COptionManager)

private:
	COptionManager();
	virtual ~COptionManager() = default;

public:
	HRESULT Inititalize();
	void Tick(_double dTimeDelta);

	void Set_Is_Option_On(_bool Is);
	_bool Get_Is_Option_On();

	_bool Get_Is_Move_Done();

	_float Get_Option_Move();

	void Set_Is_Reset(_bool Is);
	_bool Get_Is_Reset();

private:
	_bool m_Is_Option_On = { false };
	_bool m_Is_Move_Done = { false };


	_float m_Option_Move = { 40.f };
	_bool  m_Is_Reset = { false };

public:
	virtual void Free() override;
};

END