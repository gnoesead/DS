#pragma once
#include "Client_Defines.h"
#include "Base.h"

#include "AtkCollider.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CAtkCollManager final : public CBase
{
	DECLARE_SINGLETON(CAtkCollManager)

private:
	CAtkCollManager();
	virtual ~CAtkCollManager() = default;

public:
	void	CreateAtkColl(const _tchar* pLayerTag, CAtkCollider::ATKCOLLDESC* pAtkCollDesc, CGameInstance* pGameInstance);

public:
	void	Collect_Collider(CGameObject* pGameObject);
	void	Reuse_Collider(const _tchar* pLayerTag, CAtkCollider::ATKCOLLDESC* pAtkCollDesc);


	_int	Get_WebFull_Index() { return m_iWeb_Full_Index; }
	void	Set_WebFull_Index(_int index) { m_iWeb_Full_Index = index; }

private:
	list<CGameObject*>	m_AtkCollPool;
	_int				m_iCount = { 0 };

	_int	m_iWeb_Full_Index = { 0 };
public:
	virtual void Free() override;
};

END