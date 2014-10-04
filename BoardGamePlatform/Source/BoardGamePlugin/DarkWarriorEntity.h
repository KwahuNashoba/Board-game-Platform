#ifndef BG_PLUGIN_DARK_WARRIOR_ENTITY_H__
#define BG_PLUGIN_DARK_WARRIOR_ENTITY_H__

#include "WarriorEntity.h"

class BG_DarkWarriorEntity : public BG_WarriorEntity
{
public:
	BG_DarkWarriorEntity();

	VType* GetControllerComponentType() HK_OVERRIDE;

	void InitFunction() HK_OVERRIDE;
	void PostInitialize() HK_OVERRIDE;

	void OnTick(float deltaTime) HKV_OVERRIDE;

protected:
	float m_dyingDuration;

private:
	V_DECLARE_SERIAL_DLLEXP(BG_DarkWarriorEntity, BG_PLUGIN_IMPEXP);
	V_DECLARE_VARTABLE(BG_DarkWarriorEntity, BG_PLUGIN_IMPEXP);
};

#endif