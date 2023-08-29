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

class CSmellBundle final : public CGameObject
{
public:
	enum TYPE { TYPE_NORMAL , TYPE_DISAPEEAR , TYPE_WORLD};
	typedef struct tagEffectDesc
	{
		_vector vPos;
		TYPE eType = TYPE_NORMAL;

	}EFFECTDESC;

private:
	CSmellBundle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSmellBundle(const CSmellBundle& rhs);
	virtual ~CSmellBundle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();

private:
	void	PlayerCheck(_double TimeDelta);

private:
	CTransform* m_pTransformCom = { nullptr };

private:
	EFFECTDESC			m_EffectDesc;

	_double				m_TimeAcc = { 0.0 };
	
public:
	static CSmellBundle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END