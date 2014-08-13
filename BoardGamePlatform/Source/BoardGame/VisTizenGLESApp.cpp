/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/Common/Tizen/VisTizenGLESApp.hpp>
#include <FSecurity.h>

#define printf(...) AppLogTag("Vision", __VA_ARGS__)

#if defined( HK_DEBUG )
  #define DEBUG_VERBOSE
#endif

extern "C" void VisionEnterBackgroundFunction();
extern "C" void VisionEnterForegroundFunction();


class VisTizenGLESForm : public Tizen::Ui::Controls::Form
{
public:
  VisTizenGLESForm( VisTizenGLESApp* pApp) 
    : m_pApp( pApp )
  {
  }

  virtual ~VisTizenGLESForm()
  {
  }

  virtual result OnDraw(void)
  {
    if ( m_pApp != null )
    {
      m_pApp->Draw();
    }
    return E_SUCCESS;
  }

private:
  VisTizenGLESApp* m_pApp;
};



VisTizenGLESApp::VisTizenGLESApp(void) : m_pTimer(NULL)
{
}

VisTizenGLESApp::~VisTizenGLESApp(void)
{
}

Tizen::App::UiApp* VisTizenGLESApp::CreateInstance(void)
{
  // Create the instance through the constructor.
  return new VisTizenGLESApp();
}

bool VisTizenGLESApp::OnAppInitializing(Tizen::App::AppRegistry& appRegistry)
{
  m_pTimer = new Tizen::Base::Runtime::Timer;
  m_pTimer->Construct(*this);

  return true;
}

bool VisTizenGLESApp::OnAppInitialized(void)
{
  using namespace Tizen::Ui;
  using namespace Tizen::Ui::Controls;

  // Create the app's UI frame.
  Frame* pAppFrame = new Frame();
  pAppFrame->Construct();
  pAppFrame->SetOrientation(ORIENTATION_LANDSCAPE);
  AddFrame(*pAppFrame);

  // Create and register our Tizen form with the app frame.
  VisTizenGLESForm* pTizenGLESForm = new VisTizenGLESForm( this );
  pTizenGLESForm->Construct( FORM_STYLE_NORMAL );
  GetAppFrame()->GetFrame()->AddControl( pTizenGLESForm );

  // Set and handle device orientation.
  pTizenGLESForm->SetOrientation( ORIENTATION_LANDSCAPE );
  pTizenGLESForm->AddOrientationEventListener( *this );

  // Register for handling frame events. We use this only for deactivating the automatic switching off of the
  // screen - trying to do that in OnForeground() doesn't work as expected.
  pAppFrame->AddFrameEventListener( *this );

  // Register our Tizen form with Vision (to allow for hooking up input events etc.).
  VVideo::SetUIForm( pTizenGLESForm );

  // Start up Vision.
  if (!VisionInitFunction())
  {
    VisionDeInitFunction();
    return false;
  }

  return true;
}

bool VisTizenGLESApp::OnAppWillTerminate(void)
{
#if defined( DEBUG_VERBOSE )
  hkvLog::Info( "### VisTizenGLESApp::OnAppWillTerminate()" );
#endif

  //Vision::Callbacks.OnEnterBackground.TriggerCallbacks();

  return true;
}


bool VisTizenGLESApp::OnAppTerminating(Tizen::App::AppRegistry& appRegistry, bool forcedTermination)
{
#if defined( DEBUG_VERBOSE )
  hkvLog::Info( "### VisTizenGLESApp::OnAppTerminating()" );
#endif

  if (m_pTimer)
  {
    m_pTimer->Cancel();
    delete m_pTimer;
    m_pTimer = NULL;
  }

  VisionDeInitFunction();

  return true;
}

void VisTizenGLESApp::OnForeground(void)
{
#if defined( DEBUG_VERBOSE )
  hkvLog::Info( "### VisTizenGLESApp::OnForeground()" );
#endif

  // ------------------------------------------------------------------------------------------------------
  // Note: Trying to do this here doesn't work as intended as of SDK 2.2.0b.
  //       We still keep this code around - if this works in a later SDK, we can avoid deriving from IFrameEventListener.
  //// Try to disable automatic screen locking. 
  //// Note: The application requires the http://tizen.org/privilege/power privilege for this.
  //hkvLog::Info( "Trying to set screen on state" );
  //result res = Tizen::System::PowerManager::KeepScreenOnState( true, false );
  //hkvLog::Info( "done..." );
  //if ( res != E_SUCCESS )
  //{
  //  hkvLog::Warning( "Unable to keep screen on - error: '%s'",
  //    ( res == E_PRIVILEGE_DENIED ? "PRIVILEGE DENIED" : "SYSTEM ERROR" ) );
  //}
  //else 
  //{
  //  hkvLog::Info( "SUCCESS - PowerManager::KeepScreenOnState( true, false ) returned E_SUCCESS\n" );
  //}
  // ------------------------------------------------------------------------------------------------------

  static bool bStartingUp = true;
  if ( bStartingUp )
  {
    // The app receives the OnForeground event when starting up - we want to skip resource restoration in this case.
    bStartingUp = false;
  }
  else
  {
    VisionEnterForegroundFunction();
  }

  if (m_pTimer)
    m_pTimer->Start(1);
}

void VisTizenGLESApp::OnBackground(void)
{
#if defined( DEBUG_VERBOSE )
  hkvLog::Info( "### VisTizenGLESApp::OnBackground()" );
#endif

  if (m_pTimer)
    m_pTimer->Cancel();

  VisionEnterBackgroundFunction();
}

void VisTizenGLESApp::OnLowMemory(void)
{
#if defined( DEBUG_VERBOSE )
  hkvLog::Info( "### VisTizenGLESApp::OnLowMemory()" );
#endif
  Vision::Callbacks.OnMemoryWarning.TriggerCallbacks();
}

void VisTizenGLESApp::OnBatteryLevelChanged(Tizen::System::BatteryLevel batteryLevel)
{
#if defined( DEBUG_VERBOSE )
  hkvLog::Info( "### VisTizenGLESApp::OnBatteryLevelChanged()" );
#endif
  
  if ( batteryLevel == Tizen::System::BATTERY_CRITICAL )
  {
    // Consider terminating the application...
  }
}

void VisTizenGLESApp::OnTimerExpired(Tizen::Base::Runtime::Timer& timer)
{
  if ((m_pTimer == null) || (&timer != m_pTimer))
  {
    return;
  }
  m_pTimer->Start(1);

  Draw();
}

void VisTizenGLESApp::OnOrientationChanged(const Tizen::Ui::Control &source, Tizen::Ui::OrientationStatus orientationStatus)
{

  hkvLog::Info( "### Received new orientation: %d", orientationStatus );


  if ( Vision::Video.IsInitialized() )
  {
    Tizen::Ui::Controls::Form* pForm = Vision::Video.GetUIForm();

    if ( pForm )
    {
      int iX = 0, iY = 0;
      int iWidth = 0, iHeight = 0;
      pForm->GetBounds( iX, iY, iWidth, iHeight );

      Vision::Video.ChangeScreenResolution( iWidth, iHeight, 0 );
    }

  }
}

void VisTizenGLESApp::OnFrameActivated(const Tizen::Ui::Controls::Frame &source)
{
#if defined( DEBUG_VERBOSE )
  hkvLog::Info( "### OnFrameActivated()" );
#endif

  // Try to disable automatic screen locking.
  // Note: The application requires the http://tizen.org/privilege/power privilege for this.
  // Note: We do this here (instead of in OnForeground()) in order to make it work when the application starts up.
  //       Trying this in OnForeground() makes it work *after* the device is woken up again, but doesn't prevent
  //       the device to be put to sleep right after the application has been started (as of SDK 2.2.0b).
  if ( Tizen::Security::AccessController::CheckPrivilege( "http://tizen.org/privilege/power" ) == E_SUCCESS )
  {
    result res = Tizen::System::PowerManager::KeepScreenOnState( true, false );
    if ( res != E_SUCCESS )
    {
      hkvLog::Warning( "Unable to keep screen on - error: '%s'",
        ( res == E_PRIVILEGE_DENIED ? "PRIVILEGE DENIED" : "SYSTEM ERROR" ) );
    }
  }
  else
  {
#if defined( DEBUG_VERBOSE )
    hkvLog::Info( "Unable to keep screen on - insufficient privileges\n" );
#endif
  }

}

void VisTizenGLESApp::OnFrameDeactivated(const Tizen::Ui::Controls::Frame &source)
{
#if defined( DEBUG_VERBOSE )
  hkvLog::Info( "### OnFrameDeactivated()" );
#endif
}

void VisTizenGLESApp::OnFrameMinimized(const Tizen::Ui::Controls::Frame &source)
{
#if defined( DEBUG_VERBOSE )
  hkvLog::Info( "### OnFrameMinimized()" );
#endif
}

void VisTizenGLESApp::OnFrameRestored(const Tizen::Ui::Controls::Frame &source)
{
#if defined( DEBUG_VERBOSE )
  hkvLog::Info( "### OnFrameRestored()" );
#endif
}

void VisTizenGLESApp::OnFrameTerminating(const Tizen::Ui::Controls::Frame &source)
{
#if defined( DEBUG_VERBOSE )
  hkvLog::Info( "### OnFrameTerminating()" );
#endif
}

void VisTizenGLESApp::Draw(void)
{
  if ( !VisionRunFunction() )
  {
    Terminate();
  }
}

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
