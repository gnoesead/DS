#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CMiniMapManager final : public CBase
{
	DECLARE_SINGLETON(CMiniMapManager)

private:
	CMiniMapManager();
	virtual ~CMiniMapManager() = default;

public:
	HRESULT Inititalize();
	void Tick();

	void Set_MiniMap_X(_float x);
	void Set_MiniMap_Y(_float y);

	_float Get_MiniMap_X();
	_float Get_MiniMap_Y();

private:
	_float m_MiniMap_X = {};
	_float m_MiniMap_Y = {};

	_bool m_Village_Battle_On = { false };
	_bool m_House_Battle_On = { false };

public:
	virtual void Free() override;
};

END