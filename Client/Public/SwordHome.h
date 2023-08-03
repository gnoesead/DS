#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CBone;
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CSwordHome final : public CGameObject
{
public:
	enum PlayerName {PLAYER_TANJIRO, PLAYER_KYOJURO, PLAYER_ZENITSU, PLAYER_END};
	
	typedef struct tagSwordDesc
	{
		_float4		vPos;

		PlayerName	m_PlayerName = { PLAYER_END };

		CBone* pBone = { nullptr };
		CTransform* pParentTransform = { nullptr };

	}SWORDDESC;

protected:
	CSwordHome(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSwordHome(const CSwordHome& rhs);
	virtual ~CSwordHome() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	_matrix Remove_Scale(_fmatrix TransformMatrix);

	void Set_SwordIn(_bool In) { m_isSwordIn = In; }

protected:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources( );


protected:
	CShader*	 m_pShaderCom = { nullptr };
	CRenderer*	 m_pRendererCom = { nullptr };
	CTransform*  m_pTransformCom = { nullptr };
	CModel*		 m_pModelCom = { nullptr };
	CModel* m_pModelCom_Sword = { nullptr };

protected:

	_bool		m_isSwordIn = { false };

	SWORDDESC			m_SwordDesc;
	_float4x4				m_WorldMatrix;

public:
	static  CSwordHome* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
