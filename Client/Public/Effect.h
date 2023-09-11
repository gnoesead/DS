#pragma once

#include "Client_Defines.h"
#include "MasterEffect.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
class CShader;
END

BEGIN(Client)

class CEffect : public CMasterEffect
{
public:
	typedef struct tagParentDesc
	{
		CTransform* pParentTransform = { nullptr };
		class CParticleSystem* pParent = { nullptr };
	}PARENTDESC;

	typedef struct tagBurstDesc
	{
		float			fTime = { 0.f };
		int				eCountOption = { OPTION_END };		//EFFECTOPTION
		int				iCountMin = { 0 };
		int				iCountMax = { 0 };
		int				eCycle = { CYCLE_END };		//CYCLE
		int				iNumCycles = { 0 };
		float			fInterval = { 0.f };
		float			fProbability = { 0.f };
	}BURSTDESC;

protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

public:
	virtual HRESULT	Initialize_Prototype(void) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double dTimeDelta) override;
	virtual void	LateTick(_double dTimeDelta) override;
	virtual HRESULT Render(void) override;

public:
	EFFECTDESC* Get_EffectDesc(void) {
		return &m_eEffectDesc;
	}
	int Get_StartLifeTimeOption(void) {
		return m_eEffectDesc.eStartLifeTimeOption;
	}
	float Get_Duration(void) {
		return m_eEffectDesc.fDuration;
	}
	float Get_StartLifeTimeMin(void) {
		return m_eEffectDesc.fStartLifeTimeMin;
	}
	float Get_StartLifeTimeMax(void) {
		return m_eEffectDesc.fStartLifeTimeMax;
	}
	int Get_EffectType(void) {
		return m_eEffectDesc.eEffectType;
	}
	BURSTDESC* Get_BurstDesc(int iIndex)
	{
		return &m_BurstList[iIndex];
	}
	LIFETIMEVALUE* Get_SizeOverLifeTime(int iAxis, int iIndex) {
		return &m_SizeOverLifeTimes[iAxis][iIndex];
	}
	LIFETIMEVALUE* Get_RotationOverLifeTime(int iAxis, int iIndex) {
		return &m_RotOverLifeTimes[iAxis][iIndex];
	}
	LIFETIMEVALUE* Get_PositionOverLifeTime(int iAxis, int iIndex) {
		return &m_PosOverLifeTimes[iAxis][iIndex];
	}
	LIFETIMEVALUE* Get_AlphaOverLifeTime(int iIndex) {
		return &m_AlphaOverLifetimes[iIndex];
	}
	const _tchar* Get_ModelKey(void) {
		return m_szModelKey;
	}
	const _tchar* Get_TextureKey(int iIndex) {
		return m_szTextureKeys[iIndex];
	}
	int Get_SimulationSpace(void) {
		return m_eEffectDesc.eSimulationSpace;
	}
	bool Get_IsSetYToGround(void) {
		return m_eEffectDesc.isSetYToGround;
	}

public:
	void Set_Parent(class CTransform* pTransform, class CParticleSystem* pParent)
	{
		m_ParentDesc.pParentTransform = pTransform; m_ParentDesc.pParent = pParent;
	}
	void Set_EffectDesc(EFFECTDESC* pDesc)
	{
		m_eEffectDesc = *pDesc;
	}
	void Set_ModelKey(const _tchar* pKey) {
		lstrcpy(m_szModelKey, pKey);
	}
	void Set_TextureKey(int iIndex, const _tchar* pKey) {
		lstrcpy(m_szTextureKeys[iIndex], pKey);
	}
	void Set_EffectType(int eType) {
		m_eEffectDesc.eEffectType = eType;
	}
	void Set_PlaySpeed(_double dSpeed) {
		m_dPlaySpeed = dSpeed;
	}
	void Set_Alpha(_float fAlpha) {
		m_fAlpha = fAlpha;
	}

public:
	HRESULT Add_Component_Texture(_uint iLevelIndex, const _tchar* pComponentTag, int eType);
	void Reserve_BurstList(int iNumBurst) {
		m_BurstList.reserve(iNumBurst);
	}
	void Reserve_SizeOverLifeTime(int iIndex, int iNumSOL) {
		m_SizeOverLifeTimes[iIndex].reserve(iNumSOL);
	}
	void Reserve_RotationOverLifeTime(int iIndex, int iNumROL) {
		m_RotOverLifeTimes[iIndex].reserve(iNumROL);
	}
	void Reserve_PositionOverLifeTime(int iIndex, int iNumPOL) {
		m_PosOverLifeTimes[iIndex].reserve(iNumPOL);
	}
	void Reserve_AlphaOverLifeTime(int iNumAOL) {
		m_AlphaOverLifetimes.reserve(iNumAOL);
	}
	void Reserve_GravityModiferOverLifeTime(int iNumGOL) {
		m_GravityModiferOverLifetimes.reserve(iNumGOL);
	}
	void Reserve_SpeedOverLifeTime(int iNumSOL) {
		m_SpeedOverLifeTimes.reserve(iNumSOL);
	}
	void Reserve_RateOverLifeTime(int iNumROL) {
		m_RateOverTimes.reserve(iNumROL);
	}
	void Reserve_FrameOverLifeTime(int iNumFOL) {
		m_FrameOverTime.reserve(iNumFOL);
	}
	
	void Add_BurstDesc(BURSTDESC Desc) {
		m_BurstList.push_back(Desc);
	}
	void Add_SizeOverLifeTime(int iIndex, LIFETIMEVALUE value)
	{
		m_SizeOverLifeTimes[iIndex].push_back(value);
	}
	void Add_RotationOverLifeTime(int iIndex, LIFETIMEVALUE value)
	{
		m_RotOverLifeTimes[iIndex].push_back(value);
	}
	void Add_PositionOverLifeTime(int iIndex, LIFETIMEVALUE value)
	{
		m_PosOverLifeTimes[iIndex].push_back(value);
	}
	void Add_AlphaOverLifeTime(LIFETIMEVALUE value)
	{
		m_AlphaOverLifetimes.push_back(value);
	}
	void Add_GravityModiferOverLifetime(LIFETIMEVALUE value)
	{
		m_GravityModiferOverLifetimes.push_back(value);
	}
	void Add_SpeedOverLifetime(LIFETIMEVALUE value)
	{
		m_SpeedOverLifeTimes.push_back(value);
	}
	void Add_RateOverLifetime(LIFETIMEVALUE value)
	{
		m_RateOverTimes.push_back(value);
	}
	void Add_FrameOverLifetime(LIFETIMEVALUE value)
	{
		m_FrameOverTime.push_back(value);
	}
	virtual void Clear(void);
	virtual void Reset_Data(void);
	virtual void Set_Initial_Data(void);

protected:
	HRESULT Add_Components(void);
	virtual HRESULT SetUp_ShaderResources(void);
	virtual void Check_PassIndex(void) = 0;

protected:
	class CTransform*	m_pTransformCom = { nullptr };
	class CRenderer*	m_pRendererCom = { nullptr };
	class CShader*		m_pShaderCom = { nullptr };

	class CTexture*		m_pTextures[TEX_END] = { nullptr };

protected:
	_float4x4				m_WorldMatrix;
	PARENTDESC				m_ParentDesc;
	_uint					m_iPassIndex = { 0 };
	_float					m_fLifeTime = { 0.f };
	_uint					m_iCurSizeIndex[3] = { 0 };
	_uint					m_iCurRotIndex[3] = { 0 };
	_uint					m_iCurPosIndex[3] = { 0 };
	_uint					m_iCurAlphaIndex = { 0 };

	_float					m_fAlpha = { 1.f };
	_float					m_fDissolveAmount = { 0.f };

	_float3					m_vCurPos = { 0.f, 0.f, 0.f };

	_float					m_fCurFrame = { 0 };
	_float					m_fFrameTimeAcc = { 0.f };
	_float					m_fFrameSpeed = { 0.f };
	_uint					m_iCurFrameIndex = { 0 };

	vector<BURSTDESC>		m_BurstList;
	vector<LIFETIMEVALUE>	m_RotOverLifeTimes[3];	// x,y,z
	vector<LIFETIMEVALUE>	m_PosOverLifeTimes[3];	// x,y,z

	vector<LIFETIMEVALUE>	m_AlphaOverLifetimes;

	_tchar					m_szModelKey[MAX_PATH] = { TEXT("") };
	_tchar					m_szTextureKeys[TEX_END][MAX_PATH] = { TEXT("") };

	_bool					m_isRendering = { true };
	_bool					m_bCollect = { false };

	_float					m_fDelayTimeAcc = { 0.f };
	_float					m_fStartDelay = { 0.f };
	_float					m_fDiscardTimeAcc = { 0.f };
	_float					m_fDissolveTimeAcc = { 0.f };
	_float					m_fPaddingTimeStartAcc = { 0.f };
	_float					m_fPaddingTimeEndAcc = { 0.f };

	_float2					m_vPaddingStart = { 0.f, 0.f };
	_float2					m_vPaddingEnd = { 0.f, 0.f };

	_double					m_dPlaySpeed = { 1.0 };

	_float3					m_vStartSize = { 1.f, 1.f, 1.f };

public:
	//static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	//virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free(void) override;
};

END