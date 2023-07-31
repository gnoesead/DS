#include "pch.h"
#include "..\Public\EffectPlayer.h"

#include "ParticleSystem.h"

CEffectPlayer* CEffectPlayer::m_pInstance = nullptr;

CEffectPlayer::CEffectPlayer()
{

}

CEffectPlayer::~CEffectPlayer()
{
	Release();
}

void CEffectPlayer::Initialize()
{
}

void CEffectPlayer::Play(const char* pEffectTag, class CTransform* pTransformCom)
{
	CParticleSystem* pParticleSystem = Find_ParticleSystem(pEffectTag);
	pParticleSystem->Set_PartsParent(pTransformCom);
	pParticleSystem->Set_isPlaying(true);
	pParticleSystem->Set_isStopped(false);
	pParticleSystem->Play_Parts(true);
}

void CEffectPlayer::Stop(const char* pEffectTag)
{
	CParticleSystem* pParticleSystem = Find_ParticleSystem(pEffectTag);
	pParticleSystem->Set_isStopped(true);
	pParticleSystem->Set_isPlaying(false);
	pParticleSystem->Stop_Parts();
}

CParticleSystem* CEffectPlayer::Find_ParticleSystem(const char* pEffectTag)
{
	auto iter = find_if(m_Effects.begin(), m_Effects.end(), CTag_Finder_Char(pEffectTag));

	if (m_Effects.end() == iter)
		return nullptr;

	return iter->second;
}

void CEffectPlayer::Release()
{
	for(auto Pair : m_Effects)
		Safe_Release(Pair.second);

	m_Effects.clear();

	for (auto Tag : m_EffectTags)
		Safe_Delete_Array(Tag);

	m_EffectTags.clear();

	for(auto Key : m_pModelKeyList)
		Safe_Delete_Array(Key);

	m_pModelKeyList.clear();

	for (int i = 0; i < TEX_END; ++i)
	{
		for (auto Key : m_pTextureKeyList[i])
			Safe_Delete_Array(Key);

		m_pTextureKeyList[i].clear();
	}
}

void CEffectPlayer::Collect_Effect(CParticleSystem* pParticleSystem)
{
	//if (nullptr == pParticleSystem)
	//	return;

	//m_EffectPool.push_back(pParticleSystem);
}

CParticleSystem* CEffectPlayer::Reuse_Effect(_float3 vPos, _float3 vDir)
{
	return nullptr;
}