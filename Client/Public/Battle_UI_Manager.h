#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CGameInstance;
END

BEGIN(Client)

class CBattle_UI_Manager final : public CBase
{
	DECLARE_SINGLETON(CBattle_UI_Manager)

private:
	CBattle_UI_Manager();
	virtual ~CBattle_UI_Manager() = default;

public:
	HRESULT Inititalize();
	void Tick(_double dTimeDelta);

public:
	void Set_Player_Type(_uint Type);
	_uint Get_Player_Type();

	void Set_Player_Skill_Type(_uint Type);
	_uint Get_Player_Skill_Type();

	_bool Get_Skill_On();

public:
	void Set_Battle_Start_On(_bool On);
	_bool Get_Battle_Start_On();

	void Set_Battle_Start_Off(_bool Off);
	_bool Get_Battle_Start_Off();

	void Set_Battle_Finish_On(_bool On);
	_bool Get_Battle_Finish_On();

	void Set_Battle_Finish_Off(_bool Off);
	_bool Get_Battle_Finish_Off();

public:
	void Set_Battle_Result_On(_bool On);
	_bool Get_Battle_Result_On();

	void Set_Battle_Result_Off(_bool Off);
	_bool Get_Battle_Result_Off();

	void Set_Battle_Result_Size_Reset(_bool Is);
	_bool Get_Battle_Result_Size_Reset();

public:
	void Set_Battle_Result_On_2(_bool On);
	_bool Get_Battle_Result_On_2();

	void Set_Battle_Result_Off_2(_bool Off);
	_bool Get_Battle_Result_Off_2();


public: // Zen_Cut_In
	void Set_Zen_UI_Num(_int On);
	_int Get_Zen_UI_Num();



private:
	_uint m_Player_Index = { 0 };
	_uint m_Player_Skill_Index = { 0 };

	_bool m_Skill_On = { false };
	_float m_Skill_On_TimeAcc = { 0.f };


private:
	_bool m_Battle_Start_On = { false };
	_bool m_Battle_Start_Off = { false };
	_float m_Battle_Start_TimeAcc = { 0.f };
	_bool m_Battle_Start_Time = { false };

	_bool m_Battle_Finish_On = { false };
	_bool m_Battle_Finish_Off = { false };
	_float m_Battle_Finish_TimeAcc = { 0.f };
	_bool m_Battle_Finish_Time = { false };

private:
	_bool m_Battle_Result_On = { false };
	_bool m_Battle_Result_Off = { false };
	_float m_Battle_Result_TimeAcc = { 0.f };
	_bool m_Battle_Result_Time = { false };
	_bool m_Battle_Result_Size_Reset = { false };

	_bool m_Battle_Result_On_2 = { false };
	_bool m_Battle_Result_Off_2 = { false };
	_float m_Battle_Result_TimeAcc_2 = { 0.f };
	_bool m_Battle_Result_Time_2 = { false };

private: // Cut_In
	_int m_Zen_UI_Num = { 0 };



public:
	virtual void Free() override;
};

END