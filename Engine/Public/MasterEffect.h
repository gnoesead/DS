#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CMasterEffect abstract : public CGameObject
{
public:
	enum TEXTURETYPE { TEX_DIFFUSE, TEX_MASK, TEX_RAMP, TEX_NOISE, TEX_NOISE2, TEX_DISTORTION, TEX_NORMAL, TEX_END };
	enum EFFECTTYPE { EFFECT_TEXTURE, EFFECT_MESH, EFFECT_PARTICLE, EFFECT_TYPE_END };
	enum SHAPETYPE { RECTANGLE, EDGE, CIRCLE, CONE, SPHERE, BOX, SHAPE_MESH, SPRITE, SHAPE_END };
	enum EFFECTOPTION { OP_CONSTANT, OP_CURVE, OP_RAND_TWO_CONSTANT, OP_RAND_TWO_CURVE, OPTION_END };
	enum COLOROPTION { OP_COLOR, OP_GRADIENT, OP_RAND_TWO, OP_RAND_TWO_GRADIENT, OP_RAND, OP_COLOR_END };
	enum SPACE { SPACE_LOCAL, SPACE_WORLD, SPACE_END };
	enum SCALING { HIERARCHY, SCALING_LOCAL, SHAPE, SCALING_END };
	enum STOPACTION { ACT_NONE, DISABLE, DESTROY, ACT_END };
	enum EMITOPTION { BASE, VOLUME, EMIT_END };
	enum ARCMODE { RANDOM, LOOP, PING_PONG, BURST_SPREAD, MODE_END };
	enum TIMEMODEOPTION { OPT_LIFETIME, OPT_SPEED, TIMEMODE_OPT_END };
	enum INHERITMODE { INITIAL, CURRENT, INHERITMODE_END };
	enum CYCLE { CYCLE_COUNT, CYCLE_INFINITE, CYCLE_END };
	enum RENDERMODE { BILLBOARD, STRETCHED_BILLBOAD, HORIZsONTAL_BILLBOARD, VERTICAL_BILLBOARD, MESH, PARTICLE, RENDERMODE_END };
	enum MATERIALTYPE { MAT_NONE, DEFAULT_DIFFUSE, DEFAULT_MATERIAL, DEFAULT_PARTICLE, DEFAULT_PARTICLESYSTEM, MATERIALTYPE_END };
	enum SORTMODE { SORT_NONE, BY_DISTANCE, OLDEST_INFRONT, YOUNGEST_INFRONT, BYDEPTH, SORTMODE_END };
	enum CULLMODE { CULL_OFF, CULL_FRONT, CULL_BACK, CULLMODE_END };
	enum RENDERGROUPOPTION { NON_LIGHT, BLEND, RENDERGROUPOPTION_END };
	enum SHADEROPTION { OPT_DEFAULT, OPT_BY_ALPHA, OPT_BY_RED, OPT_BY_GREEN, OPT_BY_BLUE, OPT_NO_ZWRITE, OPT_BLACK_NO_DISCARD, OPT_END };

public:
	typedef struct tagLifetimeValue
	{
		float			fLifetime;
		float			fValue;
	}LIFETIMEVALUE;
	typedef struct tagEffectDesc
	{
		int				eEffectType = { EFFECT_TYPE_END };	//EFFECTTYPE
		// ParticleSystem
		float			fDuration = { 5.f };
		bool			isLooping = { true };
		bool			isPrewarm = { false };
		bool			isRandomStartDelay = { false };
		float			fStartDelayMin = { 0.f };
		float			fStartDelayMax = { 0.f };
		int				eStartLifeTimeOption = { OP_CONSTANT };		//EFFECTOPTION
		float			fStartLifeTimeMin = { 3.f };
		float			fStartLifeTimeMax = { 0.f };
		int				eStartSpeedOption = { OP_CONSTANT };	//EFFECTOPTION
		float			fStartSpeedMin = { 5.f };
		float			fStartSpeedMax = { 0.f };
		int				eStartSizeOption = { OP_CONSTANT };		//EFFECTOPTION
		bool			is3DStartSize = { false };
		_float3			vStartSizeMin = { 1.f, 1.f, 1.f };
		_float3			vStartSizeMax = { 1.f, 1.f, 1.f };
		int				eStartRotationOption = { OP_CONSTANT };		//EFFECTOPTION
		bool			is3DStartRotation = { false };
		_float3			vStartRotationMin = { 0.f, 0.f, 0.f };
		_float3			vStartRotationMax = { 0.f, 0.f, 0.f };
		int				eStartColorOption = { OP_COLOR };		//COLOROPTION
		_float4			vColor = { 1.f, 1.f, 1.f, 1.f };
		int				eGravityModifierOption = { OP_CONSTANT };		//EFFECTOPTION					// 추가
		_float2			vGravityModifier = { 0.f, 1.f };												// 추가
		int				iNumGravityModiferOverLifetimes = { 0 };										// 추가
		int				eSimulationSpace = { SPACE_LOCAL };		//SPACE
		float			fSimulationSpeed = 1.f;
		int				eScalingMode = { SCALING_LOCAL };	//SCALING
		int				iMaxParticles = { 1 };		// 1000
		int				eStopAction = { ACT_NONE };		//STOPACTION
		// Emission
		bool			isEmission = { true };
		int				eRateOverTimeOption = { OP_CONSTANT };		//EFFECTOPTION
		float			fRateOverTimeMin = { 10.f };
		float			fRateOverTimeMax = { 0.f };
		int				iNumRateOverTime = { 0 };
		int				eRateOverDistanceOption = { OP_CONSTANT };		//EFFECTOPTION
		float			fRateOverDistanceMin = { 0.f };
		float			fRateOverDistanceMax = { 0.f };
		int				iNumBursts = { 0 };
		int				iNumParticlesPerFrame = { 1 };
		// Shape
		bool			isShape = { true };
		bool			isSpark = { false };
		float			fYExtendSpeed = { 1.f };
		_float2			fYExtendEndSize = { 1.f, 3.f };		// min, max
		int				eShapeType = { RECTANGLE };		//SHAPETYPE
		float			fShapeAngle = { 25.f };
		float			fShapeRadius = { 1.f };
		float			fShapeRadiusTickness = { 1.f };
		float			fArc = { 360.f };
		int				eArcMode = { RANDOM };		//ARCMODE
		float			fArcSpread = { 0.f };
		//CTexture		pTexture = { nullptr }
		int				eEmitFromOption = { EMIT_END };		//EMITOPTION
		float			fArcLength = { 0.f };
		_float3			vShapePosition = { 0.f, 0.f, 0.f };
		_float3			vShapeRotation = { 0.f, 0.f, 0.f };
		_float3			vShapeScale = { 1.f, 1.f, 1.f };
		bool			isAlignToDirection = { false };
		// Velocity over Lifetime
		bool			isVelocity = { false };
		int				eVelocityLinearOption = { OP_CONSTANT };	//EFFECTOPTION
		_float3			vVelocityLinearMin = { 0.f, 0.f, 0.f };
		_float3			vVelocityLinearMax = { 0.f, 0.f, 0.f };
		int				eSpace = { SPACE_LOCAL };		//SPACE
		int				eVelocityOrbitalOption = { OP_CONSTANT };		//EFFECTOPTION
		_float3			vVelocityOrbitalMin = { 0.f, 0.f, 0.f };
		_float3			vVelocityOrbitalMax = { 0.f, 0.f, 0.f };
		int				eVelocityOffsetOption = { OP_CONSTANT };		//EFFECTOPTION
		_float3			vVelocityOffsetMin = { 0.f, 0.f, 0.f };
		_float3			vVelocityOffsetMax = { 0.f, 0.f, 0.f };
		int				eVelocityRadialOption = { OP_CONSTANT };		//EFFECTOPTION
		float			fRadialMin = { 0.f };
		float			fRadialMax = { 0.f };
		int				eVelocitySpeedModifierOption = { OP_CONSTANT };		//EFFECTOPTION
		float			fSpeedModifierMin = { 1.f };
		float			fSpeedModifierMax = { 0.f };
		int				iNumSpeedOverLifeTimes = { 0 };
		// Limit Velocity over Lifetime
		bool			isSeparateAxesLimitVelocity = { false };
		int				eLimitVelocitySpeedOption = { OP_CONSTANT };	//EFFECTOPTION
		_float3			vLimitVelocitySpeedMin = { 1.f, 0.f, 0.f };
		_float3			vLimitVelocitySpeedMax = { 1.f, 0.f, 0.f };
		int				eLimitVelocitySpace = { SPACE_LOCAL };		//SPACE
		// Inherit Velocity
		bool			isInheritVelocity = { false };
		int				eInheritMode = { INITIAL };			//INHERITMODE
		// Force over Lifetime
		bool			isForceOverLifetime = { false };
		_float3			vForce = { 0.f, 0.f, 0.f };
		int				eSpaceForce = { SPACE_LOCAL };		//SPACE
		// Size over Lifetime
		bool			isSizeOverLifetime = { false };
		bool			isSeparateAxesSzOverLifeTime = { false };
		int				eSizeOverLifetimeOption = { OP_CURVE };		//EFFECTOPTION
		int				iNumSizes[3] = { 0, 0, 0 };

		// Rotation over Lifetime
		bool			isRotationOverLifetime = { false };
		bool			isSeparateAxesRotOverLifeTime = { false };
		int				eRotOverLifetimeOption = { OP_CURVE };		//EFFECTOPTION
		int				iNumRotations[3] = { 0, 0, 0 };

		// Position over Lifetime
		bool			isPositionOverLifetime = { false };
		int				ePosOverLifetimeOption = { OP_CURVE };		//EFFECTOPTION
		int				iNumPositions[3] = { 0, 0, 0 };

		// Rotation by Speed
		bool			isRotationBySpeed = { false };
		bool			isSeparateAxesRotBySpeed = { false };
		int				eAngularVelocityOption = { OPTION_END };	//EFFECTOPTION
		_float3			vAngularVelocity = { 45.f, 0.f, 0.f };
		// Collision
		// SubEmitters
		// Texture Sheet Animation
		bool			isTextureSheetAnimation = { false };
		_float2			vTiles = { 1.f, 1.f };
		int				eTimeModeOption = { OPT_SPEED };		//TIMEMODEOPTION
		int				eFrameOverTimeOption = { OP_CURVE };	//EFFECTOPTION
		int				eStartFrameOption = { OP_CONSTANT };	//EFFECTOPTION
		int				iNumFrameOverTime = { 0 };
		_float2			iStartFrame = { 0.f, 0.f };		// Min, Max
		float			fFrameSpeedMin = { 0.f };
		float			fFrameSpeedMax = { 0.f };
		// Trail
		// 
		// Renderer
		bool			isRenderer = { true };
		int				eRenderMode = { BILLBOARD };		//RENDERMODE
		int				eMaterialType = { DEFAULT_PARTICLESYSTEM };		//MATERIALTYPE
		int				eSortMode = { SORT_NONE };		//SORTMODE
		float			fSortingFudge = { 0.f };
		float			fMinParticleSize = { 0.f };
		float			fMaxParticleSize = { 0.5f };
		_float3			vPivot = { 0.f, 3.f, 0.f };
		float			fTextureOrder = { 0.f };
		int				eRenderGroupOption = { NON_LIGHT };	//RENDERGROUPOPTION

		// Texture
		_float2			vFlip = { 1.f, 1.f };
		float			fCutOff = { 0.f };		// Dissolve 기능, DissolveAmount (0-1)
		float			fCutOffSoftness = { 0.f };	// 카툰은 0, 실사는 1에 가깝게
		_float4			vBurnColor = { 0.f, 0.f, 0.f, 0.f };		// 끝이 타들어가는 느낌
		float			fBurnSize = { 0.f };
		float			fDissolveDelay = { 0.f };
		float			fDissolveSpeed = { 0.f };
		_float2			vDiffuseTilling = { 1.f, 1.f };
		_float2			vDiffuseOffset = { 0.f, 0.f };
		_float2			vGradientOffset = { 0.f, 0.f };
		_float2			vGradientTilling = { 1.f, 1.f };
		_float2			vPaddingDelayStartEnd = { 0.f, 0.f };
		_float2			vPaddingSpeedStart = { 0.f, 0.f };
		_float2			vPaddingSpeedEnd = { 0.f, 0.f };
		float			fDistortionStrength = { 0.f };
		float			fDistortionSpeed = { 0.f };
		//_float4		vBaseColor = { 0.f, 0.f, 0.f, 0.f };
		bool			hasSecondTexture = { false };
		_float4			vNoisePanningSpeed = { 0.f, 0.f, 0.f, 0.f };
		_float4			vSecondaryNoisePanningSpeed = { 0.f, 0.f, 0.f, 0.f };
		_float4			vPanningSpeed = { 0.f, 0.f, 0.f, 0.f };
		_float4			vNormalPanningSpeed = { 0.f, 0.f, 0.f, 0.f };
		bool			isPolarCoordinate = { false };
		float			fCircleMaskOuterRadius = { 1.f };
		float			fCircleMaskInnerRadius = { 0.f };
		float			fCircleMaskSmoothness = { 0.f };
		float			fRectangleMaskOuterRadius = { 1.f };
		float			fRectangleMaskInnerRadius = { 0.f };
		float			fRectangleMaskSmoothness = { 0.f };
		int				eCullMode = { CULL_OFF };		//CULLMODE
		_float2			vDiscardedPixelMin = { 0.f, 0.f };
		_float2			vDiscardedPixelMax = { 1.f, 1.f };
		_float2			vPixelDiscardSpeedMin = { 0.f, 0.f };
		_float2			vPixelDiscardSpeedMax = { 0.f, 0.f };
		float			fPixelDiscardDelay = { 0.f };
		int				iNumAlpha = { 0 };

		//Material
		_float3			vModelPivotScale = { 1.f, 1.f, 1.f };
		_float3			vModelPivotRot = { 0.f, 0.f, 0.f };
		_float3			vModelPivotPos = { 0.f, 0.f, 0.f };

		int				eTextureShaderOption[TEX_END] = { OPT_DEFAULT };	//SHADEROPTION

		float			fTimeAcc = { 0.0 };
	}EFFECTDESC;

protected:
	CMasterEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMasterEffect(const CMasterEffect& rhs);
	virtual ~CMasterEffect() = default;
	
public:
	_float	Get_Order() const {	//Renderer Sorting용
		return m_fTextureOrder;
	}
	void	Set_Order(_float fOrder) {	//Renderer Sorting용
		m_fTextureOrder = fOrder;
	}

	EFFECTDESC	Get_EffectDesc() const {
		return m_eEffectDesc;
	}
	vector<LIFETIMEVALUE>	Get_RateOverTimes() const {
		return m_RateOverTimes;
	}
	vector<LIFETIMEVALUE>	Get_SpeedOverTimes() const {
		return m_SpeedOverLifeTimes;
	}
	vector<LIFETIMEVALUE>	Get_GravityModiferOverTimes() const {
		return m_GravityModiferOverLifetimes;
	}
	vector<LIFETIMEVALUE>	Get_FrameOverTimes() const {
		return m_FrameOverTime;
	}
	vector<LIFETIMEVALUE>	Get_SizeOverTimesX() const {
		return m_SizeOverLifeTimes[0];
	}
	LIFETIMEVALUE* Get_RateOverTime(int iIndex) {
		return &m_RateOverTimes[iIndex];
	}
	LIFETIMEVALUE* Get_SpeedOverLifeTime(int iIndex) {
		return &m_SpeedOverLifeTimes[iIndex];
	}
	LIFETIMEVALUE* Get_GravityOverLifeTime(int iIndex) {
		return &m_GravityModiferOverLifetimes[iIndex];
	}
	LIFETIMEVALUE* Get_FrameOverTime(int iIndex) {
		return &m_FrameOverTime[iIndex];
	}

protected:
	EFFECTDESC				m_eEffectDesc;
	_float					m_fTextureOrder = { 0.f };

	vector<LIFETIMEVALUE>	m_SizeOverLifeTimes[3];	// x,y,z

	vector<LIFETIMEVALUE>	m_RateOverTimes;
	vector<LIFETIMEVALUE>	m_SpeedOverLifeTimes;
	vector<LIFETIMEVALUE>	m_GravityModiferOverLifetimes;
	vector<LIFETIMEVALUE>	m_FrameOverTime;

public:
	virtual void Free() override;
};

END