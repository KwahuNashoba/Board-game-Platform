#ifndef BG_SOUND_HELPER_H__
#define BG_SOUND_HELPER_H__

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodSoundResource.hpp>

class VRefCounter;

namespace BG_SoundEvent
{
	enum Enum
	{
		//footsteps
		eFootstepRight,
		eFootStepLeft,

		//die
		eDie,

		//melle attack
		eMeleeAttack,

		eCount
	};
}

class BG_SoundHelper : public VRefCounter
{
public:
	BG_SoundHelper(bool brightWarrior);

	void PlaySound(BG_SoundEvent::Enum eventId);

	void PlayFromPosition(BG_SoundEvent::Enum eventId, hkvVec3 position);
	
	void DeInitSoundEffects();

private:
	void InitSoundEffects(bool brightWarrior);

private:
	VFmodSoundResource *m_soundResources[BG_SoundEvent::eCount];
};
#endif
