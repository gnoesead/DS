#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"
#include "Collider.h"
#include "EffectPlayer.h"

BEGIN(Engine)

#ifdef _DEBUG
class CRenderer;
class CNavigation;
#endif // _DEBUG
END

BEGIN(Client)

class CAtkCollider final : public CGameObject
{
public:
	enum ATK_TYPE { TYPE_SMALL, TYPE_CONNECTSMALL, TYPE_BIG, TYPE_BLOW, TYPE_BIGBLOW, TYPE_SPIN, TYPE_UPPER, TYPE_BOUND, TYPE_CUTSCENE, TYPE_HEKIREKI, TYPE_SWAMP, TYPE_WEB, TYPE_EFFECT, TYPE_END };
	enum BULLET_TYPE { TYPE_DEFAULT, TYPE_BULLET, TYPE_KYOGAI_BULLET, TYPE_KYOGAI_DELAY_BULLET, TYPE_KYOGAI_VERTICAL_BULLET, TYPE_KYOGAI_HORIZON_BULLET, TYPE_KYOGAI_LIAR_BULLET,
		TYPE_BULLET_WEB, TYPE_BULLET_WEB_FULL, TYPE_BULLET_END };
	
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
		CEffectPlayer::EFFECTWORLDDESC EffectWorldDesc;
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

	_vector Get_Dir() { return m_vDir; }
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
	void	Tick_KyogaiVerticalBullet(_double dTimeDelta);
	void	Tick_KyogaiHorizonBullet(_double dTimeDelta);
	void	Tick_KyogaiLiarBullet(_double dTimeDelta);
	void	Tick_WebBullet(_double dTimeDelta);


	void	Setting_BaseBullet();
	void	Setting_KyogaiBullet();
	void	Setting_KyogaiDelayBullet();
	void	Setting_KyogaiVerticalBullet();
	void	Setting_KyogaiHorizonBullet();
	void	Setting_KyogaiLiarBullet();
	void	Setting_WebBullet();
	void	Setting_WebBullet_Full();

private:
	CTransform* m_pTransformCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CNavigation* m_pNavigationCom = { nullptr };
	list<CGameObject*>	m_AtkObj;

private:
	ATKCOLLDESC		m_AtkCollDesc;
	_double			m_dTimeAcc = { 0.0 };
	_double			m_dStopAcc = { 0.0 };

	_uint			m_iCollCount = { 0 };
	_bool			m_bStop = { false };


	_vector			m_vDir = { 0.f,0.f,0.f,0.f };

	_int	m_iWebFull_Index = { 0 };


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