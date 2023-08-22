#include "..\Public\LandObject.h"

#include "GameInstance.h"
#include "Cell.h"

CLandObject::CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CLandObject::CLandObject(const CLandObject& rhs)
	: CGameObject(rhs)
{
}

void CLandObject::Change_NaviMesh(NAVI_TYPE eNaviType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	dynamic_cast<CLandObject*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), 0))->Set_CurNaviMesh(eNaviType);

	if(pGameInstance->Get_GameObject_ListSize(pGameInstance->Get_CurLevelIdx() , TEXT("Layer_Player")) > 1)
		dynamic_cast<CLandObject*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), 1))->Set_CurNaviMesh(eNaviType);

	Safe_Release(pGameInstance);
}

//HRESULT CLandObject::Initialize_Prototype()
//{
//	if (FAILED(__super::Initialize_Prototype()))
//		return E_FAIL;
//
//	return S_OK;
//}
//
//
//HRESULT CLandObject::Initialize(void* pArg)
//{
//	if (FAILED(__super::Initialize(pArg)))
//		return E_FAIL;
//
//	return S_OK;
//}
//
//void CLandObject::Tick(_double dTimeDelta)
//{
//	__super::Tick(dTimeDelta);
//}
//
//void CLandObject::LateTick(_double dTimeDelta)
//{
//	__super::LateTick(dTimeDelta);
//}
//
//HRESULT CLandObject::Render()
//{
//	if (FAILED(__super::Render()))
//		return E_FAIL;
//
//	return S_OK;
//}

_vector CLandObject::Compute_Height(_fvector vPos)
{
	if (nullptr == m_pNavigationCom)
		return XMVectorSet(0.f, 0.f, 0.f, 1.f);

	_float3 vPoints[CCell::POINT_END];

	//m_pNavigationCom->Get_CurrentPoints(vPoints);

	_vector vPlane = XMPlaneFromPoints(XMLoadFloat3(&vPoints[CCell::POINT_A]),
										XMLoadFloat3(&vPoints[CCell::POINT_B]),
										XMLoadFloat3(&vPoints[CCell::POINT_C]));

	// ax + by + cz + d = 0
	//y = (-ax - cz - d) / b

	_float fHeight = (-XMVectorGetX(vPlane) * XMVectorGetX(vPos) 
					- XMVectorGetZ(vPlane) * XMVectorGetZ(vPos) 
					- XMVectorGetW(vPlane)) 
					/ XMVectorGetY(vPlane);

	return XMVectorSetY(vPos, fHeight);


}

void CLandObject::Free()
{
	__super::Free();
	
	for (_uint i = 0; i < NAVI_END; ++i)
	{
		Safe_Release(m_pNavigationCom[i]);
	}
}
