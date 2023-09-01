#include "pch.h"
#include "..\Public\Sword.h"

#include "GameInstance.h"
#include "Bone.h"

CSword::CSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCharacter_Tool(pDevice, pContext)
{
}

CSword::CSword(const CSword& rhs)
	: CCharacter_Tool(rhs)
{
}

HRESULT CSword::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSword::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	memcpy(&m_SwordDesc, pArg, sizeof m_SwordDesc);
	Safe_AddRef(m_SwordDesc.pBone);
	Safe_AddRef(m_SwordDesc.pParentTransform);

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(_float3(1.0f, 1.0f, 1.0f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CSword::Tick(_double TimeDelta)
{
	//__super::Tick(TimeDelta);
}

void CSword::LateTick(_double TimeDelta)
{
	//__super::LateTick(TimeDelta);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() *
		m_SwordDesc.pBone->Get_OffsetMatrix() *
		Remove_Scale(m_SwordDesc.pBone->Get_CombinedTransformationMatrix()) *
		m_SwordDesc.pBone->Get_PivotMatrix() *
		m_SwordDesc.pParentTransform->Get_WorldMatrix());
}

HRESULT CSword::Render()
{
	if (m_isSwordIn == false)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;


		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;

		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_DiffuseTexture", MESHMATERIALS::TextureType_DIFFUSE)))
				return E_FAIL;

			if (m_pModelCom->Get_IsNormalTexture(i))
			{
				if (FAILED(m_pModelCom->Bind_ShaderResource(i, m_pShaderCom, "g_NormalTexture", MESHMATERIALS::TextureType_NORMALS)))
					return E_FAIL;
			}

			if (m_pModelCom->Get_IsNormalTexture(i))
				m_pShaderCom->Begin(1);		// 노말 텍스쳐 적용 Pass	
			else
				m_pShaderCom->Begin(0);		// 노말 텍스쳐 미적용 Pass

			m_pModelCom->Render(i);
		}
	}
	return S_OK;
}

_matrix CSword::Remove_Scale(_fmatrix TransformMatrix)
{
	_matrix			ResultMatrix = TransformMatrix;

	ResultMatrix.r[0] = XMVector3Normalize(TransformMatrix.r[0]);
	ResultMatrix.r[1] = XMVector3Normalize(TransformMatrix.r[1]);
	ResultMatrix.r[2] = XMVector3Normalize(TransformMatrix.r[2]);

	return ResultMatrix;
}

HRESULT CSword::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Model_Rengoku_Sword"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	return S_OK;
}

HRESULT CSword::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 ViewMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->SetUp_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;


	Safe_Release(pGameInstance);
	return S_OK;
}

CSword* CSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSword* pInstance = new CSword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSword::Clone(void* pArg)
{
	CSword* pInstance = new CSword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSword::Free()
{
	__super::Free();

	
	Safe_Release(m_SwordDesc.pBone);
	
	Safe_Release(m_SwordDesc.pParentTransform);


	Safe_Release(m_pTransformCom);
	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	
	Safe_Release(m_pModelCom);
}
