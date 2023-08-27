#include "..\Public\Layer.h"
#include "GameObject.h"
#include "Object_Manager.h"

CLayer::CLayer()
{
}

CComponent* CLayer::Get_Component(const _tchar* pComponentTag)
{
	if (0 == m_GameObjects.size())
		return nullptr;

	for (auto& pGameObject : m_GameObjects)
		if (nullptr != pGameObject)
			return pGameObject->Get_Component(pComponentTag);

	return nullptr;
}

CComponent* CLayer::Get_Component(_uint iIndex, const _tchar* pComponentTag)
{
	if (0 == m_GameObjects.size() || m_GameObjects.size() <= iIndex)
		return nullptr;

	auto iter = m_GameObjects.begin();

	for (_uint i = 0; i < iIndex; i++)
		iter++;

	if (nullptr != (*iter))
		return (*iter)->Get_Component(pComponentTag);

	return nullptr;
}

CGameObject* CLayer::Get_GameObject(_uint iIndex)
{
	if (0 == m_GameObjects.size() || m_GameObjects.size() <= iIndex)
		return nullptr;

	auto iter = m_GameObjects.begin();

	for (_uint i = 0; i < iIndex; i++)
		iter++;

	return (*iter);
}

HRESULT CLayer::Initialize(_bool isKeep)
{
	m_isKeep = isKeep;

	return S_OK;
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

void CLayer::Tick(_double dTimeDelta)
{
	for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if (nullptr != (*iter))
		{
			if ((*iter)->Get_TimeFree()) {
				(*iter)->Tick(dTimeDelta);
			}
			else {

				if (CObject_Manager::GetInstance()->Get_Is_Time_Stop() == true) {
					(*iter)->Tick(0);
				}
				else if (CObject_Manager::GetInstance()->Get_Is_Time_Slow() == true) {
					(*iter)->Tick(dTimeDelta * CObject_Manager::GetInstance()->Get_Time_Slow_Amount());
				}
				else {
					(*iter)->Tick(dTimeDelta);
				}
			}
			
			if (true == (*iter)->Get_Dead())
			{
				if (false == m_isKeep)
					Safe_Release((*iter));

				iter = m_GameObjects.erase(iter);
				continue;
			}
			else if (true == (*iter)->Get_Collect())
			{
				iter = m_GameObjects.erase(iter);
				continue;
			}
			++iter;
		}
	}
}

void CLayer::LateTick(_double dTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject) {

			if (pGameObject->Get_TimeFree()) {
				pGameObject->Tick(dTimeDelta);
			}
			else {

				if (CObject_Manager::GetInstance()->Get_Is_Time_Stop() == true) {
					pGameObject->LateTick(0);
				}
				else if (CObject_Manager::GetInstance()->Get_Is_Time_Slow() == true) {
					pGameObject->LateTick(dTimeDelta * CObject_Manager::GetInstance()->Get_Time_Slow_Amount());
				}
				else {
					pGameObject->LateTick(dTimeDelta);
				}
			}
		}
		
	}
}

void CLayer::Clear_Layer()
{
	Free();
}

CLayer* CLayer::Create(_bool isKeep)
{
	CLayer* pInstance = new CLayer();

	if (FAILED(pInstance->Initialize(isKeep)))
	{
		MSG_BOX("Failet to Created : CLayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}
 

void CLayer::Free() 
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);
	
	m_GameObjects.clear();
}
