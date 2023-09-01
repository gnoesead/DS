#include "pch.h"
#include "..\Public\ColliderManager.h"

#include "GameInstance.h"
#include "Player.h"
#include "Collider.h"

#include "PlayerManager.h"

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

	if (FAILED(Check_PlayerToBoss(iLevelIndex, dTimeDelta)))
	{
		MSG_BOX("Failed to Check_PlayerToBoss");
		return E_FAIL;
	}

	if (FAILED(Check_PlayerAtkToBoss(iLevelIndex, dTimeDelta)))
	{
		MSG_BOX("Failed to Check_PlayerAtkToBoss");
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

	if (FAILED(Check_BossAtkToPlayer(iLevelIndex, dTimeDelta)))
	{
		MSG_BOX("Failed to Check_BossAtkToPlayer");
		return E_FAIL;
	}

	/*if (FAILED(Check_PlayerToCollisionBox(iLevelIndex, dTimeDelta)))
	{
		MSG_BOX("Failed to Check_PlayerToCollisionBoxr");
		return E_FAIL;
	}*/

	if (FAILED(Check_PlayerToNPC(iLevelIndex, dTimeDelta)))
	{
		MSG_BOX("Failed to Check_PlayerToNPC");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CColliderManager::Check_PlayerToMonster(_uint iLevelIndex, _double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player"), CPlayerManager::GetInstance()->Get_PlayerIndex()));
	
	//CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Player"), TEXT("Com_Sphere")));
	CCollider* pPlayerCollider = pPlayer->Get_ColliderCom();

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

					//CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Player"), (TEXT("Com_Transform"))));
					CTransform* pPlayerTransform = pPlayer->Get_TransformCom();

					_float fRad = pPlayerCollider->Get_Collider() + pMonsterCollider->Get_Collider();

					_vector vDir = pMonsterTransform->Get_State(CTransform::STATE_POSITION) - pPlayerTransform->Get_State(CTransform::STATE_POSITION);

					_float fDis = Convert::GetLength(vDir);

					if (fRad > fDis)
					{
						_vector vMoveDir = XMVector3Normalize(vDir); // 방향 벡터를 정규화
						//_float fMoveDistance = (fRad - fDis - 0.02f) / 2.0;
						//_float fMoveDistance = fRad - fDis - 0.02f;
						_float fMoveDistance = ((fRad - fDis) * 0.1f) / 2.0f;
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
							dynamic_cast<CCharacter*>(pMonster)->Add_HitCollider(pPlayerAtkColl);

							pMonsterCollider->Set_AtkDir(pAtkCollider->Get_AtkDir());
							pMonsterCollider->Set_fDamage(pAtkCollider->Get_fDamage());
						}
					}
				}
			}
		}
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CColliderManager::Check_PlayerToBoss(_uint iLevelIndex, _double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Player"), TEXT("Com_Sphere")));

	list<CGameObject*>* pMonsters = pGameInstance->Get_GameObjects(iLevelIndex, TEXT("Layer_Boss"));

	_int iCollCount = { 0 };

	if (nullptr != pPlayerCollider && nullptr != pMonsters)
	{
		for (auto& pMonster : (*pMonsters))
		{
			if (nullptr != pMonster)
			{
				CCollider* pMonsterCollider = dynamic_cast<CCollider*>(pMonster->Find_Component(TEXT("Com_Sphere")));

				if (pMonsterCollider->Get_Death() == false)
					pPlayerCollider->Intersect(pMonsterCollider);



				if (true == pPlayerCollider->Get_Coll())
					iCollCount++;

				if (true == pMonsterCollider->Get_Coll())
				{
					CTransform* pMonsterTransform = dynamic_cast<CTransform*>(pMonster->Find_Component(TEXT("Com_Transform")));

					CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Player"), (TEXT("Com_Transform"))));

					_float fRad = pPlayerCollider->Get_Collider() + pMonsterCollider->Get_Collider();

					_vector vDir = pMonsterTransform->Get_State(CTransform::STATE_POSITION) - pPlayerTransform->Get_State(CTransform::STATE_POSITION);

					_float fDis = Convert::GetLength(vDir);

					if (fRad > fDis)
					{
						_vector vMoveDir = XMVector3Normalize(vDir); // 방향 벡터를 정규화
						//_float fMoveDistance = (fRad - fDis - 0.02f) / 2.0;
						//_float fMoveDistance = fRad - fDis - 0.02f;
						_float fMoveDistance = ((fRad - fDis) * 0.1f) / 2.0f;
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

HRESULT CColliderManager::Check_PlayerAtkToBoss(_uint iLevelIndex, _double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*>* pPlayerAtkColls = pGameInstance->Get_GameObjects(LEVEL_STATIC, TEXT("Layer_PlayerAtk"));

	list<CGameObject*>* pMonsters = pGameInstance->Get_GameObjects(iLevelIndex, TEXT("Layer_Boss"));

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
							dynamic_cast<CCharacter*>(pMonster)->Add_HitCollider(pPlayerAtkColl);

							pMonsterCollider->Set_AtkDir(pAtkCollider->Get_AtkDir());
							pMonsterCollider->Set_fDamage(pAtkCollider->Get_fDamage());
						}
					}
				}
			}
		}
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CColliderManager::Check_PlayerToCollisionBox(_uint iLevelIndex, _double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Player"), TEXT("Com_Sphere")));

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(iLevelIndex, TEXT("Layer_Player")));

	if (pPlayer->Get_IsJumpOn() == true)
	{
		Safe_Release(pGameInstance);
		return S_OK;
	}

	list<CGameObject*>* pCollisionBoxes = pGameInstance->Get_GameObjects(iLevelIndex, TEXT("Layer_CollisionBox"));

	_int iCollCount = { 0 };

	if (nullptr != pPlayerCollider && nullptr != pCollisionBoxes)
	{
		for (auto& pCollisionBox : (*pCollisionBoxes))
		{
			if (nullptr != pCollisionBox)
			{
				CCollider* pAABBBox = dynamic_cast<CCollider*>(pCollisionBox->Find_Component(TEXT("Com_Sphere")));

				if (pAABBBox->Get_Death() == false)
					pPlayerCollider->Intersect(pAABBBox);



				if (true == pPlayerCollider->Get_Coll())
					iCollCount++;

				if (true == pAABBBox->Get_Coll())
				{
					CTransform* pAABBTransform = dynamic_cast<CTransform*>(pCollisionBox->Find_Component(TEXT("Com_Transform")));

					CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Player"), (TEXT("Com_Transform"))));

					_float fRad = pPlayerCollider->Get_Collider() + pAABBBox->Get_Desc().vSize.x;

					_vector vDir = pAABBTransform->Get_State(CTransform::STATE_POSITION) - pPlayerTransform->Get_State(CTransform::STATE_POSITION);

					_float fDis = Convert::GetLength(vDir);

					if (fRad > fDis)
					{
						_vector vMoveDir = XMVector3Normalize(vDir); // 방향 벡터를 정규화
						_float fMoveDistance = ((fRad - fDis) * 0.15f) / 2.0f;
						//_float fMoveDistance = fRad - fDis - 0.02f;
						_vector vMove = vMoveDir * fMoveDistance;

						_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
						vPlayerPos -= vMove;
						pPlayerTransform->Set_State(CTransform::STATE_POSITION, vPlayerPos);

					}

					//CTransform* pNPCTransform = dynamic_cast<CTransform*>(pNPC->Find_Component(TEXT("Com_Transform")));

					//CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Player"), (TEXT("Com_Transform"))));

					//_float fRad = pPlayerCollider->Get_Collider() + pNPCColl->Get_Collider();

					//_vector vDir = pNPCTransform->Get_State(CTransform::STATE_POSITION) - pPlayerTransform->Get_State(CTransform::STATE_POSITION);

					//_float fDis = Convert::GetLength(vDir);

					//if (fRad > fDis)
					//{
					//	_vector vMoveDir = XMVector3Normalize(vDir); // 방향 벡터를 정규화
					//	//_float fMoveDistance = (fRad - fDis - 0.02f) / 2.0;
					//	//_float fMoveDistance = fRad - fDis - 0.02f;
					//	_float fMoveDistance = ((fRad - fDis) * 0.1f) / 2.0f;
					//	_vector vMove = vMoveDir * fMoveDistance;

					//	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
					//	vPlayerPos -= vMove;
					//	pPlayerTransform->Set_State(CTransform::STATE_POSITION, vPlayerPos);
					//}



					
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
								_float fMoveDistance = ((fRad - fDis) * 0.65f) / 2.0f;
								_vector vMove = vMoveDir * fMoveDistance;

								_vector vPlayerPos = pSourTransform->Get_State(CTransform::STATE_POSITION);
								vPlayerPos -= vMove;
								pSourTransform->Set_State(CTransform::STATE_POSITION, vPlayerPos);

								_vector vMonPos = pDestTransform->Get_State(CTransform::STATE_POSITION);
								vMonPos += vMove;
								pDestTransform->Set_State(CTransform::STATE_POSITION, vMonPos);
							}
						
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
							dynamic_cast<CCharacter*>(pPlayers)->Add_HitCollider(pMonsterAtkColls);


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

HRESULT CColliderManager::Check_BossAtkToPlayer(_uint iLevelIndex, _double dTimeDelta)
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
							dynamic_cast<CCharacter*>(pPlayers)->Add_HitCollider(pMonsterAtkColls);


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

HRESULT CColliderManager::Check_PlayerToNPC(_uint iLevelIndex, _double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CCollider* pPlayerCollider = dynamic_cast<CCollider*>(pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Player"), TEXT("Com_Sphere")));

	list<CGameObject*>* pNPCs = pGameInstance->Get_GameObjects(iLevelIndex, TEXT("Layer_NPC"));

	_int iCollCount = { 0 };

	if (nullptr != pPlayerCollider && nullptr != pNPCs)
	{
		for (auto& pNPC : (*pNPCs))
		{
			if (nullptr != pNPC)
			{
				CCollider* pNPCColl = dynamic_cast<CCollider*>(pNPC->Find_Component(TEXT("Com_Sphere")));

				if (pNPCColl->Get_Death() == false)
					pPlayerCollider->Intersect(pNPCColl);



				if (true == pPlayerCollider->Get_Coll())
					iCollCount++;

				if (true == pNPCColl->Get_Coll())
				{
					CTransform* pNPCTransform = dynamic_cast<CTransform*>(pNPC->Find_Component(TEXT("Com_Transform")));

					CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pGameInstance->Get_Component(iLevelIndex, TEXT("Layer_Player"), (TEXT("Com_Transform"))));

					_float fRad = pPlayerCollider->Get_Collider() + pNPCColl->Get_Collider();

					_vector vDir = pNPCTransform->Get_State(CTransform::STATE_POSITION) - pPlayerTransform->Get_State(CTransform::STATE_POSITION);

					_float fDis = Convert::GetLength(vDir);

					if (fRad > fDis)
					{
						_vector vMoveDir = XMVector3Normalize(vDir); // 방향 벡터를 정규화
						//_float fMoveDistance = (fRad - fDis - 0.02f) / 2.0;
						//_float fMoveDistance = fRad - fDis - 0.02f;
						_float fMoveDistance = ((fRad - fDis) * 0.1f) / 2.0f;
						_vector vMove = vMoveDir * fMoveDistance;

						_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
						vPlayerPos -= vMove;
						pPlayerTransform->Set_State(CTransform::STATE_POSITION, vPlayerPos);
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

void CColliderManager::Free()
{
}
