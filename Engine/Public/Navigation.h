#pragma once

#include "Component.h"

/* 네비게이션을 구성하는 삼각형들을 모아서 보관한다. */
/* 네비게이션을 이용하는 객체가 어떤 쎌에 존재하는지에 대한 인덱스를 보관한다.  */
/* 쎌안에서의 움직임에대한 판단을 수행한다. */
BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNaviDesc
	{
		_int	iCurrentIndex = { -1 };
	}NAVIDESC;

protected:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	vector<class CCell*>		Get_Cells() { return m_Cells; }
	CCell* Get_Cell(_uint iIndex) {
		return m_Cells[iIndex];
	}

	const _int		Get_CurCellIndex() {
		return m_NaviDesc.iCurrentIndex;
	}

	void	Set_CurIndex(const _int iCurIndex) { m_NaviDesc.iCurrentIndex = iCurIndex; }
public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFile);
	virtual HRESULT Initialize(void* pArg) override;

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif

public: /* 네비게이션을 구성하는 각각의 셀들의 이웃(최대3개)를 셋팅한다.*/
	HRESULT	SetUp_Neighbors();
	_bool	is_MoveOnNavigation(_fvector & vPositio);
	_vector	MoveOnNavigation(_fvector& vPosition, _fvector vLook);
	_fvector Compute_Height(class CTransform* pOwnerTransform);
	_vector  SlidingVector(_fvector vPosition, _fvector vLook, _int* pNeighborIndex);

private:
	vector<class CCell*>			m_Cells;
	NAVIDESC						m_NaviDesc;

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif

public:
	static CNavigation* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext , const _tchar* pNavigationDataFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

END

