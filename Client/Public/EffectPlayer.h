#pragma once

#include "Engine_Defines.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CEffectPlayer
{
public:
	enum TEXTURETYPE { TEX_DIFFUSE, TEX_MASK, TEX_RAMP, TEX_NOISE, TEX_NOISE2, TEX_DISTORTION, TEX_NORMAL, TEX_END };
public:
	static CEffectPlayer* Get_Instance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CEffectPlayer;

		return m_pInstance;
	}
	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
private:
	CEffectPlayer();
	~CEffectPlayer();

public:
	class CParticleSystem* Reuse_Effect(const char* pTag, class CTransform* pTransformCom = nullptr);

	void Collect_ParticleSystem(class CParticleSystem* pParticleSystem);
	class CParticleSystem* Reuse_ParticleSystem();

	void Collect_EffectTexture(class CEffect* pEffect);
	class CEffect* Reuse_EffectTexture(class CTransform* pParentTransformCom, class CParticleSystem* pParent);

	void Collect_EffectMesh(class CEffect* pEffect);
	class CEffect* Reuse_EffectMesh(class CTransform* pParentTransformCom, class CParticleSystem* pParent);

	void Collect_EffectParticle(class CEffect* pEffect);
	class CEffect* Reuse_EffectParticle(class CTransform* pParentTransformCom, class CParticleSystem* pParent);

public:
	void Set_NumParticleSystem(_uint iNum) {
		m_iNumParticleSystem = iNum;
	}
	void Add_EffectTag(const char* pTag) {
		m_EffectTags.push_back(pTag);
	}
	void Add_Effect(const char* pTag, class CParticleSystem* pParticleSystem)
	{
		m_Effects.emplace(pTag, pParticleSystem);
	}
	void Add_TextureTag(int eTexturetype, const _tchar* pTag)
	{
		m_pTextureKeyList[eTexturetype].push_back(pTag);
	}
	void Add_ModelTag(const _tchar* pTag)
	{
		m_pModelKeyList.push_back(pTag);
	}

public:
	void	Play(const char* pEffectTag, class CTransform* pTransformCom = nullptr);
	void	Stop(const char* pEffectTag);
	_bool Find_ModelKey(const _tchar* pModelKey);
	_bool Find_TextureKey(int iTextureIndex, const _tchar* pTextureKey);

private:
	class CParticleSystem* Find_ParticleSystem(const char* pEffectTag);

private:
	static CEffectPlayer* m_pInstance;

private:
	unordered_map<const char*, class CParticleSystem*>	m_Effects;
	list<const char*>									m_EffectTags;
	_uint												m_iNumParticleSystem = { 0 };

	list<const _tchar*>									m_pModelKeyList;
	list<const _tchar*>									m_pTextureKeyList[TEX_END];

private:
	list<class CParticleSystem*>						m_ParticleSystemPool;
	list<class CEffect*>								m_EffectTexturePool;
	list<class CEffect*>								m_EffectMeshPool;
	list<class CEffect*>								m_EffectParticlePool;

public:
	void Release(void);
};

END