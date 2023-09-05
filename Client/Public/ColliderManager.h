#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CColliderManager final : public CBase
{
	DECLARE_SINGLETON(CColliderManager)

public:
	enum COLLIDERGROUP {
		COLLIDER_PLAYER, COLLIDER_PLAYER_BULLET,
		COLLIDER_MONSTER, COLLIDER_MONSTER_BULLET, COLLIDER_END
	};
private:
	CColliderManager();
	virtual ~CColliderManager() = default;

public:
	HRESULT Inititalize();
	HRESULT Check_Collider(_uint iLevelIndex, _double dTimeDelta);
	HRESULT Render();

private:
	void Set_IsColl(_uint iIndex) {
		m_isColl[iIndex] = true;
	}
	void Reset_IsColl();

private:
	_bool	m_isColl[3] = {};
	_uint	m_iPAtoMCount = { 0 };

private:
	HRESULT Check_PlayerToMonster(_uint iLevelIndex, _double dTimeDelta);
	HRESULT Check_PlayerAtkToMonster(_uint iLevelIndex, _double dTimeDelta);

	HRESULT Check_PlayerToBoss(_uint iLevelIndex, _double dTimeDelta);
	HRESULT Check_PlayerAtkToBoss(_uint iLevelIndex, _double dTimeDelta);

	HRESULT Check_PlayerToCollisionBox(_uint iLevelIndex, _double dTimeDelta);

	HRESULT Check_MonsterToMonster(_uint iLevelIndex, _double dTimeDelta);
	HRESULT Check_MonsterAtkToPlayer(_uint iLevelIndex, _double dTimeDelta);

	HRESULT Check_BossAtkToPlayer(_uint iLevelIndex, _double dTimeDelta);
	HRESULT Check_PlayerToNPC(_uint iLevelIndex, _double dTimeDelta);

public:
	virtual void Free() override;
};

END