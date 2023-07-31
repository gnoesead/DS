#pragma once

#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance_Effect final : public CVIBuffer_Instance
{
public:
	typedef struct tagInstanceDesc
	{
		/* 큐브형태의 범위지정. */
		_float3				vRange;
		_float				fMinSpeed, fMaxSpeed;			
		class CEffect*		pParent = { nullptr };
		_int				eShapeType = { 0 };
		_float				fStartLifeTimeMin = { 0 };
		_float				fLifetime = { 0 };
		_bool				isLooping = { true };
	}INSTANCEDESC;

	typedef struct tagParticleDesc
	{
		_float				fSpeed = { 0.f };
		//_float3			vRotation = { 0.f, 0.f, 0.f };
	}PARTICLEDESC;

private:
	CVIBuffer_Point_Instance_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instance_Effect(const CVIBuffer_Point_Instance_Effect& rhs);
	virtual ~CVIBuffer_Point_Instance_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;
//
//public:
//	// 함수로 기능 제어
	virtual void Tick(_double TimeDelta) {};
	virtual void Tick(_double TimeDelta, INSTANCEDESC* pDesc);
//	void InitialSetting();

//protected:
//	_uint						m_iNumIndicesPerInstance = { 0 };
//	_uint						m_iInstanceStride = { 0 };
//	_uint						m_iNumInstance = { 0 };
//	ID3D11Buffer*				m_pVBInstance = { nullptr };

private:
	PARTICLEDESC*		m_pParticleDescriptions = { nullptr };
	INSTANCEDESC		m_InstanceDesc;
	_double				m_dTimeAcc = { 0.0 };
	size_t				m_iStartIndex = { 0 };
	_bool				m_bStart = { true };

public:
	static CVIBuffer_Point_Instance_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END