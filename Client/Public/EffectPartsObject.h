#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "EffectPlayer.h"

BEGIN(Engine)
class CBone;
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CEffectPartsObject final : public CGameObject
{
public:
	typedef struct tagPartsEffectDesc
	{
		CBone* pBone = { nullptr };
		CTransform* pParentTransform = { nullptr };
	}PARTSEFFECTDESC;

protected:
	CEffectPartsObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectPartsObject(const CEffectPartsObject& rhs);
	virtual ~CEffectPartsObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;

	void	Play_Effect(const char* pEffectTag, CEffectPlayer::EFFECTWORLDDESC* pEffectWorldDesc = nullptr);

	_matrix Get_WorldMatrix() {
		return Convert::ToMatrix(m_WorldMatrix);
	}

protected:
	HRESULT Add_Components();


protected:
	CTransform*  m_pTransformCom = { nullptr };

protected:
	_bool		m_isSwordIn = { false };

	PARTSEFFECTDESC			m_PartsDesc;
	_float4x4				m_WorldMatrix;

public:
	static  CEffectPartsObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
