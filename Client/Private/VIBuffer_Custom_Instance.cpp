#include "pch.h"
#include "..\Public\VIBuffer_Custom_Instance.h"

#include "GameInstance.h"

CVIBuffer_CustomParticle::CVIBuffer_CustomParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Point_Instance(pDevice, pContext)
{
}

CVIBuffer_CustomParticle::CVIBuffer_CustomParticle(const CVIBuffer_CustomParticle& rhs)
	: CVIBuffer_Point_Instance(rhs)
{
}

HRESULT CVIBuffer_CustomParticle::Reset_Particle(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	memcpy(&m_CustomPartDesc, pArg, sizeof m_CustomPartDesc);

	m_iNumSprite = m_CustomPartDesc.vSpriteCount.x * m_CustomPartDesc.vSpriteCount.y;
	m_dCycle = _double(m_CustomPartDesc.fLifeTime / _float(m_iNumSprite));

	if (m_CustomPartDesc.InstanceDesc.vMaxRange.x < m_CustomPartDesc.InstanceDesc.vMinRange.x)
	{
		_float fMin = m_CustomPartDesc.InstanceDesc.vMaxRange.x;
		m_CustomPartDesc.InstanceDesc.vMaxRange.x = m_CustomPartDesc.InstanceDesc.vMinRange.x;
		m_CustomPartDesc.InstanceDesc.vMinRange.x = fMin;
	}
	if (m_CustomPartDesc.InstanceDesc.vMaxRange.y < m_InstanceDesc.vMinRange.y)
	{
		_float fMin = m_CustomPartDesc.InstanceDesc.vMaxRange.y;
		m_CustomPartDesc.InstanceDesc.vMaxRange.y = m_CustomPartDesc.InstanceDesc.vMinRange.y;
		m_CustomPartDesc.InstanceDesc.vMinRange.y = fMin;
	}
	if (m_CustomPartDesc.InstanceDesc.vMaxRange.z < m_InstanceDesc.vMinRange.z)
	{
		_float fMin = m_CustomPartDesc.InstanceDesc.vMaxRange.z;
		m_CustomPartDesc.InstanceDesc.vMaxRange.z = m_CustomPartDesc.InstanceDesc.vMinRange.z;
		m_CustomPartDesc.InstanceDesc.vMinRange.z = fMin;
	}


	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		_float fRand = Random::Generate_Float(0.01f, 1.f);

		m_pSpeed[i] = (fRand * (m_InstanceDesc.fMaxSpeed - m_InstanceDesc.fMinSpeed)) + m_InstanceDesc.fMinSpeed;
	}
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		_float fRand = Random::Generate_Float(0.01f, 1.f);

		m_pScale[i] = (fRand * (m_InstanceDesc.fMaxScale - m_InstanceDesc.fMinScale)) + m_InstanceDesc.fMinScale;
	}
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		_float fRand = Random::Generate_Float(0.f, 1.f);

		m_pAngle[i] = (fRand * 360.f);
	}

	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXINSTANCE*)SubResource.pData)[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		((VTXINSTANCE*)SubResource.pData)[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		((VTXINSTANCE*)SubResource.pData)[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);

		((VTXINSTANCE*)SubResource.pData)[i].vTranslation = _float4(Random::Generate_Float(m_CustomPartDesc.InstanceDesc.vMinRange.x, m_CustomPartDesc.InstanceDesc.vMaxRange.x),
																	Random::Generate_Float(m_CustomPartDesc.InstanceDesc.vMinRange.y, m_CustomPartDesc.InstanceDesc.vMaxRange.y),
																	Random::Generate_Float(m_CustomPartDesc.InstanceDesc.vMinRange.z, m_CustomPartDesc.InstanceDesc.vMaxRange.z), 1.f);
		((VTXINSTANCE*)SubResource.pData)[i].vColor = _float4(1.0f, 1.f, (_float)(rand() % 2), 0.f);
		((VTXINSTANCE*)SubResource.pData)[i].vPSize = _float2(m_pScale[i], m_pScale[i]);
		((VTXINSTANCE*)SubResource.pData)[i].fAngle = m_pAngle[i];
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	return S_OK;
}

HRESULT CVIBuffer_CustomParticle::Initialize_Prototype(_uint iNumInstance)
{
	if (FAILED(__super::Initialize_Prototype(iNumInstance)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_CustomParticle::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	memcpy(&m_CustomPartDesc, pArg, sizeof m_CustomPartDesc);

	if (FAILED(__super::Initialize(&m_CustomPartDesc.InstanceDesc)))
		return E_FAIL;

	m_iNumSprite = m_CustomPartDesc.vSpriteCount.x * m_CustomPartDesc.vSpriteCount.y;
	m_dCycle = _double(m_CustomPartDesc.fLifeTime / _float(m_iNumSprite));
	
	if (m_CustomPartDesc.bSpriteRandomStart)
		m_iStartSprite = Random::Generate_Int(0, m_iNumSprite);
	else
		m_iStartSprite = 0;

	return S_OK;
}

void CVIBuffer_CustomParticle::Tick(_double dTimeDelta)
{
	switch (m_CustomPartDesc.eParticleType)
	{
	case TYPE_SIZE:
		Tick_Size(dTimeDelta);
		break;
	case TYPE_DIR:
		Tick_Size(dTimeDelta);
		break;
	case TYPE_SIZE_DIR:
		Tick_Size_Dir(dTimeDelta);
		break;
	}

	m_dTimeAcc += dTimeDelta;
}

void CVIBuffer_CustomParticle::Tick_Size(_double dTimeDelta)
{
	_bool bSprite = false;

	if (3 <= m_CustomPartDesc.vSpriteCount.x + m_CustomPartDesc.vSpriteCount.y)
		bSprite = true;

	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		((VTXINSTANCE*)SubResource.pData)[i].vPSize.x +=  m_pSpeed[i] * (_float)dTimeDelta * m_CustomPartDesc.fTickPerSize;
		((VTXINSTANCE*)SubResource.pData)[i].vPSize.y +=  m_pSpeed[i] * (_float)dTimeDelta * m_CustomPartDesc.fTickPerSize;

		if (bSprite)
		{
			_int iCount = 0;

			if (0.0 != m_CustomPartDesc.dSpriteSpeed)
				iCount = _int(_double(m_pSpeed[i]) * (m_dTimeAcc / (m_dCycle / m_CustomPartDesc.dSpriteSpeed)));

			if (m_CustomPartDesc.bSpriteRandomStart)
			{
				iCount += m_iStartSprite;
			}

			((VTXINSTANCE*)SubResource.pData)[i].vUV.x = _float(iCount % m_CustomPartDesc.vSpriteCount.x) * (1.f / _float(m_CustomPartDesc.vSpriteCount.x));
			((VTXINSTANCE*)SubResource.pData)[i].vUV.y = _float(iCount / m_CustomPartDesc.vSpriteCount.x) * (1.f / _float(m_CustomPartDesc.vSpriteCount.y));
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_CustomParticle::Tick_Dir(_double dTimeDelta)
{
	_bool bSprite = false;

	if (3 <= m_CustomPartDesc.vSpriteCount.x + m_CustomPartDesc.vSpriteCount.y)
		bSprite = true;

	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		if (0 != m_CustomPartDesc.vDirOption.x || 0 != m_CustomPartDesc.vDirOption.y || 0 != m_CustomPartDesc.vDirOption.z)
		{
			_vector vDir = Convert::ToVector(((VTXINSTANCE*)SubResource.pData)[i].vTranslation);

			vDir = XMVectorSetW(vDir, 0.f);

			vDir = XMVector3Normalize(vDir);

			_float3 vDir_float = Convert::ToFloat3(vDir);

			if(1 == m_CustomPartDesc.vDirOption.x)
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation.x += vDir_float.x * m_pSpeed[i] * (_float)dTimeDelta * m_CustomPartDesc.vTickPerDir.x;
			else
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation.x += m_pSpeed[i] * (_float)dTimeDelta * m_CustomPartDesc.vTickPerDir.x;

			if (1 == m_CustomPartDesc.vDirOption.y)
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation.y += vDir_float.y * m_pSpeed[i] * (_float)dTimeDelta * m_CustomPartDesc.vTickPerDir.y;
			else
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation.y += m_pSpeed[i] * (_float)dTimeDelta * m_CustomPartDesc.vTickPerDir.y;

			if (1 == m_CustomPartDesc.vDirOption.z)
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation.z += vDir_float.z * m_pSpeed[i] * (_float)dTimeDelta * m_CustomPartDesc.vTickPerDir.z;
			else
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation.z += m_pSpeed[i] * (_float)dTimeDelta * m_CustomPartDesc.vTickPerDir.z;
		}
		else
		{
			((VTXINSTANCE*)SubResource.pData)[i].vTranslation.x += m_pSpeed[i] * (_float)dTimeDelta * m_CustomPartDesc.vTickPerDir.x;
			((VTXINSTANCE*)SubResource.pData)[i].vTranslation.y += m_pSpeed[i] * (_float)dTimeDelta * m_CustomPartDesc.vTickPerDir.y;
			((VTXINSTANCE*)SubResource.pData)[i].vTranslation.z += m_pSpeed[i] * (_float)dTimeDelta * m_CustomPartDesc.vTickPerDir.z;
		}

		if (bSprite)
		{
			_int iCount = 0;

			if (0.0 != m_CustomPartDesc.dSpriteSpeed)
				iCount = _int(_double(m_pSpeed[i]) * (m_dTimeAcc / (m_dCycle / m_CustomPartDesc.dSpriteSpeed)));

			if (m_CustomPartDesc.bSpriteRandomStart)
			{
				iCount += m_iStartSprite;
			}

			((VTXINSTANCE*)SubResource.pData)[i].vUV.x = _float(iCount % m_CustomPartDesc.vSpriteCount.x) * (1.f / _float(m_CustomPartDesc.vSpriteCount.x));
			((VTXINSTANCE*)SubResource.pData)[i].vUV.y = _float(iCount / m_CustomPartDesc.vSpriteCount.x) * (1.f / _float(m_CustomPartDesc.vSpriteCount.y));
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_CustomParticle::Tick_Size_Dir(_double dTimeDelta)
{
	_bool bSprite = false;

	if (3 <= m_CustomPartDesc.vSpriteCount.x + m_CustomPartDesc.vSpriteCount.y)
		bSprite = true;

	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		((VTXINSTANCE*)SubResource.pData)[i].vPSize.x += m_pSpeed[i] * (_float)dTimeDelta * m_CustomPartDesc.fTickPerSize;
		((VTXINSTANCE*)SubResource.pData)[i].vPSize.y += m_pSpeed[i] * (_float)dTimeDelta * m_CustomPartDesc.fTickPerSize;

		if (0 != m_CustomPartDesc.vDirOption.x || 0 != m_CustomPartDesc.vDirOption.y || 0 != m_CustomPartDesc.vDirOption.z)
		{
			_vector vDir = Convert::ToVector(((VTXINSTANCE*)SubResource.pData)[i].vTranslation);

			vDir = XMVectorSetW(vDir, 0.f);

			vDir = XMVector3Normalize(vDir);

			_float3 vDir_float = Convert::ToFloat3(vDir);

			if (1 == m_CustomPartDesc.vDirOption.x)
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation.x += vDir_float.x * m_pSpeed[i] * (_float)dTimeDelta * _float(m_CustomPartDesc.vTickPerDir.x);
			else
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation.x += m_pSpeed[i] * (_float)dTimeDelta * m_CustomPartDesc.vTickPerDir.x;

			if (1 == m_CustomPartDesc.vDirOption.y)
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation.y += vDir_float.y * m_pSpeed[i] * (_float)dTimeDelta * _float(m_CustomPartDesc.vTickPerDir.y);
			else
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation.y += m_pSpeed[i] * (_float)dTimeDelta * m_CustomPartDesc.vTickPerDir.y;

			if (1 == m_CustomPartDesc.vDirOption.z)
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation.z += vDir_float.z * m_pSpeed[i] * (_float)dTimeDelta * _float(m_CustomPartDesc.vTickPerDir.z);
			else
				((VTXINSTANCE*)SubResource.pData)[i].vTranslation.z += m_pSpeed[i] * (_float)dTimeDelta * m_CustomPartDesc.vTickPerDir.z;
		}
		else
		{
			((VTXINSTANCE*)SubResource.pData)[i].vTranslation.x += m_pSpeed[i] * (_float)dTimeDelta * _float(m_CustomPartDesc.vTickPerDir.x);
			((VTXINSTANCE*)SubResource.pData)[i].vTranslation.y += m_pSpeed[i] * (_float)dTimeDelta * _float(m_CustomPartDesc.vTickPerDir.y);
			((VTXINSTANCE*)SubResource.pData)[i].vTranslation.z += m_pSpeed[i] * (_float)dTimeDelta * _float(m_CustomPartDesc.vTickPerDir.z);
		}

		if (bSprite)
		{
			_int iCount = 0;

			if (0.0 != m_CustomPartDesc.dSpriteSpeed)
				iCount = _int(_double(m_pSpeed[i]) * (m_dTimeAcc / (m_dCycle / m_CustomPartDesc.dSpriteSpeed)));

			if (m_CustomPartDesc.bSpriteRandomStart)
			{
				iCount += m_iStartSprite;
			}

			((VTXINSTANCE*)SubResource.pData)[i].vUV.x = _float(iCount % m_CustomPartDesc.vSpriteCount.x) * (1.f / _float(m_CustomPartDesc.vSpriteCount.x));
			((VTXINSTANCE*)SubResource.pData)[i].vUV.y = _float(iCount / m_CustomPartDesc.vSpriteCount.x) * (1.f / _float(m_CustomPartDesc.vSpriteCount.y));
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

CVIBuffer_CustomParticle* CVIBuffer_CustomParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance)
{
	CVIBuffer_CustomParticle* pInstance = new CVIBuffer_CustomParticle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumInstance)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_CustomParticle");
		Safe_Release(pInstance);
	}

	return pInstance;
}
CComponent* CVIBuffer_CustomParticle::Clone(void* pArg)
{
	CVIBuffer_CustomParticle* pInstance = new CVIBuffer_CustomParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Ice_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_CustomParticle::Free()
{
	__super::Free();
}
