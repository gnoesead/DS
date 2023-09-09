#pragma once
#include "Client_Defines.h"
#include "VIBuffer_Point_Instance.h"

BEGIN(Engine)
END

BEGIN(Client)

class CVIBuffer_CustomParticle final : public CVIBuffer_Point_Instance
{
public:
	enum PARTICLE_TYPE { 
		TYPE_SIZE, TYPE_DIR, TYPE_SIZE_DIR, CHANGE_END 
	};

	typedef struct tagVIB_CustomParticleDesc
	{
		PARTICLE_TYPE	eParticleType = { TYPE_SIZE };
		_float		fLifeTime = { 1.f };
		_float		fTickPerSize = { 1.f };
		_float3		vTickPerDir = { 1.f, 1.f, 1.f };
		_int3		vDirOption = { 1, 1, 1 };
		_double		dSpriteSpeed = { 1.0 };
		_int2		vSpriteCount = { 1, 1 };

		CVIBuffer_Point_Instance::INSTANCEDESC		InstanceDesc;
	}VIB_CUSTOMPARTDESC;
private:
	CVIBuffer_CustomParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_CustomParticle(const CVIBuffer_CustomParticle& rhs);
	virtual ~CVIBuffer_CustomParticle() = default;

public:
	HRESULT Reset_Particle(void* pArg);

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;

private:
	VIB_CUSTOMPARTDESC	m_CustomPartDesc;
	_double				m_dTimeAcc = { 0.0 };
	_double				m_dCycle = { 1.0 };

private:
	void Tick_Size(_double dTimeDelta);
	void Tick_Dir(_double dTimeDelta);

	void Tick_Size_Dir(_double dTimeDelta);

public:
	static CVIBuffer_CustomParticle* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iNumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END