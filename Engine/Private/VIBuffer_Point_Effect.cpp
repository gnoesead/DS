#include "..\Public\VIBuffer_Point_Effect.h"



CVIBuffer_Point_Effect::CVIBuffer_Point_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Point_Effect::CVIBuffer_Point_Effect(const CVIBuffer_Point_Effect & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Point_Effect::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumVertices = 1;
	m_iNumIndices = 1;
	m_iStride = sizeof(VTXPOS);
	m_iIndexStride = sizeof(_ushort);

	m_iNumVertexBuffers = 1;

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;	//인덱스 한칸에 2바이트
											// DXGI_FORMAT_R32_UINT : 한칸에 4바이트
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;



	/* 버텍스 버퍼를 할당한다. */
#pragma region VERTEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS));

	pVertices->vPosition = _float3(0.0f, 0.0f, 0.f);

	ZeroMemory(&m_SubresourceData, sizeof m_SubresourceData);
	m_SubresourceData.pSysMem = pVertices;
	
	if(FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubresourceData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion

	/* 인덱스 버퍼를 할당한다. */
#pragma region INDEX_BUFFER
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	pIndices[0] = 0;

	ZeroMemory(&m_SubresourceData, sizeof m_SubresourceData);
	m_SubresourceData.pSysMem = pIndices;

	if(FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubresourceData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Point_Effect::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Point_Effect * CVIBuffer_Point_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Point_Effect* pInstance = new CVIBuffer_Point_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CVIBuffer_Point_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Point_Effect::Clone(void * pArg)
{
	CVIBuffer_Point_Effect* pInstance = new CVIBuffer_Point_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CVIBuffer_Point_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Effect::Free(void)
{
	__super::Free();
}
