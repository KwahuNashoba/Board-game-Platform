/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if !defined(BOARDGAMEPLUGINPCH_H_INCLUDED)
#define BOARDGAMEPLUGINPCH_H_INCLUDED

#if defined(WIN32)
  #define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#endif

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include "BoardGamePluginModule.h"

#include <Vision\Runtime\EnginePlugins\Havok\HavokPhysicsEnginePlugin\vHavokConversionUtils.hpp>

//Havok Ai
#include <Vision\Runtime\EnginePlugins\Havok\HavokAiEnginePlugin\vHavokAiModule.hpp>
#include <Ai\Pathfinding\World\hkaiWorld.h>
#include <Ai\Pathfinding\Character\hkaiCharacter.h>
#include <Ai\Pathfinding\Character\Behavior\hkaiPathFollowingBehavior.h>
#include <Ai\Pathfinding\Character\LocalSteering\hkaiLocalSteeringFilter.h>
#include <Ai\Physics2012Bridge\LocalSteering\hkaiPhysics2012BodyObstacleGenerator.h>

//Havok Behavor
#include <Vision\Runtime\EnginePlugins\Havok\HavokBehaviorEnginePlugin\vHavokBehaviorModule.hpp>
#include <Behavior\Behavior\Character\hkbCharacter.h>

//Havok Physics
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>

extern VModule g_BoardGamePluginModule;

#endif

/*
 * Havok SDK - Base file, BUILD(#20140328)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2014
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
