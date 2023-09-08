#pragma once
#include "Client_Defines.h"
#include "Base.h"

#include "AtkCollider.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CWebManager final : public CBase
{
	DECLARE_SINGLETON(CWebManager)

private:
	CWebManager();
	virtual ~CWebManager() = default;

	
public: // 함수
	void	Tick(_double dTimeDelta);

	void	Create_WebShot(_int TexIdx, _float4 CreatePos, _float3 Scale, _float4 ShotDir, _float Turn = 0.0f);
	void	Shoot_ArrowWeb(_float4 CreatePos, _float4 ShotDir);
	void	Shoot_WebBall(_float4 CreatePos, _float4 ShotDir);
	void	Shoot_WideWeb(_float4 CreatePos, _float4 ShotDir);
	void	Shoot_JikWeb(_float4 CreatePos, _float4 ShotDir, _float Turn);
	void	Shoot_SlideWeb(_float4 CreatePos, _float4 ShotDir, _float Turn);

	void	Make_WebBullet(_float3 Size, _float3 Pos, _vector vAtkDir, _double Speed, 
		CAtkCollider::BULLET_TYPE eBulletType, const char* pEffectTag);

public: // 겟셋
	void	Set_TransformCom(CTransform* pTransform) { 
		m_pTransformCom = pTransform;
	}
	void	Reset() {
		m_isTrigger_Akaza = false;
		m_isFirst_Akaza = true;
		m_iTrigger_Index = 1;
	}

	_bool	Get_Trigger_Akaza() { return m_isTrigger_Akaza; }

private: // 변수
	CTransform* m_pTransformCom = { nullptr }; // 플레이어의 트랜스폼임.


	//제어용
	_int	m_iTrigger_Index = { 1 };

	_bool	m_isTrigger_First = { false };
	_double m_dDelay_First = { 0.0 };

	_bool	m_isTrigger_Second = { false };
	_double	m_dDelay_Second = { 0.0 };

	_bool	m_isTrigger_Third = { false };
	_double m_dDelay_Third = { 0.0 };

	_bool	m_isTrigger_Akaza = { false };
	_double m_dDelay_Akaza = { 0.0 };
	_bool	m_isFirst_Akaza = { true };

public:
	virtual void Free() override;
};

END