#pragma once
#include "Base.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent*			Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag);
	class CComponent*			Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, _uint iIndex, const _tchar* pComponentTag);
	class CGameObject*			Get_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, _uint iIndex);
	list<class CGameObject*>*	Get_GameObjects(_uint iLevelIndex, const _tchar* pLayerTag);
	size_t						Get_GameObject_ListSize(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	HRESULT		Reserve_Containers(_uint iNumLevels);
	HRESULT		Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT		Add_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg, _bool isKeep);
	HRESULT		Add_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, class CGameObject* pGameObject, _bool isKeep);
	class CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, void* pArg);
	void		Clear_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	void        Time_Stop();
	void        Time_Stop_Off();

	void        Time_Slow(_double Time , _double Slow);

	_bool       Get_Is_Time_Stop();
	_bool       Get_Is_Time_Slow();
	_double     Get_Time_Slow_Amount();


public:
	void		Tick(_double dTimeDelta);
	void		LateTick(_double dTimeDelta);
	void		Clear(_uint iLevelIndex);

private: /*원본 객체*/
	unordered_map<const _tchar*, class CGameObject*>	m_uPrototypes;

private: /*사본 객체*/
	_uint		m_iNumLevels = { 0 }; /*동적 배열의 Size로 사용할 _uint*/

	unordered_map<const _tchar*, class CLayer*>*		m_pLayers = { nullptr };
	typedef unordered_map<const _tchar*, class CLayer*>	LAYERS;


private: /*시간 조작*/
	_double m_Slow = { 1 };
	_bool   m_Is_Slow = { false };
	_double m_Slow_TimeAcc = { 0 };
	_double m_Slow_Time = { 0 };


	_bool   m_Is_Stop = { false };
	





private:
	class CGameObject*	Find_Prototype(const _tchar* pPrototypeTag);
	class CLayer*		Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	virtual void Free() override;
};

END