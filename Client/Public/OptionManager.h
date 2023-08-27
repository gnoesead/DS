#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class COptionManager final : public CBase
{
	DECLARE_SINGLETON(COptionManager)

private:
	COptionManager();
	virtual ~COptionManager() = default;

public:
	HRESULT Inititalize();
	void Tick(_double dTimeDelta);

	void Set_Is_Option_On(_bool Is);
	_bool Get_Is_Option_On();

	_bool Get_Is_Move_Done();

	_float Get_Option_Move();

	void Set_Is_Reset(_bool Is);
	_bool Get_Is_Reset();

	void Set_Select_Num(_int num);
	_int Get_Select_Num();

public: // 可记 楷悼
	void Set_Camera_Option(_uint i, _int num);
	_int Get_Camera_Option(_uint i);

	void Set_Graphic_Option(_uint i, _int num);
	_int Get_Graphic_Option(_uint i);

	void Set_Sound_Option(_uint i, _int num);
	_int Get_Sound_Option(_uint i);

	void Set_Is_Set_Origin_Light(_bool Is);

private:
	_bool m_Is_Option_On = { false };
	_bool m_Is_Move_Done = { false };

	_float m_Option_Move = { 40.f };
	_bool  m_Is_Reset = { false };

	_int   m_Select_Num = { 0 };


private: // 可记 楷悼
	_int m_Option_Line = { 0 };

	_int m_Camera_Option[2] = { 10,0 };
	_int m_Graphic_Option[3] = { 10,0,1};
	_int m_Sound_Option[4] = {20,20,20,20};


	_bool m_Is_Set_Origin_Light = { false };
	_vector m_Light_Origin = {};


public:
	virtual void Free() override;
};

END