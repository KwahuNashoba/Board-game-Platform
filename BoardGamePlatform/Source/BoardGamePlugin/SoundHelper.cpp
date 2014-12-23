#include "BoardGamePluginPCH.h"
#include "SoundHelper.h"

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodManager.hpp>

BG_SoundHelper::BG_SoundHelper(bool brightWarrior)
{
	InitSoundEffects(brightWarrior);
}

void BG_SoundHelper::InitSoundEffects(bool brightWarrior)
{
	if(brightWarrior)
	{
		m_soundResources[BG_SoundEvent::eFootstepRight] = VFmodManager::GlobalManager().LoadSoundResource("Sounds/Footsteps/BW_Foot_right.ogg");
		m_soundResources[BG_SoundEvent::eFootStepLeft] = VFmodManager::GlobalManager().LoadSoundResource("Sounds/Footsteps/BW_Foot_left.ogg");
		m_soundResources[BG_SoundEvent::eDie] = VFmodManager::GlobalManager().LoadSoundResource("Sounds/Characters/BW_die.wav");
		//m_soundResources[BG_SoundEvent::eMeleeAttack] = VFmodManager::GlobalManager().LoadSoundResource("Sounds/Characters/BW_melee_attack.wav");
	}
	else
	{
		m_soundResources[BG_SoundEvent::eFootstepRight] = VFmodManager::GlobalManager().LoadSoundResource("Sounds/Footsteps/DW_Foot_right.wav");
		m_soundResources[BG_SoundEvent::eFootStepLeft] = VFmodManager::GlobalManager().LoadSoundResource("Sounds/Footsteps/DW_Foot_left.wav");
		m_soundResources[BG_SoundEvent::eDie] = VFmodManager::GlobalManager().LoadSoundResource("Sounds/Characters/DW_die.wav");
		//m_soundResources[BG_SoundEvent::eMeleeAttack] = VFmodManager::GlobalManager().LoadSoundResource("Sounds/Characters/DW_melee_attack.wav");
	}
}

void BG_SoundHelper::DeInitSoundEffects()
{
	for(int i = 0; i < BG_SoundEvent::eCount; i++)
	{
		m_soundResources[i] = NULL;
	}
}

void BG_SoundHelper::PlayFromPosition(BG_SoundEvent::Enum eventId, hkvVec3 position)
{
	if(m_soundResources[eventId])
	{
		VFmodSoundObject *source = m_soundResources[eventId]->CreateInstance(position, VFMOD_FLAG_NONE);
		
		source->Play();
	}
}
 
void BG_SoundHelper::PlaySound(BG_SoundEvent::Enum eventId)
{
	//TODO:
}
