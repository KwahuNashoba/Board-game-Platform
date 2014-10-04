#ifndef BG_PLUGIN_BEHAVIOR_WORLD_LISTENER
#define BG_PLUGIN_BEHAVIOR_WORLD_LISTENER

#include <Behavior\Behavior\World\hkbWorldListener.h>
#include <Behavior\Behavior\World\hkbWorld.h>

class BG_BehaviorWorldListener : public hkbWorldListener
{
public:
	BG_BehaviorWorldListener();

	virtual void eventRaisedCallback( hkbCharacter* character, const hkbEvent& behaviorEvent, bool raisedBySdk ) HK_OVERRIDE;

public:
	static BG_BehaviorWorldListener g_behaviorWorldListener;
};

#endif

