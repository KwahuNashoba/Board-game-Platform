/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

//  Plugin Defines removed from stdafx and placed here.
// 

#ifndef BG_PLUGIN_MODULE_H__
#define BG_PLUGIN_MODULE_H__

namespace BG_UserMessages
{
	enum Enum
	{
		kUserMessageStart = VIS_MSG_USER,

		kAnimationEvent,
	};
}

#ifdef WIN32
  #ifdef BG_PLUGIN_EXPORTS
    #define BG_PLUGIN_IMPEXP __declspec(dllexport)
  #elif BG_PLUGIN_IMPORTS
    #define BG_PLUGIN_IMPEXP __declspec(dllimport)
  #else
    #define BG_PLUGIN_IMPEXP __declspec()
  #endif

#elif defined (_VISION_IOS) || defined(_VISION_ANDROID) || defined(HK_PLATFORM_TIZEN)
  #define BG_PLUGIN_IMPEXP

#else
  #error Undefined platform!
#endif

#endif //  BG_PLUGIN_MODULE_H_INCLUDED

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
