#include "BoardGamePluginPCH.h"
#include "BehaviorWorldListener.h"

#include <Vision\Runtime\EnginePlugins\Havok\HavokBehaviorEnginePlugin\vHavokBehaviorComponent.hpp>


BG_BehaviorWorldListener::BG_BehaviorWorldListener()
{
}

void BG_BehaviorWorldListener::eventRaisedCallback( hkbCharacter* character, const hkbEvent& behaviorEvent, bool raisedBySdk )
{
	vHavokBehaviorComponent *behaviorComponent = (vHavokBehaviorComponent*)(character->m_userData);
	VASSERT(behaviorComponent);
	if(behaviorComponent)
	{
		Vision::Game.SendMsg(behaviorComponent->GetOwner(), BG_UserMessages::kAnimationEvent, (INT_PTR)&behaviorEvent, raisedBySdk ? 1 : 0);
	}
}

BG_BehaviorWorldListener BG_BehaviorWorldListener::g_behaviorWorldListener;