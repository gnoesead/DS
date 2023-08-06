#include "..\Public\Cell.h"

#ifdef _DEBUG
#include "PipeLine.h"
#include "Shader.h"
#include "VIBuffer_Cell.h"
#endif

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3* pPoints, _uint iIndex)
{
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	_float2 vTemp = { 0.f , 0.f };

	m_iIndex = iIndex;

	XMStoreFloat3(&m_vNormals[LINE_AB], XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
	vTemp = { m_vNormals[LINE_AB].x , m_vNormals[LINE_AB].z };
	XMStoreFloat2(&vTemp, XMVector2Normalize(XMLoadFloat2(&vTemp)));
	m_vNormals[LINE_AB] = _float3(vTemp.y * -1.f, 0.f, vTemp.x);

	XMStoreFloat3(&m_vNormals[LINE_BC], XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
	vTemp = { m_vNormals[LINE_BC].x , m_vNormals[LINE_BC].z };
	XMStoreFloat2(&vTemp, XMVector2Normalize(XMLoadFloat2(&vTemp)));
	m_vNormals[LINE_BC] = _float3(vTemp.y * -1.f, 0.f, vTemp.x);

	XMStoreFloat3(&m_vNormals[LINE_CA], XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));
	vTemp = { m_vNormals[LINE_CA].x , m_vNormals[LINE_CA].z };
	XMStoreFloat2(&vTemp, XMVector2Normalize(XMLoadFloat2(&vTemp)));
	m_vNormals[LINE_CA] = _float3(vTemp.y * -1.f, 0.f, vTemp.x);


#ifdef _DEBUG
	if (FAILED(Ready_Debug()))
		return E_FAIL;
#endif

	return S_OK;
}

_bool CCell::Compare_Points(const _float3* pSourPoint, const _float3* pDestPoint)
{
	if (true == XMVector3Equal(XMLoadFloat3(pSourPoint), XMLoadFloat3(&m_vPoints[POINT_A])))
	{
		if (true == XMVector3Equal(XMLoadFloat3(pDestPoint), XMLoadFloat3(&m_vPoints[POINT_B])))
			return true;

		else if (true == XMVector3Equal(XMLoadFloat3(pDestPoint), XMLoadFloat3(&m_vPoints[POINT_C])))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(pSourPoint), XMLoadFloat3(&m_vPoints[POINT_B])))
	{
		if (true == XMVector3Equal(XMLoadFloat3(pDestPoint), XMLoadFloat3(&m_vPoints[POINT_C])))
			return true;

		else if (true == XMVector3Equal(XMLoadFloat3(pDestPoint), XMLoadFloat3(&m_vPoints[POINT_A])))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(pSourPoint), XMLoadFloat3(&m_vPoints[POINT_C])))
	{
		if (true == XMVector3Equal(XMLoadFloat3(pDestPoint), XMLoadFloat3(&m_vPoints[POINT_A])))
			return true;

		else if (true == XMVector3Equal(XMLoadFloat3(pDestPoint), XMLoadFloat3(&m_vPoints[POINT_B])))
			return true;
	}

	return false;
}

_bool CCell::isIn(_fvector vPosition, _int* pNeighborIndex)
{
	for (_uint i = 0; i < LINE_END; ++i)
	{
		_vector		vSour = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));
		_vector		vDest = XMLoadFloat3(&m_vNormals[i]);

		if (0.f < XMVectorGetX(XMVector3Dot(vSour, vDest)))
		{
			*pNeighborIndex = m_iNeighborIndex[i];
			return false;
		}
	}

	return true;
}

_bool CCell::Check_Point(_float* pPoint, _float fRadius)
{
	_vector vSourPoint = { pPoint[0] , pPoint[1] ,pPoint[2] ,1.f };
	_vector vDestPoint;

	for (_uint i = 0; i < POINT_END; ++i)
	{
		vDestPoint = XMLoadFloat3(&m_vPoints[i]);

		_float fLength = XMVectorGetX(XMVector3Length(vSourPoint - vDestPoint));

		if (fLength <= fRadius)
		{
			pPoint[0] = m_vPoints[i].x;
			pPoint[1] = m_vPoints[i].y;
			pPoint[2] = m_vPoints[i].z;
			return true;
		}
	}

	return false;

}

void CCell::Get_SlidingVector(_fvector& vPosition, _fvector vLook)
{
	_vector vSlidingVector;
	for (_uint i = 0; i < LINE_END; ++i)
	{
		_vector vSour = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));
		_vector vDest = XMVector3Normalize(XMLoadFloat3(&m_vNormals[i]));
		_float Scalar = XMVectorGetX(XMVector3Dot(vSour, vDest));

		if (0.f < Scalar)
		{
			if (0.f < XMVectorGetX(XMVector3Dot(vLook, vDest)))
				vSlidingVector = (vLook - (XMVector3Dot(vLook, XMVector3Normalize(vDest)) * XMVector3Normalize(vDest)));
			else
				vSlidingVector = vLook;
		}
	}
}

#ifdef _DEBUG

HRESULT CCell::Ready_Debug()
{
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG
HRESULT CCell::Render_Debug()
{
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	return m_pVIBuffer->Render();
}
#endif // _DEBUG

#endif
CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _uint iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("Failed to Created : CCell");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CCell::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
