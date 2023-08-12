#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Collider.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CTransform;

END

BEGIN(Client)

class CCollisionBox  final : public CGameObject
{
public:
	enum COLLISION_TYPE { COLLISION_NORMAL, COLLISION_NAVI, COLLISION_INTERACTION, COLLISION_END };

	typedef struct tagCollisionBoxInfo
	{
		_float4		vPos;
		_float3		vScale;

		_uint		iCollisionType = 0;

	}COLLISIONBOX_INFO;

private:
	CCollisionBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollisionBox(const CCollisionBox& rhs);
	virtual ~CCollisionBox() = default;

public:
	COLLISIONBOX_INFO Get_CollisionBoxInfo() {
		return m_CollisionBoxInfo;
	}

	void	Set_Scale(_float3 vScale) { m_CollisionBoxInfo.vScale = vScale; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT Add_Components();

private:
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };

private:
	COLLISIONBOX_INFO		m_CollisionBoxInfo;

public:
	static CCollisionBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
