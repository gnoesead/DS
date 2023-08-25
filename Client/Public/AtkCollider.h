#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"
#include "Collider.h"

BEGIN(Engine)

#ifdef _DEBUG
class CRenderer;
#endif // _DEBUG
END

BEGIN(Client)

class CAtkCollider final : public CGameObject
{
public:
	enum ATK_TYPE { TYPE_SMALL, TYPE_CONNECTSMALL, TYPE_BIG, TYPE_BLOW, TYPE_BIGBLOW, TYPE_SPIN, TYPE_UPPER, TYPE_BOUND, TYPE_CUTSCENE, TYPE_HEKIREKI, TYPE_EFFECT, TYPE_END };
	enum BULLET_TYPE { TYPE_DEFAULT, TYPE_BULLET, TYPE_KYOGAIBULLET, TYPE_KYOGAIDELAYBULLET,TYPE_BULLET_END };
	
	typedef struct tagAtkCollDesc
	{
		CTransform* pParentTransform = { nullptr };
		CTransform::TRANSFORMDESC TransformDesc;
		CCollider::COLLIDERDESC ColliderDesc;
		_double		dLifeTime = { 0.0 };

		_float		fDamage = { 0.0f };

		_float3		fPos = { 0.f, 0.f, 0.f };

		_double 	Speed = { 5.0 };
		ATK_TYPE	eAtkType = {TYPE_END};
		BULLET_TYPE eBulletType = { TYPE_BULLET_END };

		_float4		AtkDir = { 0.f, 0.f, 0.f, 0.f };
		
		_bool		bBullet;
		char		pEffectTag[MAX_PATH] = { "" };
	}ATKCOLLDESC;
private:
	CAtkCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAtkCollider(const CAtkCollider& rhs);
	virtual ~CAtkCollider() = default;

public:
	CCollider* Get_Collider() {
		return m_pColliderCom;
	}
	_uint Get_CollCount() {
		return m_iCollCount;
	}
	_bool Get_IsAttack(CGameObject* pHitObj);
	void Reset_AtkCollider(ATKCOLLDESC* pAtkCollDesc);

	void Add_AtkObejct(CGameObject* pHitObj);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double dTimeDelta) override;
	virtual void LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Setting_AtkCollDesc();

private:
	void	Tick_Default(_double dTimeDelta);
	void	Tick_BaseBullet(_double dTimeDelta);
	void	Tick_KyogaiBullet(_double dTimeDelta);
	void	Tick_KyogaiDelayBullet(_double dTimeDelta);

	void	Setting_BaseBullet();
	void	Setting_KyogaiBullet();
	void	Setting_KyogaiDelayBullet();

private:
	CTransform* m_pTransformCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	list<CGameObject*>	m_AtkObj;

private:
	ATKCOLLDESC		m_AtkCollDesc;
	_double			m_dTimeAcc = { 0.0 };

	_uint			m_iCollCount = { 0 };
	_bool			m_bSaveTransform = { false };

	_vector			m_vDir = { 0.f,0.f,0.f,0.f };



#ifdef _DEBUG
private:
	CRenderer* m_pRendererCom = { nullptr };
#endif // _DEBUG

private:
	HRESULT Add_Components();
public:
	static CAtkCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END