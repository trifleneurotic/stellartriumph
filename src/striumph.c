/**
 * @file striumph.c
 * @date 8-Dec-2025
 */

#include "orx.h"
#include "orxExtensions.h"

#ifdef __orxMSVC__

/* Requesting high performance dedicated GPU on hybrid laptops */
__declspec(dllexport) unsigned long NvOptimusEnablement        = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#endif // __orxMSVC__

static orxOBJECT* shipObject		= orxNULL;
/** Update function, it has been registered to be called every tick of the core clock
 */
void orxFASTCALL Update(const orxCLOCK_INFO *_pstClockInfo, void *_pContext)
{
  // Should quit?
  if(orxInput_HasBeenActivated("Quit"))
  {
    // Send close event
    orxEvent_SendShort(orxEVENT_TYPE_SYSTEM, orxSYSTEM_EVENT_CLOSE);
  }


  if (orxInput_IsActive("RotateLeft"))
  {
	  orxFLOAT currentRotation = orxObject_GetRotation(shipObject);
	  orxFLOAT rotationChange = (0.08f + _pstClockInfo->fDT) - (_pstClockInfo->fDT / orxMATH_KF_2_PI);
	  orxObject_SetRotation(shipObject, currentRotation - rotationChange);
  }

  if (orxInput_IsActive("RotateRight"))
  {
	  orxFLOAT currentRotation = orxObject_GetRotation(shipObject);
	  orxFLOAT rotationChange = (0.08f + _pstClockInfo->fDT) - (_pstClockInfo->fDT / orxMATH_KF_2_PI);
	  orxObject_SetRotation(shipObject, currentRotation + rotationChange);
  }

  if (orxInput_IsActive("Thrust"))
  {
	  orxFLOAT currentRotation = orxObject_GetRotation(shipObject);
	  orxVECTOR currentPosition = orxVECTOR_0;
	  orxObject_GetPosition(shipObject, &currentPosition);

	  orxFLOAT x = orxMath_Cos(currentRotation);
	  orxFLOAT y = orxMath_Sin(currentRotation);
	  orxVECTOR components = orxVECTOR_0;
	  orxVector_Set(&components, x * 1.9f, y * 1.6f, 0);
	  
	  orxVECTOR newPosition = orxVECTOR_0;
	  orxVector_Add(&newPosition, &components, &currentPosition);

	  orxObject_SetPosition(shipObject, &newPosition);
  }	  

}

/** Camera Update function, it has been registered to be called every tick of the core clock, after physics & objects have been updated
*/
void orxFASTCALL CameraUpdate(const orxCLOCK_INFO *_pstClockInfo, void *_pContext)
{
  // Get MainCamera proxy object
  orxConfig_PushSection("MainCamera");
  orxOBJECT *pstMainCamera = orxOBJECT(orxStructure_Get(orxConfig_GetU64("ID")));
  orxConfig_PopSection();
  if(pstMainCamera)
  {
    // Update & move the camera here
    // [...]

    // Round its position
    orxVECTOR vPosition;
    orxObject_SetPosition(pstMainCamera, orxVector_Round(&vPosition, orxObject_GetPosition(pstMainCamera, &vPosition)));
  }
}

/** Init function, it is called when all orx's modules have been initialized
 */
orxSTATUS orxFASTCALL Init()
{
  // Init extensions
  InitExtensions();

  // Push [Main] as the default config section
  orxConfig_PushSection("Main");

  // Create the viewports
  for(orxS32 i = 0, iCount = orxConfig_GetListCount("ViewportList"); i < iCount; i++)
  {
    orxViewport_CreateFromConfig(orxConfig_GetListString("ViewportList", i));
  }

  // Register the Update & CameraUpdate functions to the core clock
  orxClock_Register(orxClock_Get(orxCLOCK_KZ_CORE), Update, orxNULL, orxMODULE_ID_MAIN, orxCLOCK_PRIORITY_NORMAL);
  orxClock_Register(orxClock_Get(orxCLOCK_KZ_CORE), CameraUpdate, orxNULL, orxMODULE_ID_MAIN, orxCLOCK_PRIORITY_LOWER);

  // Create the scene
  orxObject_CreateFromConfig("Sound");
  shipObject = orxObject_CreateFromConfig("Ship");

  // Done!
  return orxSTATUS_SUCCESS;
}

/** Run function, it should not contain any game logic
 */
orxSTATUS orxFASTCALL Run()
{
  // Return orxSTATUS_FAILURE to instruct orx to quit
  return orxSTATUS_SUCCESS;
}

/** Exit function, it is called before exiting from orx
 */
void orxFASTCALL Exit()
{
  // Exit from extensions
  ExitExtensions();

  // Let orx clean all our mess automatically. :)
}

/** Bootstrap function, it is called before config is initialized, allowing for early resource storage definitions
 */
orxSTATUS orxFASTCALL Bootstrap()
{
  // Bootstrap extensions
  BootstrapExtensions();

  // Return orxSTATUS_FAILURE to prevent orx from loading the default config file
  return orxSTATUS_SUCCESS;
}

/** Main function
 */
int main(int argc, char **argv)
{
  // Set the bootstrap function to provide at least one resource storage before loading any config files
  orxConfig_SetBootstrap(Bootstrap);

  // Execute our game
  orx_Execute(argc, argv, Init, Run, Exit);

  // Done!
  return EXIT_SUCCESS;
}
