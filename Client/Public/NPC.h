#pragma once

#include "Client_Defines.h"
#include "Character.h"
#include "FIcon.h"
#include "Interaction.h"

BEGIN(Engine)

END

BEGIN(Client)

class CNPC : public CCharacter
{
public:
	

protected:
	CNPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPC(const CNPC& rhs);
	virtual ~CNPC() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth();


protected:
	void Create_Icon();
	void Create_Interaction();


protected: //애니메이션 제어용 함수들
	void	Dir_Setting(_bool Reverse);

	void Get_PlayerComponent();
	_bool Check_Distance(_float fDistance);
	_float Calculate_Distance();
	_vector Calculate_Dir();
	_vector Calculate_Dir_FixY();
	_vector Calculate_Dir_Cross();

	_float	Calculate_To_Spot();

	void	Find_Section();

protected: // 애니메이션 제어용 변수들
	CTransform* m_pPlayerTransformCom = { nullptr };

protected:
	/* 임시 코드 */
	_uint	m_iNumAnim = { 0 };
	// 렌더 확인용
	_uint	m_iMeshNum = { 0 };

	
	_int	m_iPlayer_Section = { 0 };
	_int	m_iPlayer_Section_Sub = { 0 };

protected:
	// Outline Default
	_float	m_fOutlineThickness = 0.9f;
	// Outline Face
	_float	m_fOutlineFaceThickness = 0.3f;

	//워크스팟
	_int	m_iSpot_Index = { 0 };
	_bool	m_isSpot_Reverse = { false };
	
private:
	CFIcon* m_pIcon = { nullptr };
	CInteraction* m_pInteraction = { nullptr };
	CInteraction* m_pInteraction_Back = { nullptr };

	_vector m_Player_Pos = {};
	_bool m_Sub_Up_Done = { false };

protected:
	HRESULT Add_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CNPC* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END