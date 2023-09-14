#pragma once

#include "EffectW.h"

BEGIN(Client)

class CGroundSmoke final : public CEffectW
{
public:
	enum SMOKE_TYPE{SMOKE_FALLDOWN , SMOKE_SMESHSPREAD , SMOKE_SIDESTEP , SMOKE_UPDOWN , SMOKE_DASHLAND , SMOKE_RUN , SMOKE_JUMP , SMOKE_BLADECREATE , SMOKE_BLADEDISAPPEAR
	,SMOKE_KYOGAI_KICKDOWN , SMOKE_KYOGAI_LAND , SMOKE_BLADE , SMOKE_JENITSU_HIKI 
	, SMOKE_TANJIRO_COMBODOWN_UPDOWN , SMOKE_TANJIRO_COMBODOWN_SPREAD , SMOKE_TANJIRO_COMBODOWN_KICK ,SMOKE_KYOGAI_PUSH, SMOKE_DEAD, SMOKE_TEST};


private:
	CGroundSmoke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGroundSmoke(const CGroundSmoke& rhs);
	virtual ~CGroundSmoke() = default;

public:
	virtual HRESULT Initialize_Prototype() ;
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta) ;
	virtual void LateTick(_double TimeDelta) ;
	virtual HRESULT Render() ;

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CGroundSmoke* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) ;
	virtual void Free() ;
};

END