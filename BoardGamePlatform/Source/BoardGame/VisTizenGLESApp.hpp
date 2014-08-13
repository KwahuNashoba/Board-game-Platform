/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VIS_TIZEN_GLES_APP_HPP
#define VIS_TIZEN_GLES_APP_HPP

#include <Vision/Runtime/Base/VBase.hpp>
#include <Vision/Runtime/Framework/VisionApp/VAppBase.hpp>

extern "C" bool VisionInitFunction();
extern "C" bool VisionRunFunction();
extern "C" bool VisionDeInitFunction();

/**
* [VisTizenGLESApp] UiApp must inherit from UiApp class
* which provides basic features necessary to define an UiApp.
* 
* VisTizenGLESApp is only there for legacy reasons. For normal applications consider VAppTizen.
*/
class VisTizenGLESApp 
  : public Tizen::App::UiApp
  , public Tizen::Base::Runtime::ITimerEventListener
  , public Tizen::Ui::IOrientationEventListener
  , public Tizen::Ui::Controls::IFrameEventListener
{
  friend bool VisionInitFunction();
  friend bool VisionRunFunction();
  friend bool VisionDeInitFunction();

public:
  /**
  * [Test] UiApp must have a factory method that creates an instance of itself.
  */
  static Tizen::App::UiApp* CreateInstance(void);

public:

  VisTizenGLESApp(void);

  virtual ~VisTizenGLESApp(void);

public:

  // Called when the app is initializing. (via UiApp)
  virtual bool OnAppInitializing(Tizen::App::AppRegistry& appRegistry) HKV_OVERRIDE;

  // Called when the app initialization is finished. (via UiApp)
  virtual bool OnAppInitialized(void) HKV_OVERRIDE; 

  // Called when the app is requested to terminate. (via UiApp)
  virtual bool OnAppWillTerminate(void) HKV_OVERRIDE; 

  // Called when the app is terminating. (via UiApp)
  virtual bool OnAppTerminating(Tizen::App::AppRegistry& appRegistry, bool forcedTermination = false) HKV_OVERRIDE;

  // Called when the UiApp's frame moves to the top of the screen. (from UiApp)
  virtual void OnForeground(void) HKV_OVERRIDE;

  // Called when this UiApp's frame is moved from top of the screen to the background. (from UiApp)
  virtual void OnBackground(void) HKV_OVERRIDE;

  // Called when the system memory is not sufficient to run the UiApp any further. (via UiApp)
  virtual void OnLowMemory(void) HKV_OVERRIDE;

  // Called when the battery level changes. (via UiApp)
  virtual void OnBatteryLevelChanged(Tizen::System::BatteryLevel batteryLevel) HKV_OVERRIDE;

  // Timer callback (called as frequent as possible). (from ITimerEventListener)
  virtual void OnTimerExpired(Tizen::Base::Runtime::Timer& timer) HKV_OVERRIDE;

  // Called when the device orientation changes. (from IOrientationEventListener)
  virtual void OnOrientationChanged(const Tizen::Ui::Control &source, Tizen::Ui::OrientationStatus orientationStatus) HKV_OVERRIDE;

  // Called when the main application frame gets activated. (from IFrameEventListener)
  virtual void OnFrameActivated(const Tizen::Ui::Controls::Frame &source) HKV_OVERRIDE;

  // Called when the main application frame gets deactivated. (from IFrameEventListener)
  virtual void OnFrameDeactivated(const Tizen::Ui::Controls::Frame &source) HKV_OVERRIDE;

  // Called when the main application frame gets minimized. (from IFrameEventListener)
  virtual void OnFrameMinimized(const Tizen::Ui::Controls::Frame &source) HKV_OVERRIDE;

  // Called when the main application frame gets restored. (from IFrameEventListener)
  virtual void OnFrameRestored(const Tizen::Ui::Controls::Frame &source) HKV_OVERRIDE;

  // Called when the main application frame gets terminated. (from IFrameEventListener)
  virtual void OnFrameTerminating(const Tizen::Ui::Controls::Frame &source) HKV_OVERRIDE;

  // The main draw call - calling the Vision run function.
  void Draw(void);

private:

  Tizen::Base::Runtime::Timer* m_pTimer;
};

#endif // _FRAMEBASEDGLESTIZEN_H_

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
