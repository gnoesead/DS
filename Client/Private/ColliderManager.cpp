#include "pch.h"
#include "..\Public\ColliderManager.h"

#include "GameInstance.h"
#include "Player.h"

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
		MSG_BOX("Failed to Check_PlayerAtkToMonster");
		return E_FAIL;
	}

	if (FAILED(Check_MonsterToMonster(iLevelIndex, dTimeDelta)))
	{
		MSG_BOX("Failed to Check_MonsterToMonster");
		return E_FAIL;
	}

	if (FAILED(Check_MonsterAtkToPlayer(iLevelIndex, dTimeDelta)))
	{
		MSG_BOX("Failed to Check_MonsterAtkToPlayer");
		return E_FAIL;
	}

	return S_OK;
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
				
				if(pMonsterCollider->Get_Death() == false)
					pPlayerCollider->Intersect(pMonsterCollider);

				

				if (true == pPlayerCollider->Get_Coll())
					iCollCount++;

				if (true == pMonsterCollider->Get_Coll())
				{
					CTransform* pMonsterTransform = dynamic_cast<CTransform*>(pMonster->Find_Component(TEXT("Com_Transform")));

					CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Player"), (TEXT("Com_Transform"))));

					/*
					_vector vDir = pMonsterTransform->Get_State(CTransform::STATE_POSITION) - pPlayerTransform->Get_State(CTransform::STATE_POSITION);

					pMonsterTransform->Go_Dir(dTimeDelta * 0.2, vDir);
					pPlayerTransform->Go_Dir(dTimeDelta * 0.2, vDir * -1);
					*/
					_float fRad = pPlayerCollider->Get_Collider() + pMonsterCollider->Get_Collider();

					_vector vDir = pMonsterTransform->Get_State(CTransform::STATE_POSITION) - pPlayerTransform->Get_State(CTransform::STATE_POSITION);

					_float fDis = Convert::GetLength(vDir);

					if (fRad > fDis)
					{
						_vector vMoveDir = XMVector3Normalize(vDir); // 방향 벡터를 정규화
						//_float fMoveDistance = (fRad - fDis - 0.02f) / 2.0;
						//_float fMoveDistance = fRad - fDis - 0.02f;
						_float fMoveDistance = ((fRad - fDis) * 0.1f) / 2.0;
						_vector vMove = vMoveDir * fMoveDistance;

						_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
						vPlayerPos -= vMove;
						pPlayerTransform->Set_State(CTransform::STATE_POSITION, vPlayerPos);

						_vector vMonPos = pMonsterTransform->Get_State(CTransform::STATE_POSITION);
						vMonPos += vMove;
						pMonsterTransform->Set_State(CTransform::STATE_POSITION, vMonPos);
					}
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
							if (pAtkCollider->Get_Hit_Small())
							{
								pMonsterCollider->Set_Hit_Small(true);
							}
							else if (pAtkCollider->Get_Hit_Big())
							{
								pMonsterCollider->Set_Hit_Big(true);
							}
							else if (pAtkCollider->Get_Hit_Blow())
							{
								pMonsterCollider->Set_Hit_Blow(true);
							}
							else if (pAtkCollider->Get_Hit_Spin())
							{
								pMonsterCollider->Set_Hit_Spin(true);
							}

							pMonsterCollider->Set_AtkDir(pAtkCollider->Get_AtkDir());
							pMonsterCollider->Set_fDamage(pAtkCollider->Get_fDamage());

							/*//몬스터를 넉백
							CTransform* pMonsterTransform = dynamic_cast<CTransform*>(pMonster->Find_Component(TEXT("Com_Transform")));
							CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Player"), (TEXT("Com_Transform"))));

							_vector vDir = pMonsterTransform->Get_State(CTransform::STATE_POSITION) - pPlayerTransform->Get_State(CTransform::STATE_POSITION);

							pMonsterTransform->Go_Dir(dTimeDelta * 0.5, vDir);
							//pPlayerTransform->Go_Dir(dTimeDelta * 0.5, vDir * -1);*/
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

						if (pSourCollider->Get_SphereOff() == false && pDestCollider->Get_SphereOff() == false)
						{
							pSourCollider->Intersect(pDestCollider);
						}

						if (true == pDestCollider->Get_Coll())
						{
							CTransform* pSourTransform = dynamic_cast<CTransform*>(pSourMonster->Find_Component(TEXT("Com_Transform")));
							CTransform* pDestTransform = dynamic_cast<CTransform*>(pDestMonster->Find_Component(TEXT("Com_Transform")));

							_float fRad = pSourCollider->Get_Collider() + pDestCollider->Get_Collider();

							_vector vDir = pDestTransform->Get_State(CTransform::STATE_POSITION) - pSourTransform->Get_State(CTransform::STATE_POSITION);

							_float fDis = Convert::GetLength(vDir);

							if (fRad > fDis)
							{
								_vector vMoveDir = XMVector3Normalize(vDir); // 방향 벡터를 정규화
								//_float fMoveDistance = (fRad - fDis - 0.05f) / 2.0f;
								//_float fMoveDistance = fRad - fDis - 0.1f;
								_float fMoveDistance = ((fRad - fDis) * 0.95f) / 2.0;
								_vector vMove = vMoveDir * fMoveDistance;

								_vector vPlayerPos = pSourTransform->Get_State(CTransform::STATE_POSITION);
								vPlayerPos -= vMove;
								pSourTransform->Set_State(CTransform::STATE_POSITION, vPlayerPos);

								_vector vMonPos = pDestTransform->Get_State(CTransform::STATE_POSITION);
								vMonPos += vMove;
								pDestTransform->Set_State(CTransform::STATE_POSITION, vMonPos);
							}

							/*CTransform* pSourTransform = dynamic_cast<CTransform*>(pSourMonster->Find_Component(TEXT("Com_Transform")));
							CTransform* pDestTransform = dynamic_cast<CTransform*>(pDestMonster->Find_Component(TEXT("Com_Transform")));
							
							_vector vDir = pDestTransform->Get_State(CTransform::STATE_POSITION) - pSourTransform->Get_State(CTransform::STATE_POSITION);

							if (0.f == Convert::GetLength(vDir))
							{
								vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
							}

							pDestTransform->Go_Dir(dTimeDelta * 0.5, vDir);
							pSourTransform->Go_Dir(dTimeDelta * 0.5, vDir * -1);*/
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

HRESULT CColliderManager::Check_MonsterAtkToPlayer(_uint iLevelIndex, _double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*>* pMonsterAtkColls = pGameInstance->Get_GameObjects(LEVEL_STATIC, TEXT("Layer_MonsterAtk"));

	list<CGameObject*>* pPlayers = pGameInstance->Get_GameObjects(iLevelIndex, TEXT("Layer_Player"));

	if (nullptr != pMonsterAtkColls && nullptr != pPlayers)
	{
		for (auto& pMonsterAtkColls : (*pMonsterAtkColls))
		{
			if (nullptr != pMonsterAtkColls)
			{
				for (auto& pPlayers : (*pPlayers))
				{
					if (nullptr != pPlayers)
					{
						CCollider* pAtkCollider = dynamic_cast<CCollider*>(pMonsterAtkColls->Find_Component(TEXT("Com_Sphere")));
						CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pPlayers->Find_Component(TEXT("Com_Sphere")));

						pAtkCollider->Intersect(pPlayerCollider);

						if (true == pPlayerCollider->Get_Coll())
						{
							if (pAtkCollider->Get_Hit_Small())
							{
								pPlayerCollider->Set_Hit_Small(true);
							}
							else if (pAtkCollider->Get_Hit_Big())
							{
								pPlayerCollider->Set_Hit_Big(true);
							}
							else if (pAtkCollider->Get_Hit_Blow())
							{
								pPlayerCollider->Set_Hit_Blow(true);
							}
							else if (pAtkCollider->Get_Hit_Spin())
							{
								pPlayerCollider->Set_Hit_Spin(true);
							}

							pPlayerCollider->Set_AtkDir(pAtkCollider->Get_AtkDir());
							pPlayerCollider->Set_fDamage(pAtkCollider->Get_fDamage());

							
						}
					}
				}
			}
		}
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

void CColliderManager::Free()
{
}
