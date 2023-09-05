#include "pch.h"
#include "..\Public\InstanceMapObject.h"

#include "GameInstance.h"
#include "Player.h"

CInstanceMapObject::CInstanceMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapObject(pDevice, pContext)
{
}

CInstanceMapObject::CInstanceMapObject(const CInstanceMapObject& rhs)
	: CMapObject(rhs)
{
}

HRESULT CInstanceMapObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstanceMapObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	memcpy(&m_MapObject_Info, pArg, sizeof m_MapObject_Info);

	wcscpy_s(m_PrototypeObjectTag, m_MapObject_Info.szMeshName);

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_MapObject_Info.vPos));
	m_pTransformCom->Rotation(_float3(m_MapObject_Info.vRotAngle.x, m_MapObject_Info.vRotAngle.y, m_MapObject_Info.vRotAngle.z));
	m_pTransformCom->Scaling(m_MapObject_Info.vScale);

	m_MapObject_Info.iMapObjectType = MAPOBJECT_INSTANCE;

	return S_OK;
}

void CInstanceMapObject::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_VILLAGE)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")));
		if (pPlayer->Get_CurNaviMesh() == CLandObject::NAVI_VILLAGE_BATTLE)
		{
			if (m_MapObject_Info.iRenderGroup == 0)
			{
				Safe_Release(pGameInstance);
				return;
			}
		}
		else
		{
			if (m_MapObject_Info.iRenderGroup == 3 || m_MapObject_Info.iRenderGroup == 2 || m_MapObject_Info.iRenderGroup == 1)
			{
				Safe_Release(pGameInstance);
				return;
			}
		}
	}
	
	__super::Tick(TimeDelta);

	if (m_MapObject_Info.iInstanceType == INSTANCE_GRASS)
	{
		/*m_fRatio += (_float)TimeDelta * m_fDir;

		if (m_fRatio > 1.f)
		{
			m_fRatio = 1.f;
			m_fDir = -1.f;
		}

		if (m_fRatio < -1.f)
		{
			m_fRatio = -1.f;
			m_fDir = 1.f;
		}*/
	}

	if(LEVEL_TRAIN == pGameInstance->Get_CurLevelIdx())
		Scroll(TimeDelta);
	
	Safe_Release(pGameInstance);
		
	//m_pModelInstanceCom->Tick(TimeDelta);
}

void CInstanceMapObject::LateTick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Get_CurLevelIdx() == LEVEL_VILLAGE)
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObject(pGameInstance->Get_CurLevelIdx(), TEXT("Layer_Player")));
		if (pPlayer->Get_CurNaviMesh() == CLandObject::NAVI_VILLAGE_BATTLE)
		{
			if (m_MapObject_Info.iRenderGroup == 0)
			{
				Safe_Release(pGameInstance);
				return;
			}
		}
		else
		{
			if (m_MapObject_Info.iRenderGroup == 3 || m_MapObject_Info.iRenderGroup == 2 || m_MapObject_Info.iRenderGroup == 1)
			{
				Safe_Release(pGameInstance);
				return;
			}
		}
	}

	__super::LateTick(TimeDelta);

	if (m_MapObject_Info.iArrangementType != ARRANGEMENT_RECT || pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 60.f))
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		{
			Safe_Release(pGameInstance);
			return;
		}
	}

	Safe_Release(pGameInstance);
}

HRESULT CInstanceMapObject::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelInstanceCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelInstanceCom->Bind_ShaderResource(m_pShaderCom, i, "g_DiffuseTexture", MESHMATERIALS::TextureType_DIFFUSE)))
			return E_FAIL;

		/*if (m_pModelCom->Get_IsNormalTexture(i))
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_NormalTexture", MESHMATERIALS::TextureType_NORMALS)))
				return E_FAIL;
		}*/

		/*if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_EmissiveTexture", MESHMATERIALS::TextureType_EMISSIVE)))
			return E_FAIL;*/

		if (m_MapObject_Info.iInstanceType == INSTANCE_GRASS)
			m_pShaderCom->Begin(1);
		else
			m_pShaderCom->Begin(0);

		m_pModelInstanceCom->Render(i);
	}

	return S_OK;
}

HRESULT CInstanceMapObject::Add_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModelInstance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	MODELINSTANCEDESC tModelInstanceDesc;
	ZeroMemory(&tModelInstanceDesc, sizeof tModelInstanceDesc);
	tModelInstanceDesc.iNumInstance = m_MapObject_Info.iNumInstance;
	tModelInstanceDesc.fRange = m_MapObject_Info.fRange;
	tModelInstanceDesc.fMinSize = m_MapObject_Info.fMinSize;
	tModelInstanceDesc.fMaxSize = m_MapObject_Info.fMaxSize;
	tModelInstanceDesc.bRandomRatationY = m_MapObject_Info.bRandomRatationY;
	tModelInstanceDesc.iArrangementType = m_MapObject_Info.iArrangementType;

	
	LEVELID eLevelID = LEVEL_END;

	switch (m_MapObject_Info.iSceneType)
	{
	case SCENE_STATIC:
		eLevelID = LEVEL_STATIC;
		break;
	case SCENE_GAMEPLAY:
		eLevelID = LEVEL_GAMEPLAY;
		break;
	case SCENE_VILLAGE:
		eLevelID = LEVEL_VILLAGE;
		break;
	case SCENE_HOUSE:
		eLevelID = LEVEL_HOUSE;
		break;
	case SCENE_TRAIN:
		eLevelID = LEVEL_TRAIN;
		break;
	case SCENE_FINALBOSS:
		eLevelID = LEVEL_FINALBOSS;
		break;
	}

	/* For.Com_ModelInstance */
	if (FAILED(__super::Add_Component(eLevelID, m_PrototypeObjectTag,
		TEXT("Com_ModelInstance"), (CComponent**)&m_pModelInstanceCom , &tModelInstanceDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstanceMapObject::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_RawValue("g_fRatio", &m_fRatio, sizeof _float)))
		return E_FAIL;
	

	Safe_Release(pGameInstance);

	return S_OK;
}

CInstanceMapObject* CInstanceMapObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInstanceMapObject* pInstance = new CInstanceMapObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInstanceMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInstanceMapObject::Clone(void* pArg)
{
	CInstanceMapObject* pInstance = new CInstanceMapObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInstanceMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInstanceMapObject::Free()
{
	__super::Free();

	Safe_Release(m_pModelInstanceCom);
}
