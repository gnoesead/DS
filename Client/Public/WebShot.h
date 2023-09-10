#pragma once

#include "Client_Defines.h"
#include "MasterEffect.h"
#include "Monster.h"
#include "Swamp.h"

BEGIN(Engine)

END

BEGIN(Client)

class CWebShot final : public CMasterEffect
{
public:
	typedef struct tagWebDesc
	{
		CGameObject::WORLDINFO		WorldInfo;

		_int	iWebTexIndex; // 0:, 1:, 2:, 3:

		_float3 vScale = { 1.0f, 1.0f, 1.0f };
		_float4	vDir = { 0.0f, 0.0f, -1.0f, 0.0f };
		_float	fSpeed = { 1.0f };
		//_double	dDelay = 0.0;

		_float	fTurn = { 0.0f };
		
	}WEBDESC;
private:
	CWebShot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWebShot(const CWebShot& rhs);
	virtual ~CWebShot() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	
public:
	void	Initialize_Create_Coll();

	void	Tick_For_Index(_double dTimeDelta);

	HRESULT Render_For_Index();

public:
	void	Make_AttackColl(const _tchar* pLayerTag, _float3 Size, _float3 Pos, _double DurationTime, CAtkCollider::ATK_TYPE AtkType, _vector vDir, _float fDmg, CAtkCollider::BULLET_TYPE eBulletType = CAtkCollider::TYPE_DEFAULT);

	_vector	Calculate_Dir_From_Pos(_float4 Pos);
	_float	Calculate_Distance_From_Pos(_float4 Pos);

private:
	CShader*		 m_pShaderCom = { nullptr };
	CTexture*		 m_pTextureCom = { nullptr };
	CRenderer*		 m_pRendererCom = { nullptr };
	CVIBuffer_Rect*	 m_pVIBufferCom = { nullptr };
	CTransform*		 m_pTransformCom = { nullptr };


private:
	WEBDESC		m_WebDesc;

	_bool		m_isFirst = { true };

	_double		m_dDelay_All = { 0.0 };

	
	_float		m_fUv_Web = { 1.0f };


	//히트시 삭제 관련
	_bool		m_isHit = { false };
	_bool		m_isFirst_Hit = { true };
	_float		m_fAlphaCut = { 0.1f };

private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources(_int icross);

public:
	static CWebShot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END