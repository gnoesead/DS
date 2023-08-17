#include "..\Public\Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "PipeLine.h"
#include "GameInstance.h"
#include "Transform.h"

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)

{
}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
#endif // _DEBUG
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif // _DEBUG
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationDataFile)
{
#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS_DECL::Elements, VTXPOS_DECL::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif // _DEBUG 

	_ulong	dwByte = { 0 };

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	
	HANDLE	hFile = CreateFile(pNavigationDataFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[CCell::POINT_END];

	while (true)
	{
		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_uint)m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}


	CloseHandle(hFile);

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;


	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_NaviDesc = *(NAVIDESC*)pArg;
	}

	return S_OK;
}

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDestCell);
			}

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDestCell);
			}
		}
	}
	
	return S_OK;
}

_bool CNavigation::is_MoveOnNavigation(_fvector& vPosition)
{
	if (-1 == m_NaviDesc.iCurrentIndex)
		return false;

	_int	iNeighborIndex = { -1 };

	if (true == m_Cells[m_NaviDesc.iCurrentIndex]->isIn(vPosition, &iNeighborIndex))		// 셀들중 플레이어가 존재하는 셀에게 vPosition이 아직 너 안에 있냐 라고 물어보는것이다.
	{
		return true;
	}
	else /* 밖으로 나갔다. */
	{
		/* 나간방향에 iNeighborInex 이웃이 있다. */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex))
					break;
			}

			m_NaviDesc.iCurrentIndex = iNeighborIndex;		// 현재 인덱스 갱신
			return true;
		}
		else /* 이웃이 없다 */
		{
			// 필요하면 벽타라

			//Get_SlidingVector(vPosition, vLook, m_NaviDesc.iCurrentIndex);
			return false;
		}
	}
	return _bool();
}

_vector CNavigation::MoveOnNavigation(_fvector& vPosition, _fvector vLook)
{
	if (-1 == m_NaviDesc.iCurrentIndex)
		return  SlidingVector(vPosition, vLook, &m_NaviDesc.iCurrentIndex);

	_int	iNeighborIndex = { -1 };

	if (true == m_Cells[m_NaviDesc.iCurrentIndex]->isIn(vPosition, &iNeighborIndex))
	{

		return vLook;
	}
	else	 /* 밖으로 나갔다. */
	{

		/* 나간방향에 iNeighborIndex 이웃이 있다. */
		if (-1 < iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return  SlidingVector(vPosition, vLook, &m_NaviDesc.iCurrentIndex);

				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex))
					break;
			}

			m_NaviDesc.iCurrentIndex = iNeighborIndex;
			return vLook;
		}
		else
		{
			/*if (false == m_Cells[m_NaviDesc.iCurrentIndex]->isOut(vPosition, &m_NaviDesc.iCurrentIndex))
				return vLook;*/
			return  SlidingVector(vPosition, vLook, &m_NaviDesc.iCurrentIndex);
		}
	}

	return  XMVectorSet(0.f, 0.f, 0.f, 0.f);
}

_float CNavigation::Compute_Height(CTransform* pOwnerTransform)
{
	_vector vPlane;

	_vector vOwnerPos = pOwnerTransform->Get_State(CTransform::STATE_POSITION);

	vPlane = XMPlaneFromPoints(m_Cells[m_NaviDesc.iCurrentIndex]->Get_PointVector(CCell::POINT_A),
		m_Cells[m_NaviDesc.iCurrentIndex]->Get_PointVector(CCell::POINT_B),
		m_Cells[m_NaviDesc.iCurrentIndex]->Get_PointVector(CCell::POINT_C));

	// y = (-ax -cz - d) / b

	_float		fHeight = (-XMVectorGetX(vPlane) * XMVectorGetX(vOwnerPos)
		- XMVectorGetZ(vPlane) * XMVectorGetZ(vOwnerPos)
		- XMVectorGetW(vPlane)) / XMVectorGetY(vPlane) - 0.05f;

	return fHeight;
}

_vector CNavigation::SlidingVector(_fvector vPosition, _fvector vLook, _int* pNeighborIndex)
{
	_vector SlidingVector;
	//if(m_NaviDesc.iCurrentIndex > -1)
	SlidingVector = m_Cells[*pNeighborIndex]->Get_SlidingVector(vPosition, vLook, pNeighborIndex);
	return SlidingVector;
}



#ifdef _DEBUG
HRESULT CNavigation::Render()
{

	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	if (FAILED(m_pShader->SetUp_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	_float4x4 ViewMatrix = pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
	_float4x4 ProjMatrix = pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
	_float4	 vColor = { 0.f, 1.f, 0.f, 1.f };

	if (FAILED(m_pShader->SetUp_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->SetUp_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->SetUp_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	Safe_Release(pPipeLine);

	m_pShader->Begin(0);

	for (auto& pCell : m_Cells)
		pCell->Render_Debug();

	return S_OK;
}
#endif // _DEBUG


CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext , const _tchar* pNavigationDataFile)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationDataFile)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif // _DEBUG
}
