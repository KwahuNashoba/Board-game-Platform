#ifndef BG_BRIGHT_WARRIOR_CONTROLLER_COMPONENT_H__
#define BG_BRIGHT_WARRIOR_CONTROLLER_COMPONENT_H__

#include "ControllerComponent.h"

class BG_DarkWarriorControllerComponent : public BG_ControllerComponent
{
public:
	BG_DarkWarriorControllerComponent();

	void OnTick(float deltaTime) HKV_OVERRIDE;

private:
	void SetOwner(VisTypedEngineObject_cl *newOwner) HKV_OVERRIDE;


	V_DECLARE_SERIAL_DLLEXP(BG_DarkWarriorControllerComponent, BG_PLUGIN_IMPEXP);
	V_DECLARE_VARTABLE(BG_DarkWarriorControllerComponent, BG_PLUGIN_IMPEXP);
};

namespace BG_DarkWarriorControllerState
{
	class Idling : public BG_ControllerStateBase
	{
		void OnEnterState(BG_ControllerComponent *const controller) HKV_OVERRIDE;

		void OnTick(BG_ControllerComponent *const controller, float deltaTime) HKV_OVERRIDE;

		char const *GetName() const HKV_OVERRIDE { return "DarkWarrior::Idling"; }
	};

	class Moving : public BG_ControllerState::Moving
	{
		void OnTick(BG_ControllerComponent *const controller, float deltaTime) HKV_OVERRIDE;

		char const *GetName() const HKV_OVERRIDE { return "DarkWarrior::Moving"; }
	};

	class MeleeAttacking : public BG_ControllerStateBase
	{
		void OnEnterState(BG_ControllerComponent *const controller) HKV_OVERRIDE;

		void OnProcessAnimationEvent(BG_ControllerComponent *const controller, hkbEvent const& animationEvent) HKV_OVERRIDE;

		void OnTick(BG_ControllerComponent *const controller, float deltaTime) HKV_OVERRIDE;

		char const *GetName() const HKV_OVERRIDE { return "BrightWarrior::MeleeAttacking"; }
	};
}


#endif