#pragma once
#include "Engine_Defines.h"
#include "GameObject.h"
#include "Navigation.h"

BEGIN(Engine)

class ENGINE_DLL CLandObject abstract : public CGameObject
{
public:
enum NAVI_TYPE { NAVI_VILLAGE_MAINROAD1,NAVI_VILLAGE_MAINROAD2, NAVI_VILLAGE_INSIDEWALL1, NAVI_VILLAGE_INSIDEWALL2 , NAVI_VILLAGE_ROOF , NAVI_VILLAGE_WALL , 
	NAVI_HOUSE_0_0,NAVI_HOUSE_1_0,NAVI_HOUSE_1_1,NAVI_HOUSE_3_0,NAVI_HOUSE_4_0, NAVI_ACAZA, NAVI_END };

protected:
	CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLandObject(const CLandObject& rhs);
	virtual ~CLandObject() = default;

public:
	//virtual HRESULT Initialize_Prototype() override;
	//virtual HRESULT Initialize(void* pArg) override;
	//virtual void	Tick(_double dTimeDelta) override;
	//virtual void	LateTick(_double dTimeDelta) override;
	//virtual HRESULT Render() override;

protected:
	CNavigation* m_pNavigationCom[NAVI_END] = {nullptr};

	//NaviMesh
	NAVI_TYPE		m_eCurNavi = { NAVI_END };

protected:
	_vector Compute_Height(_fvector vPos);

public:
	//virtual CGameObject* Clone(void* pArg) override = 0;
	virtual void Free() override;
};

END