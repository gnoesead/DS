#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Effect final : public CVIBuffer
{
private:
	CVIBuffer_Point_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Effect(const CVIBuffer_Point_Effect& rhs);
	virtual ~CVIBuffer_Point_Effect() = default;

public:
	virtual HRESULT Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CVIBuffer_Point_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free(void) override;
};

END