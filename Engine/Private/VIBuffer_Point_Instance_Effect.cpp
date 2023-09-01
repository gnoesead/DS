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

	VTXPOS* pVertices = new VTXPOS;
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

	_ushort* pIndices = new _ushort[m_iNumIndices];
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

	m_eEffectDesc = m_InstanceDesc.pParent->Get_EffectDesc();

#pragma region INSTANCEBUFFER	
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iMaxNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;

	VTXINSTANCEEFFECT* pInstanceVertices = new VTXINSTANCEEFFECT[m_iMaxNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXINSTANCEEFFECT) * m_iMaxNumInstance);

	for (_uint i = 0; i < m_iMaxNumInstance; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);

		switch (m_eEffectDesc.eShapeType)
		{
		case 2:
		{
			_float fAngle = Random::Generate_Int(0, 360);
			pInstanceVertices[i].vTranslation = _float4(cosf(XMConvertToRadians(fAngle)), 0.f, sinf(XMConvertToRadians(fAngle)), 1.f);
		}
		break;
		case 4:
		{
			pInstanceVertices[i].vTranslation = _float4(Random::Generate_Float(-10.f, 10.f) * 0.1f + 0.001f,
				Random::Generate_Float(-10.f, 10.f) * 0.1f + 0.001f,
				Random::Generate_Float(-10.f, 10.f) * 0.1f + 0.001f, 1.f);
		}
		break;
		default:
			pInstanceVertices[i].vTranslation = _float4(Random::Generate_Float(-10.f, 10.f),
				Random::Generate_Float(-10.f, 10.f),
				Random::Generate_Float(-10.f, 10.f), 1.f);
			break;
		}

		pInstanceVertices[i].vColor = _float4(1.0f, 1.f, rand() % 2, 0.f);
		pInstanceVertices[i].vPSize = _float2(m_eEffectDesc.vStartSizeMin.x, m_eEffectDesc.vStartSizeMin.y);
		pInstanceVertices[i].vAdditional = _float4(m_eEffectDesc.fStartLifeTimeMin, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vAdditional2 = _float4(1.f, 1.f, 0.f, 0.f);
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
	m_dTimeAccDuration += TimeDelta;
	m_dTimeAccRateOverTime += TimeDelta;

	if (m_eEffectDesc.isLooping)
	{
		//if (m_dTimeAcc > pDesc->pParent->m_eEffectDesc.fDuration / (m_iNumInstance + 1))
		//{
		//	if(m_iNumInstanceUsing < m_iNumInstance)
		//		++m_iNumInstanceUsing;
		//	m_dTimeAcc = 0.0;
		//}

		if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eRateOverTimeOption)
		{
			if (m_dTimeAcc > 1 / m_eEffectDesc.fRateOverTimeMin)
			{
				if (m_iNumInstanceUsing < m_iNumInstance)
					m_iNumInstanceUsing += m_eEffectDesc.iNumParticlesPerFrame;
				m_dTimeAcc = 0.0;
			}
		}
		else if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eRateOverTimeOption)
		{
			_float fRateOverTime = Random::Generate_Float(m_eEffectDesc.fRateOverTimeMin, m_eEffectDesc.fRateOverTimeMax);

			if (m_dTimeAcc > 1 / fRateOverTime)
			{
				if (m_iNumInstanceUsing < m_iNumInstance)
					m_iNumInstanceUsing += m_eEffectDesc.iNumParticlesPerFrame;
				m_dTimeAcc = 0.0;
			}
		}
		else if (CMasterEffect::OP_CURVE == m_eEffectDesc.eRateOverTimeOption)
		{
			size_t iSize = pDesc->pParent->Get_RateOverTimes().size();

			if (0 < iSize)
			{
				if (m_iCurRateOverTimeIndex < iSize)
				{
					while (true)
					{
						if (m_iCurRateOverTimeIndex >= iSize - 1)
							m_iCurRateOverTimeIndex = iSize - 2;

						if (m_dTimeAccRateOverTime <= m_RateOverTimes[m_iCurRateOverTimeIndex + 1].fLifetime)
							break;

						++m_iCurRateOverTimeIndex;

						if (m_iCurRateOverTimeIndex >= iSize - 1)
							m_iCurRateOverTimeIndex = iSize - 2;
					}

					_float y = fabs(m_RateOverTimes[m_iCurRateOverTimeIndex + 1].fValue - m_RateOverTimes[m_iCurRateOverTimeIndex].fValue);
					_float fWeight = fabs(m_dTimeAccRateOverTime - m_RateOverTimes[m_iCurRateOverTimeIndex].fLifetime)
						/ fabs(m_RateOverTimes[m_iCurRateOverTimeIndex + 1].fLifetime - m_RateOverTimes[m_iCurRateOverTimeIndex].fLifetime);

					if (m_RateOverTimes[m_iCurRateOverTimeIndex + 1].fValue < m_RateOverTimes[m_iCurRateOverTimeIndex].fValue)
						y *= -1;

					_float	fRateOverTime = fWeight * y + m_RateOverTimes[m_iCurRateOverTimeIndex].fValue;

					if (m_dTimeAcc > 1 / fRateOverTime)
					{
						if (m_iNumInstanceUsing < m_iNumInstance)
							m_iNumInstanceUsing += m_eEffectDesc.iNumParticlesPerFrame;
						m_dTimeAcc = 0.0;
					}
				}
			}
		}
	}
	else
	{
		if (m_dTimeAccDuration < m_eEffectDesc.fDuration)
		{
			if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eRateOverTimeOption)
			{
				if (m_dTimeAcc > 1 / m_eEffectDesc.fRateOverTimeMin)
				{
					if (m_iNumInstanceUsing < m_iNumInstance)
						m_iNumInstanceUsing += m_eEffectDesc.iNumParticlesPerFrame;
					m_dTimeAcc = 0.0;
				}
			}
			else if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eRateOverTimeOption)
			{
				_float fRateOverTime = Random::Generate_Float(m_eEffectDesc.fRateOverTimeMin, m_eEffectDesc.fRateOverTimeMax);

				if (m_dTimeAcc > 1 / fRateOverTime)
				{
					if (m_iNumInstanceUsing < m_iNumInstance)
						m_iNumInstanceUsing += m_eEffectDesc.iNumParticlesPerFrame;
					m_dTimeAcc = 0.0;
				}
			}
			else if (CMasterEffect::OP_CURVE == m_eEffectDesc.eRateOverTimeOption)
			{
				size_t iSize = m_RateOverTimes.size();

				if (0 < iSize)
				{
					if (m_iCurRateOverTimeIndex < iSize)
					{
						while (true)
						{
							if (m_iCurRateOverTimeIndex >= iSize - 1)
								m_iCurRateOverTimeIndex = iSize - 2;

							if (m_dTimeAccRateOverTime <= m_RateOverTimes[m_iCurRateOverTimeIndex + 1].fLifetime)
								break;

							++m_iCurRateOverTimeIndex;

							if (m_iCurRateOverTimeIndex >= iSize - 1)
								m_iCurRateOverTimeIndex = iSize - 2;
						}

						_float y = fabs(m_RateOverTimes[m_iCurRateOverTimeIndex + 1].fValue - m_RateOverTimes[m_iCurRateOverTimeIndex].fValue);
						_float fWeight = fabs(m_dTimeAccRateOverTime - m_RateOverTimes[m_iCurRateOverTimeIndex].fLifetime)
							/ fabs(m_RateOverTimes[m_iCurRateOverTimeIndex + 1].fLifetime - m_RateOverTimes[m_iCurRateOverTimeIndex].fLifetime);

						if (m_RateOverTimes[m_iCurRateOverTimeIndex + 1].fValue < m_RateOverTimes[m_iCurRateOverTimeIndex].fValue)
							y *= -1;

						_float	fRateOverTime = fWeight * y + m_RateOverTimes[m_iCurRateOverTimeIndex].fValue;

						if (m_dTimeAcc > 1 / fRateOverTime)
						{
							if (m_iNumInstanceUsing < m_iNumInstance)
								m_iNumInstanceUsing += m_eEffectDesc.iNumParticlesPerFrame;
							m_dTimeAcc = 0.0;
						}
					}
				}
			}
		}
	}

	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = m_iStartIndex; i <= m_iNumInstanceUsing; i++)
	{
		// Lifetime
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x -= TimeDelta;
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.z += TimeDelta;			// TimeAcc

		// Lifetime < 0
		if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x < 0)
		{
			if (m_eEffectDesc.isLooping)
			{
				if (!m_eEffectDesc.eShapeType)
					m_iStartIndex = i + 1;
				else
				{
					switch (m_eEffectDesc.eShapeType)
					{
					case 2:		// CIRCLE
					{
						_float fAngle = Random::Generate_Float(0, m_eEffectDesc.fArc);
						((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation = _float4(cosf(XMConvertToRadians(fAngle)), 0.f, sinf(XMConvertToRadians(fAngle)), 1.f);

						_vector vPos = XMLoadFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation);
						_vector vLook = XMVector3Normalize(vPos - XMVectorSet(0.f, 0.f, 0.f, 1.f));
						_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
						_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

						XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vRight, vRight);
						XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vUp, vUp);
						XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLook, vLook);
					}
					break;
					case 4:		// SPHERE
					{
						((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation = _float4(Random::Generate_Float(-10, 10) * 0.1f + 0.001f,
							Random::Generate_Float(-10, 10) * 0.1f + 0.001f,
							Random::Generate_Float(-10, 10) * 0.1f + 0.001f, 1.f);

						_vector vPos = XMLoadFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation);
						_vector vLook = XMVector3Normalize(vPos - XMVectorSet(0.f, 0.f, 0.f, 1.f));
						_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
						_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

						XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vRight, vRight);
						XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vUp, vUp);
						XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLook, vLook);
					}
					break;
					case CMasterEffect::CONE:
					{
						_float fRadius = Random::Generate_Float(0.001, m_eEffectDesc.fShapeRadius);
						_float fAngle = Random::Generate_Float(0, m_eEffectDesc.fArc);
						((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation = _float4(cosf(XMConvertToRadians(fAngle)) * fRadius, 0.f, sinf(XMConvertToRadians(fAngle)) * fRadius, 1.f);

						_float fRadiusUpper = fRadius + tanf(XMConvertToRadians(m_eEffectDesc.fShapeAngle));
						_float4 vPosUpper = _float4(cosf(XMConvertToRadians(fAngle)) * fRadiusUpper, 1.f, sinf(XMConvertToRadians(fAngle)) * fRadiusUpper, 1.f);

						_vector vPos = XMLoadFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation);
						_vector vLook = XMVector3Normalize(Convert::ToVector(vPosUpper) - vPos);
						_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
						_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

						XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vRight, vRight);
						XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vUp, vUp);
						XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLook, vLook);
					}
					break;
					default:
						((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation.y = rand() % (_int(10) + 1) - _int(10 * 0.5f);
						break;
					}

					if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eStartLifeTimeOption)
						((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x = m_eEffectDesc.fStartLifeTimeMin;
					else if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eStartLifeTimeOption)
					{
						((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x = Random::Generate_Float(m_eEffectDesc.fStartLifeTimeMin, m_eEffectDesc.fStartLifeTimeMax);
					}
					((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.y = 1.f;
					((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.z = 0.f;
					// EndSize
					((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.w = Random::Generate_Float(m_eEffectDesc.fYExtendEndSize.x, m_eEffectDesc.fYExtendEndSize.y);		// EndSize

					if (!m_eEffectDesc.is3DStartSize)
					{
						if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eStartSizeOption)
						{
							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize = _float2(m_eEffectDesc.vStartSizeMin.x, m_eEffectDesc.vStartSizeMin.x);
							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.x = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.x;
							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.y = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.y;
						}
						else if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eStartSizeOption)
						{
							_float fSize = Random::Generate_Float(m_eEffectDesc.vStartSizeMin.x, m_eEffectDesc.vStartSizeMax.x);

							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize = _float2(fSize, fSize);
							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.x = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.x;
							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.y = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.y;
						}
					}
					else
					{
						if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eStartSizeOption)
						{
							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize = _float2(m_eEffectDesc.vStartSizeMin.x, m_eEffectDesc.vStartSizeMin.y);

							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.x = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.x;
							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.y = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.y;
						}
						else if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eStartSizeOption)
						{
							_float fSizeX = Random::Generate_Float(m_eEffectDesc.vStartSizeMin.x, m_eEffectDesc.vStartSizeMax.x);
							_float fSizeY = Random::Generate_Float(m_eEffectDesc.vStartSizeMin.y, m_eEffectDesc.vStartSizeMax.y);
							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize = _float2(fSizeX, fSizeY);

							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.x = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.x;
							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.y = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.y;
						}
					}

					// CurSpeedIndex
					((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w = 0;

					// CurSizeIndex
					((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w = 0;

					// CurGravityIndex
					((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w = 0;

					// SizeYAcc
					((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.x = 1;
				}
			}
			else
			{
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.y = 0.f;
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
			}
		}
		else
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.y = 1.f;

		if (m_iNumInstance >= m_iMaxNumInstance)
			m_iNumInstance = m_iMaxNumInstance - 1;

		// Velocity Over Lifetime
		size_t iSize = m_SpeedOverLifeTimes.size();
		if (0 < iSize)
		{
			if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w < iSize && ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x > 0)
			{
				while (true)
				{
					if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w >= iSize - 1)
						((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w = iSize - 2;

					if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x >= m_SpeedOverLifeTimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w + 1].fLifetime)
						break;

					++((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w;

					if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w >= iSize - 1)
						((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w = iSize - 2;
				}

				_float y = fabs(m_SpeedOverLifeTimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w + 1].fValue - m_SpeedOverLifeTimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w].fValue);
				_float fWeight = fabs(((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x - m_SpeedOverLifeTimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w].fLifetime)
					/ fabs(m_SpeedOverLifeTimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w + 1].fLifetime - m_SpeedOverLifeTimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w].fLifetime);

				if (m_SpeedOverLifeTimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w + 1].fValue < m_SpeedOverLifeTimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w].fValue)
					y *= -1;

				m_fSpeedModifier = fWeight * y + m_SpeedOverLifeTimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w].fValue;
			}
		}


		// Size Over Lifetime
		if (CMasterEffect::OP_CURVE == m_eEffectDesc.eSizeOverLifetimeOption)
		{
			size_t iSize = m_SizeOverLifeTimesX.size();

			if (0 < iSize)
			{
				if (!m_eEffectDesc.isSeparateAxesSzOverLifeTime)
				{
					if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w < iSize && ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x > 0)
					{
						while (true)
						{
							if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w >= iSize - 1)
								((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w = iSize - 2;

							if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x >= m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w + 1].fLifetime)
								break;

							++((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w;

							if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w >= iSize - 1)
								((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w = iSize - 2;
						}

						_float y = fabs(m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w + 1].fValue - m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w].fValue);
						_float fWeight = fabs(((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x - m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w].fLifetime)
							/ fabs(m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w + 1].fLifetime - m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w].fLifetime);

						if (m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w + 1].fValue < m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w].fValue)
							y *= -1;

						_float	fCurSize = fWeight * y + m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w].fValue;

						if (!m_eEffectDesc.is3DStartSize)
							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize = _float2(fCurSize, fCurSize);
						else
						{
							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.x = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.x * fCurSize;
							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.y = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.y * fCurSize;
						}
					}
				}
				else
				{
					if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x > 0)
					{
						_float2 vCurSize = { 1.f, 1.f };

						for (int i = 0; i < 2; ++i)
						{
							if (i == 0)
							{
								size_t iSize = m_SizeOverLifeTimesX.size();

								if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w < iSize)
								{
									while (true)
									{
										if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w >= iSize - 1)
											((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w = iSize - 2;

										if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x >= m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w + 1].fLifetime)
											break;

										++((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w;

										if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w >= iSize - 1)
											((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w = iSize - 2;
									}

									_float y = fabs(m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w + 1].fValue - m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w].fValue);
									_float fWeight = fabs(((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x - m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w].fLifetime)
										/ fabs(m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w + 1].fLifetime - m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w].fLifetime);

									if (m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w + 1].fValue < m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w].fValue)
										y *= -1;

									vCurSize.x = fWeight * y + m_SizeOverLifeTimesX[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w].fValue;
								}
							}
							else
							{
								size_t iSize = m_SizeOverLifeTimesY.size();

								if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.y < iSize)
								{
									while (true)
									{
										if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.y >= iSize - 1)
											((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.y = iSize - 2;

										if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x >= m_SizeOverLifeTimesY[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.y + 1].fLifetime)
											break;

										++((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.y;

										if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.y >= iSize - 1)
											((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.y = iSize - 2;
									}

									_float y = fabs(m_SizeOverLifeTimesY[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.y + 1].fValue - m_SizeOverLifeTimesY[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.y].fValue);
									_float fWeight = fabs(((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x - m_SizeOverLifeTimesY[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.y].fLifetime)
										/ fabs(m_SizeOverLifeTimesY[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.y + 1].fLifetime - m_SizeOverLifeTimesY[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.y].fLifetime);

									if (m_SizeOverLifeTimesY[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.y + 1].fValue < m_SizeOverLifeTimesY[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.y].fValue)
										y *= -1;

									vCurSize.y = fWeight * y + m_SizeOverLifeTimesY[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.y].fValue;
								}
							}
						}

						if (!m_eEffectDesc.is3DStartSize)
							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize = _float2(vCurSize.x, vCurSize.y);
						else
						{
							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.x = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.x * vCurSize.x;
							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.y = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.y * vCurSize.y;
						}
					}
				}
			}
		}

		// GravityModifier
		if (CMasterEffect::OP_CURVE == m_eEffectDesc.eGravityModifierOption)
		{
			size_t iSize = m_GravityModiferOverLifetimes.size();

			if (0 < iSize)
			{
				if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w < iSize && ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x > 0)
				{
					while (true)
					{
						if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w >= iSize - 1)
							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w = iSize - 2;

						if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x >= m_GravityModiferOverLifetimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w + 1].fLifetime)
							break;

						++((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w;

						if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w >= iSize - 1)
							((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w = iSize - 2;
					}

					_float y = fabs(m_GravityModiferOverLifetimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w + 1].fValue - m_GravityModiferOverLifetimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w].fValue);
					_float fWeight = fabs(((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x - m_GravityModiferOverLifetimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w].fLifetime)
						/ fabs(m_GravityModiferOverLifetimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w + 1].fLifetime - m_GravityModiferOverLifetimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w].fLifetime);

					if (m_GravityModiferOverLifetimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w + 1].fValue < m_GravityModiferOverLifetimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w].fValue)
						y *= -1;

					((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.z = fWeight * y + m_GravityModiferOverLifetimes[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w].fValue;
				}
			}
		}

		// TextureSheetAnimation
		if (m_eEffectDesc.isTextureSheetAnimation)
		{
			if (CMasterEffect::OPT_SPEED == m_eEffectDesc.eTimeModeOption)
			{
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.x += TimeDelta * ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.z;

				if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.x >= 1.f)
				{
					++((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.x;
					((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.x = 0.0;
					if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.x >= m_eEffectDesc.vTiles.x * m_eEffectDesc.vTiles.y)
						((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.x = 0;
				}
			}
			else if (CMasterEffect::OPT_LIFETIME == m_eEffectDesc.eTimeModeOption)
			{
				size_t iSize = m_FrameOverTime.size();

				if (0 < iSize)
				{
					if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.w < iSize)
					{
						while (true)
						{
							if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.x >= iSize - 1)
								((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.w = iSize - 2;

							if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x >= m_FrameOverTime[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.w + 1].fLifetime)
								break;

							++((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.w;

							if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.w >= iSize - 1)
								((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.w = iSize - 2;
						}

						_float y = fabs(m_FrameOverTime[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.w + 1].fValue - m_FrameOverTime[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.w].fValue);
						_float fWeight = fabs(((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x - m_FrameOverTime[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.w].fLifetime)
							/ fabs(m_FrameOverTime[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.w + 1].fLifetime - m_FrameOverTime[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.w].fLifetime);

						if (m_FrameOverTime[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.w + 1].fValue < m_FrameOverTime[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.w].fValue)
							y *= -1.f;

						((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.x = fWeight * y + m_FrameOverTime[((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.w].fValue;
					}
				}
			}
		}

		switch (m_eEffectDesc.eShapeType)
		{
		case 2: case 4:	// CIRCLE, SPHERE
		{
			_vector vPos = XMLoadFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation);
			_vector vLook = XMVector3Normalize(vPos - XMVectorSet(0.f, 0.f, 0.f, 1.f));
			_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
			_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vRight, vRight);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vUp, vUp);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLook, vLook);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation, vPos + vLook * ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.y * m_fSpeedModifier * TimeDelta
				+ XMVectorSet(0.f, -1.f, 0.f, 0.f) * ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.z * ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.z);

			int a = 10;
		}
		break;
		case 3:		// CONE
		{
			_vector vPos = XMLoadFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation);
			_vector vLook = XMLoadFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLook);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation, vPos + vLook * ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.y * m_fSpeedModifier * TimeDelta
				+ XMVectorSet(0.f, -1.f, 0.f, 0.f) * ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.z * ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.z);
		}
		break;
		default:
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation.y += ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.y * m_fSpeedModifier * TimeDelta
				- ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.z * ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.z;
			break;
		}

		// Spark
		if (m_eEffectDesc.isSpark)
		{
			// SizeY += TimeAcc
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.x += ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.z * m_eEffectDesc.fYExtendSpeed;

			if (((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.x > ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.w)
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.x = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.w;
		}

		// Velocity Linear
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation.x += ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.x;
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation.y += ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.y;
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation.z += ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.z;
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Point_Instance_Effect::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance,
	};

	_uint					iStrides[] = {
		m_iStride,
		m_iInstanceStride,
	};

	_uint					iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexedInstanced(m_iNumIndicesPerInstance, m_iNumInstanceUsing, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Point_Instance_Effect::InitialSetting()
{
	m_eEffectDesc = m_InstanceDesc.pParent->Get_EffectDesc();
	m_RateOverTimes = m_InstanceDesc.pParent->Get_RateOverTimes();
	m_SpeedOverLifeTimes = m_InstanceDesc.pParent->Get_SpeedOverTimes();
	m_GravityModiferOverLifetimes = m_InstanceDesc.pParent->Get_GravityModiferOverTimes();
	m_FrameOverTime = m_InstanceDesc.pParent->Get_FrameOverTimes();
	m_SizeOverLifeTimesX = m_InstanceDesc.pParent->Get_SizeOverTimesX();

	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = m_iStartIndex; i < m_eEffectDesc.iMaxParticles; i++)
	{

		if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eStartLifeTimeOption)
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x = m_eEffectDesc.fStartLifeTimeMin;
		else if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eStartLifeTimeOption)
		{
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x = Random::Generate_Float(m_eEffectDesc.fStartLifeTimeMin, m_eEffectDesc.fStartLifeTimeMax);
		}
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.y = 1.f;
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.z = 0.f;

		if (!m_eEffectDesc.is3DStartSize)
		{
			if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eStartSizeOption)
			{
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize = _float2(m_eEffectDesc.vStartSizeMin.x, m_eEffectDesc.vStartSizeMin.x);
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.x = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.x;
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.y = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.y;
			}
			else if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eStartSizeOption)
			{
				_float fSize = Random::Generate_Float(m_eEffectDesc.vStartSizeMin.x, m_eEffectDesc.vStartSizeMax.x);

				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize = _float2(fSize, fSize);
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.x = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.x;
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.y = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.y;
			}
		}
		else
		{
			if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eStartSizeOption)
			{
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize = _float2(m_eEffectDesc.vStartSizeMin.x, m_eEffectDesc.vStartSizeMin.y);

				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.x = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.x;
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.y = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.y;
			}
			else if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eStartSizeOption)
			{
				_float fSizeX = Random::Generate_Float(m_eEffectDesc.vStartSizeMin.x, m_eEffectDesc.vStartSizeMax.x);
				_float fSizeY = Random::Generate_Float(m_eEffectDesc.vStartSizeMin.y, m_eEffectDesc.vStartSizeMax.y);
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize = _float2(fSizeX, fSizeY);

				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.x = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.x;
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.y = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.y;
			}
		}

		switch (m_eEffectDesc.eShapeType)
		{
		case CMasterEffect::CIRCLE:
		{
			_float fAngle = Random::Generate_Float(0, m_eEffectDesc.fArc);
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation = _float4(cosf(XMConvertToRadians(fAngle)), 0.f, sinf(XMConvertToRadians(fAngle)), 1.f);

			_vector vPos = XMLoadFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation);
			_vector vLook = XMVector3Normalize(vPos - XMVectorSet(0.f, 0.f, 0.f, 1.f));
			_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
			_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vRight, vRight);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vUp, vUp);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLook, vLook);
		}
		break;
		case CMasterEffect::SPHERE:
		{
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation = _float4(Random::Generate_Float(-10, 10) * 0.1f + 0.001f,
				Random::Generate_Float(-10, 10) * 0.1f + 0.001f,
				Random::Generate_Float(-10, 10) * 0.1f + 0.001f, 1.f);

			_vector vPos = XMLoadFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation);
			_vector vLook = XMVector3Normalize(vPos - XMVectorSet(0.f, 0.f, 0.f, 1.f));
			_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
			_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vRight, vRight);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vUp, vUp);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLook, vLook);
		}
		break;
		case CMasterEffect::CONE:
		{
			_float fRadius = Random::Generate_Float(0.001, m_eEffectDesc.fShapeRadius);
			_float fAngle = Random::Generate_Float(0, m_eEffectDesc.fArc);
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation = _float4(cosf(XMConvertToRadians(fAngle)) * fRadius, 0.f, sinf(XMConvertToRadians(fAngle)) * fRadius, 1.f);

			_float fRadiusUpper = fRadius + tanf(XMConvertToRadians(m_eEffectDesc.fShapeAngle));
			_float4 vPosUpper = _float4(cosf(XMConvertToRadians(fAngle)) * fRadiusUpper, 1.f, sinf(XMConvertToRadians(fAngle)) * fRadiusUpper, 1.f);

			_vector vPos = XMLoadFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation);
			_vector vLook = XMVector3Normalize(Convert::ToVector(vPosUpper) - vPos);
			_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
			_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vRight, vRight);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vUp, vUp);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLook, vLook);
		}
		break;
		case CMasterEffect::EDGE:
		{
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation = _float4(0.f, 0.f, Random::Generate_Float(0.f, m_eEffectDesc.fShapeRadius) + 0.001f, 1.f);

			_vector vPos = XMLoadFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation);
			_vector vLook = XMVector3Normalize(vPos - XMVectorSet(0.f, 0.f, 0.f, 1.f));
			_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
			_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vRight, vRight);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vUp, vUp);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLook, vLook);
		}
		break;
		default:
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation = _float4(Random::Generate_Float(-10.f, 10.f),
				Random::Generate_Float(-10.f, 10.f),
				Random::Generate_Float(- 10.f, 10.f), 1.f);
			break;
		}

		// EndSize
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.w = Random::Generate_Float(m_eEffectDesc.fYExtendEndSize.x, m_eEffectDesc.fYExtendEndSize.y);		// EndSize

		if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eStartSpeedOption)
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.y = Random::Generate_Float(m_eEffectDesc.fStartSpeedMin, m_eEffectDesc.fStartSpeedMax);
		else if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eStartSpeedOption)
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.y = m_eEffectDesc.fStartSpeedMin;

		if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eVelocityLinearOption)
		{
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.x = m_eEffectDesc.vVelocityLinearMin.x;
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.y = m_eEffectDesc.vVelocityLinearMin.y;
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.z = m_eEffectDesc.vVelocityLinearMin.z;
		}
		else if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eVelocityLinearOption)
		{
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.x = Random::Generate_Float(m_eEffectDesc.vVelocityLinearMin.x, m_eEffectDesc.vVelocityLinearMax.x);
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.y = Random::Generate_Float(m_eEffectDesc.vVelocityLinearMin.y, m_eEffectDesc.vVelocityLinearMax.y);
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.z = Random::Generate_Float(m_eEffectDesc.vVelocityLinearMin.z, m_eEffectDesc.vVelocityLinearMax.z);
		}

		if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eGravityModifierOption)
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.z = m_eEffectDesc.vGravityModifier.x;
		else if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eGravityModifierOption)
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.z = Random::Generate_Float(m_eEffectDesc.vGravityModifier.x, m_eEffectDesc.vGravityModifier.y);

		// TextureSheet Animation
		if (m_eEffectDesc.isTextureSheetAnimation)
		{
			if (CMasterEffect::OPT_SPEED == m_eEffectDesc.eTimeModeOption)
			{
				if (m_eEffectDesc.fFrameSpeedMin != m_eEffectDesc.fFrameSpeedMax)
					((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.z = Random::Generate_Float(m_eEffectDesc.fFrameSpeedMin, m_eEffectDesc.fFrameSpeedMax);
				else
					((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.z = m_eEffectDesc.fFrameSpeedMin;
			}
		}

		// CurSpeedIndex
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w = 0;

		// CurSizeIndex
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w = 0;
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.y = 0;

		// CurGravityIndex
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w = 0;

		// SizeYAcc
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.x = 1;
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	m_dTimeAcc = 0;
	m_dTimeAccDuration = 0;
	m_dTimeAccRateOverTime = 0;

	m_iCurRateOverTimeIndex = 0;

	if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eVelocitySpeedModifierOption)
		m_fSpeedModifier = Random::Generate_Float(m_eEffectDesc.fSpeedModifierMin, m_eEffectDesc.fSpeedModifierMax);
	else if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eVelocitySpeedModifierOption)
		m_fSpeedModifier = m_eEffectDesc.fSpeedModifierMin;
	else if (CMasterEffect::OP_CURVE == m_eEffectDesc.eVelocitySpeedModifierOption)
	{
		if (m_SpeedOverLifeTimes.size() > 0)
			m_fSpeedModifier = m_SpeedOverLifeTimes[0].fValue;
	}

	m_iNumInstance = m_eEffectDesc.iMaxParticles;

	m_iNumInstanceUsing = 0.f;
}

void CVIBuffer_Point_Instance_Effect::Reset_Data()
{
	D3D11_MAPPED_SUBRESOURCE		SubResource;

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = m_iStartIndex; i < m_eEffectDesc.iMaxParticles; i++)
	{
		if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eStartLifeTimeOption)
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x = m_eEffectDesc.fStartLifeTimeMin;
		else if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eStartLifeTimeOption)
		{
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.x = Random::Generate_Float(m_eEffectDesc.fStartLifeTimeMin, m_eEffectDesc.fStartLifeTimeMax);
		}
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.y = 0.f;
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.z = 0.f;

		if (!m_eEffectDesc.is3DStartSize)
		{
			if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eStartSizeOption)
			{
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize = _float2(m_eEffectDesc.vStartSizeMin.x, m_eEffectDesc.vStartSizeMin.x);
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.x = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.x;
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.y = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.y;
			}
			else if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eStartSizeOption)
			{
				_float fSize = Random::Generate_Float(m_eEffectDesc.vStartSizeMin.x, m_eEffectDesc.vStartSizeMax.x);

				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize = _float2(fSize, fSize);
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.x = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.x;
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.y = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.y;
			}
		}
		else
		{
			if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eStartSizeOption)
			{
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize = _float2(m_eEffectDesc.vStartSizeMin.x, m_eEffectDesc.vStartSizeMin.y);

				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.x = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.x;
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.y = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.y;
			}
			else if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eStartSizeOption)
			{
				_float fSizeX = Random::Generate_Float(m_eEffectDesc.vStartSizeMin.x, m_eEffectDesc.vStartSizeMax.x);
				_float fSizeY = Random::Generate_Float(m_eEffectDesc.vStartSizeMin.y, m_eEffectDesc.vStartSizeMax.y);
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize = _float2(fSizeX, fSizeY);

				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.x = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.x;
				((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.y = ((VTXINSTANCEEFFECT*)SubResource.pData)[i].vPSize.y;
			}
		}

		switch (m_eEffectDesc.eShapeType)
		{
		case CMasterEffect::CIRCLE:
		{
			_float fAngle = Random::Generate_Float(0, m_eEffectDesc.fArc);
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation = _float4(cosf(XMConvertToRadians(fAngle)), 0.f, sinf(XMConvertToRadians(fAngle)), 1.f);

			_vector vPos = XMLoadFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation);
			_vector vLook = XMVector3Normalize(vPos - XMVectorSet(0.f, 0.f, 0.f, 1.f));
			_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
			_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vRight, vRight);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vUp, vUp);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLook, vLook);
		}
		break;
		case CMasterEffect::SPHERE:
		{
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation = _float4(Random::Generate_Float(-10, 10) * 0.1f + 0.001f,
				Random::Generate_Float(-10, 10) * 0.1f + 0.001f,
				Random::Generate_Float(-10, 10) * 0.1f + 0.001f, 1.f);

			_vector vPos = XMLoadFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation);
			_vector vLook = XMVector3Normalize(vPos - XMVectorSet(0.f, 0.f, 0.f, 1.f));
			_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
			_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vRight, vRight);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vUp, vUp);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLook, vLook);
		}
		break;
		case CMasterEffect::CONE:
		{
			_float fRadius = Random::Generate_Float(0.001, m_eEffectDesc.fShapeRadius);
			_float fAngle = Random::Generate_Float(0, m_eEffectDesc.fArc);
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation = _float4(cosf(XMConvertToRadians(fAngle)) * fRadius, 0.f, sinf(XMConvertToRadians(fAngle)) * fRadius, 1.f);

			_float fRadiusUpper = fRadius + tanf(XMConvertToRadians(m_eEffectDesc.fShapeAngle));
			_float4 vPosUpper = _float4(cosf(XMConvertToRadians(fAngle)) * fRadiusUpper, 1.f, sinf(XMConvertToRadians(fAngle)) * fRadiusUpper, 1.f);

			_vector vPos = XMLoadFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation);
			_vector vLook = XMVector3Normalize(Convert::ToVector(vPosUpper) - vPos);
			_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
			_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vRight, vRight);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vUp, vUp);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLook, vLook);
		}
		break;
		case CMasterEffect::EDGE:
		{
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation = _float4(0.f, 0.f, Random::Generate_Float(0.f, m_eEffectDesc.fShapeRadius) + 0.001f, 1.f);
			_vector vPos = XMLoadFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation);
			_vector vLook = XMVector3Normalize(vPos - XMVectorSet(0.f, 0.f, 0.f, 1.f));
			_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
			_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vRight, vRight);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vUp, vUp);
			XMStoreFloat4(&((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLook, vLook);
		}
		break;
		default:
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vTranslation = _float4(Random::Generate_Float(-10.f, 10.f),
				Random::Generate_Float(-10.f, 10.f),
				Random::Generate_Float(-10.f, 10.f), 1.f);
			break;
		}

		// EndSize
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional.w = Random::Generate_Float(m_eEffectDesc.fYExtendEndSize.x, m_eEffectDesc.fYExtendEndSize.y);		// EndSize

		if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eStartSpeedOption)
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.y = Random::Generate_Float(m_eEffectDesc.fStartSpeedMin, m_eEffectDesc.fStartSpeedMax);
		else if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eStartSpeedOption)
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.y = m_eEffectDesc.fStartSpeedMin;

		if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eVelocityLinearOption)
		{
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.x = m_eEffectDesc.vVelocityLinearMin.x;
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.y = m_eEffectDesc.vVelocityLinearMin.y;
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.z = m_eEffectDesc.vVelocityLinearMin.z;
		}
		else if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eVelocityLinearOption)
		{
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.x = Random::Generate_Float(m_eEffectDesc.vVelocityLinearMin.x, m_eEffectDesc.vVelocityLinearMax.x);
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.y = Random::Generate_Float(m_eEffectDesc.vVelocityLinearMin.y, m_eEffectDesc.vVelocityLinearMax.y);
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.z = Random::Generate_Float(m_eEffectDesc.vVelocityLinearMin.z, m_eEffectDesc.vVelocityLinearMax.z);
		}

		if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eGravityModifierOption)
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.z = m_eEffectDesc.vGravityModifier.x;
		else if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eGravityModifierOption)
			((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.z = Random::Generate_Float(m_eEffectDesc.vGravityModifier.x, m_eEffectDesc.vGravityModifier.y);

		// TextureSheet Animation
		if (m_eEffectDesc.isTextureSheetAnimation)
		{
			if (CMasterEffect::OPT_SPEED == m_eEffectDesc.eTimeModeOption)
			{
				if (m_eEffectDesc.fFrameSpeedMin != m_eEffectDesc.fFrameSpeedMax)
					((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.z = Random::Generate_Float(m_eEffectDesc.fFrameSpeedMin, m_eEffectDesc.fFrameSpeedMax);
				else
					((VTXINSTANCEEFFECT*)SubResource.pData)[i].vFrame.z = m_eEffectDesc.fFrameSpeedMin;
			}
		}

		// CurSpeedIndex
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vLinearSpeed.w = 0;

		// CurSizeIndex
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vColor.w = 0;
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional3.y = 0;

		// CurGravityIndex
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.w = 0;

		// SizeYAcc
		((VTXINSTANCEEFFECT*)SubResource.pData)[i].vAdditional2.x = 1;
	}

	m_pContext->Unmap(m_pVBInstance, 0);

	m_dTimeAcc = 0;
	m_dTimeAccDuration = 0;
	m_dTimeAccRateOverTime = 0;

	m_iCurRateOverTimeIndex = 0;

	if (CMasterEffect::OP_RAND_TWO_CONSTANT == m_eEffectDesc.eVelocitySpeedModifierOption)
		m_fSpeedModifier = Random::Generate_Float(m_eEffectDesc.fSpeedModifierMin, m_eEffectDesc.fSpeedModifierMax);
	else if (CMasterEffect::OP_CONSTANT == m_eEffectDesc.eVelocitySpeedModifierOption)
		m_fSpeedModifier = m_eEffectDesc.fSpeedModifierMin;
	else if (CMasterEffect::OP_CURVE == m_eEffectDesc.eVelocitySpeedModifierOption)
	{
		if (m_SpeedOverLifeTimes.size() > 0)
			m_fSpeedModifier = m_SpeedOverLifeTimes[0].fValue;
	}

	m_iNumInstanceUsing = 0.f;
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
}
