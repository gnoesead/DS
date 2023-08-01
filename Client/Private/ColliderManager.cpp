#include "pch.h"
#include "..\Public\ColliderManager.h"

#include "GameInstance.h"
#include "Player.h"

#include "AtkCollider.h"

IMPLEMENT_SINGLETON(CColliderManager)

CColliderManager::CColliderManager()
{
}

HRESULT CColliderManager::Inititalize()
{
	return S_OK;
}

HRESULT CColliderManager::Check_Collider(_uint iLevelIndex, _double dTimeDelta)
{
	if (FAILED(Check_PlayerToMonster(iLevelIndex, dTimeDelta)))
	{
		MSG_BOX("Failed to Check_PlayerToMonster");
		return E_FAIL;
	}

	if (FAILED(Check_PlayerAtkToMonster(iLevelIndex, dTimeDelta)))
	{
		MSG_BOX("Failed to Check_PlayerToMonster");
		return E_FAIL;
	}

	if (FAILED(Check_MonsterToMonster(iLevelIndex, dTimeDelta)))
	{
		MSG_BOX("Failed to Check_PlayerToMonster");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CColliderManager::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_tchar szText[MAX_PATH] = { TEXT("") };

	if (m_isColl[0])
		wsprintf(szText, TEXT("PtoM : true"));
	else
		wsprintf(szText, TEXT("PtoM : false"));

	if (FAILED(pGameInstance->Draw_Font(TEXT("Font_KR"), szText, _float2(500.f, 0.f), _float2(0.5f, 0.5f))))
		return E_FAIL;

	if (m_isColl[1])
		wsprintf(szText, TEXT("PAtoM : true, %d"), m_iPAtoMCount);
	else
		wsprintf(szText, TEXT("PAtoM : false"));

	if (FAILED(pGameInstance->Draw_Font(TEXT("Font_KR"), szText, _float2(500.f, 20.f), _float2(0.5f, 0.5f))))
		return E_FAIL;

	if (m_isColl[2])
		wsprintf(szText, TEXT("MtoM : true"));
	else
		wsprintf(szText, TEXT("MtoM : false"));

	if (FAILED(pGameInstance->Draw_Font(TEXT("Font_KR"), szText, _float2(500.f, 40.f), _float2(0.5f, 0.5f))))
		return E_FAIL;

	list<CGameObject*>* pPlayerAtkColls = pGameInstance->Get_GameObjects(LEVEL_STATIC, TEXT("Layer_PlayerAtk"));

	_uint iNum = { 0 };

	if (nullptr != pPlayerAtkColls && 0 != pPlayerAtkColls->size())
	{
		for (auto& pAtkColl : (*pPlayerAtkColls))
		{
			_uint iCount = dynamic_cast<CAtkCollider*>(pAtkColl)->Get_CollCount();

			_float fY = 20.f * iNum;

			wsprintf(szText, TEXT("iCollCount : %d"), iCount);

			if (FAILED(pGameInstance->Draw_Font(TEXT("Font_KR"), szText, _float2(900.f, fY), _float2(0.5f, 0.5f))))
				return E_FAIL;

			iNum++;
		}
	}

	Safe_Release(pGameInstance);

	Reset_IsColl();

	return S_OK;
}

void CColliderManager::Reset_IsColl()
{
	for (_uint i = 0; i < 3; i++)
	{
		m_isColl[i] = false;
	}

	m_iPAtoMCount = 0;
}

HRESULT CColliderManager::Check_PlayerToMonster(_uint iLevelIndex, _double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Player"), TEXT("Com_Sphere")));

	list<CGameObject*>* pMonsters = pGameInstance->Get_GameObjects(iLevelIndex, TEXT("Layer_Monster"));

	_int iCollCount = { 0 };

	if (nullptr != pPlayerCollider && nullptr != pMonsters)
	{
		for (auto& pMonster : (*pMonsters))
		{
			if (nullptr != pMonster)
			{
				CCollider* pMonsterCollider = dynamic_cast<CCollider*>(pMonster->Find_Component(TEXT("Com_Sphere")));
				pPlayerCollider->Intersect(pMonsterCollider);

				if (true == pPlayerCollider->Get_Coll())
					iCollCount++;

				if (true == pMonsterCollider->Get_Coll())
				{
					Set_IsColl(0);

					CTransform* pMonsterTransform = dynamic_cast<CTransform*>(pMonster->Find_Component(TEXT("Com_Transform")));
					CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Player"), (TEXT("Com_Transform"))));

					_vector vDir = pMonsterTransform->Get_State(CTransform::STATE_POSITION) - pPlayerTransform->Get_State(CTransform::STATE_POSITION);

					pMonsterTransform->Go_Dir(dTimeDelta * 0.5, vDir);
					pPlayerTransform->Go_Dir(dTimeDelta * 0.5, vDir * -1);
				}
			}
		}
	}

	if (0 < iCollCount)
		pPlayerCollider->Set_Coll(true);
	else
		pPlayerCollider->Set_Coll(false);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CColliderManager::Check_PlayerAtkToMonster(_uint iLevelIndex, _double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*>* pPlayerAtkColls = pGameInstance->Get_GameObjects(LEVEL_STATIC, TEXT("Layer_PlayerAtk"));

	list<CGameObject*>* pMonsters = pGameInstance->Get_GameObjects(iLevelIndex, TEXT("Layer_Monster"));

	if (nullptr != pPlayerAtkColls && nullptr != pMonsters)
	{
		for (auto& pPlayerAtkColl : (*pPlayerAtkColls))
		{
			if (nullptr != pPlayerAtkColl)
			{
				for (auto& pMonster : (*pMonsters))
				{
					if (nullptr != pMonster)
					{
						CCollider* pAtkCollider = dynamic_cast<CCollider*>(pPlayerAtkColl->Find_Component(TEXT("Com_Sphere")));
						CCollider* pMonsterCollider = dynamic_cast<CCollider*>(pMonster->Find_Component(TEXT("Com_Sphere")));

						pAtkCollider->Intersect(pMonsterCollider);

						if (true == pMonsterCollider->Get_Coll())
						{
							Set_IsColl(1);

							m_iPAtoMCount++;
							//몬스터를 넉백
							CTransform* pMonsterTransform = dynamic_cast<CTransform*>(pMonster->Find_Component(TEXT("Com_Transform")));
							CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Player"), (TEXT("Com_Transform"))));

							_vector vDir = pMonsterTransform->Get_State(CTransform::STATE_POSITION) - pPlayerTransform->Get_State(CTransform::STATE_POSITION);

							pMonsterTransform->Go_Dir(dTimeDelta * 0.5, vDir);
							//pPlayerTransform->Go_Dir(dTimeDelta * 0.5, vDir * -1);
						}
					}
				}
			}
		}
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CColliderManager::Check_MonsterToMonster(_uint iLevelIndex, _double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*>* pMonsters = pGameInstance->Get_GameObjects(iLevelIndex, TEXT("Layer_Monster"));

	if (nullptr != pMonsters)
	{
		_uint iSourIndex = { 0 };
		for (auto& pSourMonster : (*pMonsters))
		{
			if (nullptr != pSourMonster)
			{
				_uint iDestIndex = { 0 };
				for (auto& pDestMonster : (*pMonsters))
				{
					if (nullptr != pDestMonster)
					{
						if (iSourIndex == iDestIndex)
						{
							iDestIndex++;
							continue;
						}

						CCollider* pSourCollider = dynamic_cast<CCollider*>(pSourMonster->Find_Component(TEXT("Com_Sphere")));
						CCollider* pDestCollider = dynamic_cast<CCollider*>(pDestMonster->Find_Component(TEXT("Com_Sphere")));
						pSourCollider->Intersect(pDestCollider);

						if (true == pDestCollider->Get_Coll())
						{
							Set_IsColl(2);

							CTransform* pSourTransform = dynamic_cast<CTransform*>(pSourMonster->Find_Component(TEXT("Com_Transform")));
							CTransform* pDestTransform = dynamic_cast<CTransform*>(pDestMonster->Find_Component(TEXT("Com_Transform")));
							
							_vector vDir = pDestTransform->Get_State(CTransform::STATE_POSITION) - pSourTransform->Get_State(CTransform::STATE_POSITION);

							if (0.f == Convert::GetLength(vDir))
							{
								vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
							}

							pDestTransform->Go_Dir(dTimeDelta * 0.5, vDir);
							pSourTransform->Go_Dir(dTimeDelta * 0.5, vDir * -1);
						}
						iDestIndex++;
					}
				}
				iSourIndex++;
			}
		}
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

void CColliderManager::Free()
{
}
