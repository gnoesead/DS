#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CMasterEffect abstract : public CGameObject
{
protected:
	CMasterEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMasterEffect(const CMasterEffect& rhs);
	virtual ~CMasterEffect() = default;
	
public:
	_float	Get_Order() const {	//Renderer Sorting¿ë
		return m_fTextureOrder;
	}
	void	Set_Order(_float fOrder) {	//Renderer Sorting¿ë
		m_fTextureOrder = fOrder;
	}

protected:
	_float					m_fTextureOrder = { 0.f };

public:
	virtual void Free() override;
};

END