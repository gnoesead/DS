#pragma once

#include "Client_Defines.h"
#include "MasterEffect.h"

BEGIN(Engine)

class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;

END

BEGIN(Client)

class CSmeshStone final : public CMasterEffect
{
public:
	enum TYPE { TYPE_NORMAL , TYPE_DISAPEEAR };
	typedef struct tagEffectDesc
	{
		_vector vPos;
		_float fScale = 2.f;
		
	}EFFECTDESC;

private:
	CSmeshStone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSmeshStone(const CSmeshStone& rhs);
	virtual ~CSmeshStone() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();

private:
	CTransform* m_pTransformCom = { nullptr };

private:
	EFFECTDESC			m_EffectDesc;

	_double				m_TimeAcc = { 0.0 };

	_bool				m_bPlay = false;
	
public:
	static CSmeshStone* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END