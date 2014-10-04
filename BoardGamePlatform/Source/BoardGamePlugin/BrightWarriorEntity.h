
#ifndef BG_PLUGIN_BRIGHT_WARRIOR_ENTITY_H__
#define BG_PLUGIN_BRIGHT_WARRIOR_ENTITY_H__

#include "WarriorEntity.h"

class BG_BrightWarriorEntity : public BG_WarriorEntity
{
public:
	BG_BrightWarriorEntity();

	VType* GetControllerComponentType() HK_OVERRIDE;

	void InitFunction() HK_OVERRIDE;
	void PostInitialize() HK_OVERRIDE;

	void OnTick(float deltaTime) HKV_OVERRIDE;

protected:
	float m_dyingDuration;

private:
	V_DECLARE_SERIAL_DLLEXP(BG_BrightWarriorEntity, BG_PLUGIN_IMPEXP);
	V_DECLARE_VARTABLE(BG_BrightWarriorEntity, BG_PLUGIN_IMPEXP);
};

#endif