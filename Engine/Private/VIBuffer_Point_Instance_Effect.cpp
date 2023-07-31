#include "..\Public\VIBuffer_Point_Instance_Effect.h"

#include "GameInstance.h"

CVIBuffer_Point_Instance_Effect::CVIBuffer_Point_Instance_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instance(pDevice, pContext)
{

}

CVIBuffer_Point_Instance_Effect::CVIBuffer_Point_Instance_Effect(const CVIBuffer_Point_Instance_Effect & rhs)
	: CVIBuffer_Instance(rhs)
{
}

HRESULT CVIBuffer_Point_Instance_Effect::Initialize_Prototype(_uint iNumInstance)
{
	m_iMaxNumInstance = iNumInstance;
	m_iMaxNumInstance = 100;
	return S_OK;
}

HRESULT CVIBuffer_Point_Instance_Effect::Initialize(void * pArg)
{
	m_iNumInstance = m_iMaxNumInstance;
	m_iNumIndicesPerInstance = 1;
	m_iNumVertexBuffers = 2;
	m_iStride = sizeof(VTXPOS);
	m_iNumVertices = 1;
	m_iNumIndices = m_iMaxNumInstance;
	m_iIndexStride = sizeof(_ushort);
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_iInstanceStride = sizeof(VTXINSTANCEEFFECT);


	/* 버텍스 버퍼를 할당한다. */
#pragma region VERTEXBUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXPOS*		pVertices = new VTXPOS;
	ZeroMemory(pVertices, sizeof(VTXPOS));

	pVertices->vPosition = _float3(0.0f, 0.0f, 0.f);

	ZeroMemory(&m_SubresourceData, sizeof m_SubresourceData);
	m_SubresourceData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubresourceData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion


#pragma region INDEXBUFFER	
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort*		pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	for (_uint i = 0; i < m_iNumIndices; ++i)
		pIndices[i] = 0;

	ZeroMemory(&m_SubresourceData, sizeof m_SubresourceData);
	m_SubresourceData.pSysMem = pIndices;

	/* 인덱스 버퍼를 할당한다. */
	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubresourceData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	if (nullptr == pArg)
		return E_FAIL;

	INSTANCEDESC	InstanceDesc = *(INSTANCEDESC*)pArg;
	m_InstanceDesc = InstanceDesc;

	m_pParticleDescriptions = new PARTICLEDESC[m_iMaxNumInstance];

	for (_uint i = 0; i < m_iMaxNumInstance; ++i)
	{
		m_pParticleDescriptions[i].fSpeed = rand() % _uint(InstanceDesc.fMaxSpeed - InstanceDesc.fMinSpeed + 1) + (_uint)InstanceDesc.fMinSpeed;
	}

#pragma region INSTANCEBUFFER	
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iMaxNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;

	VTXINSTANCEEFFECT*		pInstanceVertices = new VTXINSTANCEEFFECT[m_iMaxNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXINSTANCEEFFECT) * m_iMaxNumInstance);

	for (_uint i = 0; i < m_iMaxNumInstance; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		
		switch (m_InstanceDesc.eShapeType)
		{
		case 2:
		{
			_float fAngle = Random::Generate_Int(0, 360);
			pInstanceVertices[i].vTranslation = _float4(cosf(XMConvertToRadians(fAngle)), 0.f, sinf(XMConvertToRadians(fAngle)), 1.f);
		}
		break;
		case 4:
		{
			pInstanceVertices[i].vTranslation = _float4(Random::Generate_Float(-10, 10) * 0.1f + 0.001f,
				Random::Generate_Float(-10, 10) * 0.1f + 0.001f,
				Random::Generate_Float(-10, 10) * 0.1f + 0.001f, 1.f);
		}
		break;
		default:
			pInstanceVertices[i].vTranslation = _float4(Random::Generate_Float(InstanceDesc.vRange.x * -1, InstanceDesc.vRange.x),
				Random::Generate_Float(InstanceDesc.vRange.y * -1, InstanceDesc.vRange.y),
				Random::Generate_Float(InstanceDesc.vRange.z * -1, InstanceDesc.vRange.z), 1.f);
			break;
		}

		pInstanceVertices[i].vColor = _float4(1.0f, 1.f, rand() % 2, 0.f);
		pInstanceVertices[i].vPSize = _float2(0.3f, 0.3f);
		pInstanceVertices[i].vAdditional = _float4(m_InstanceDesc.fStartLifeTimeMin, 0.f, 0.f, 0.f);
	}

	ZeroMemory(&m_SubresourceData, sizeof m_SubresourceData);
	m_SubresourceData.pSysMem = pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubresourceData, &m_pVBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVertices);

#pragma endregion

	return S_OK;
}

void CVIBuffer_Point_Instance_Effect::Tick(_double TimeDelta, INSTANCEDESC* pDesc)
{
	m_dTimeAcc += TimeDelta;

	if (m_dTimeAcc > pDesc->fLifetime / m_iMaxNumInstance)
	{
		++m_iNumInstance;
		m_dTimeAcc = 0.0;
	}

	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = m_iStartIndex; i < m_iNumInstance; i++)
	{ 
		// Lifetime
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x -= TimeDelta;

		if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x < 0)		// Lifetime < 0
		{
			if (!pDesc->isLooping)
				m_iStartIndex = i + 1;
			else
			{
				switch (pDesc->eShapeType)
				{
				case 2:		// CIRCLE
				{
					_float fAngle = Random::Generate_Int(0, 360);
					((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation = _float4(cosf(XMConvertToRadians(fAngle)), 0.f, sinf(XMConvertToRadians(fAngle)), 1.f);
				}
				break;
				case 4:		// SPHERE
				{
					((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation = _float4(Random::Generate_Float(-10, 10) * 0.1f + 0.001f,
						Random::Generate_Float(-10, 10) * 0.1f + 0.001f,
						Random::Generate_Float(-10, 10) * 0.1f + 0.001f, 1.f);
				}
				break;
				default:
					((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation.y = rand() % (_int(pDesc->vRange.y) + 1) - _int(pDesc->vRange.y * 0.5f);
					break;
				}
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x = pDesc->fStartLifeTimeMin;
			}
		}

		if(m_iNumInstance >= m_iMaxNumInstance)
			m_iNumInstance = m_iMaxNumInstance - 1;

		switch (pDesc->eShapeType)
		{
		case 2: case 4:		// CIRCLE, SPHERE
		{
			_vector vPos = XMLoadFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation);
			_vector vLook = XMVector3Normalize(vPos - XMVectorSet(0.f, 0.f, 0.f, 1.f));
			_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
			_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vRight, vRight);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vUp, vUp);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLook, vLook);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation, vPos + vLook * m_pParticleDescriptions[i].fSpeed * TimeDelta);
		}
		break;
		default:
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation.y += m_pParticleDescriptions[i].fSpeed * TimeDelta;
			break;
		}
		
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

CVIBuffer_Point_Instance_Effect * CVIBuffer_Point_Instance_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iNumInstance)
{
	CVIBuffer_Point_Instance_Effect*		pInstance = new CVIBuffer_Point_Instance_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumInstance)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Point_Instance_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Point_Instance_Effect::Clone(void * pArg)
{
	CVIBuffer_Point_Instance_Effect*		pInstance = new CVIBuffer_Point_Instance_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Point_Instance_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Instance_Effect::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pParticleDescriptions);
}
