/**
 * @file striumph.c
 * @date 8-Dec-2025
 */

#include "orx.h"
#include "orxExtensions.h"
#include <stdbool.h>
#include <stdio.h>
#include <agar/core.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#ifdef __orxMSVC__

/* Requesting high performance dedicated GPU on hybrid laptops */
__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#endif // __orxMSVC__

static orxOBJECT *redShipObject = orxNULL;
static orxOBJECT *blueShipObject = orxNULL;
static orxOBJECT *redGunObject = orxNULL;
static orxOBJECT *blueGunObject = orxNULL;
static orxOBJECT *sunGunObject = orxNULL;
static orxSPAWNER *redSpawnerObject = orxNULL;
static orxSPAWNER *blueSpawnerObject = orxNULL;
static orxSPAWNER *sunSpawnerObject = orxNULL;
static orxOBJECT *vMonolithObject = orxNULL;
static orxFLOAT screenWidth = 0.0f;
static orxFLOAT screenHeight = 0.0f;
static orxOBJECT *sunObject = orxNULL;
static orxOBJECT *explosionObject = orxNULL;
static orxOBJECT *textObject = orxNULL;
static orxU32 redFuel = 5000;
static orxGRAPHIC *textGraphic = orxNULL;
static orxSTRUCTURE *textStructure = orxNULL;
static orxTEXT *text = orxNULL;
static orxFONT *textFont = orxNULL;
static orxFLOAT radian_min_val = 0.0f;
static orxFLOAT radian_max_val = orxMATH_KF_PI_BY_2 * 3.0f;
static bool redInertia = false;
static orxFLOAT inertiaPercentage = 1.0f;

/** Update function, it has been registered to be called every tick of the core clock
 */
void orxFASTCALL Update(const orxCLOCK_INFO *_pstClockInfo, void *_pContext)
{
  // Should quit?
  if (orxInput_HasBeenActivated("Quit"))
  {
    // Send close event
    orxEvent_SendShort(orxEVENT_TYPE_SYSTEM, orxSYSTEM_EVENT_CLOSE);
  }

  if (orxInput_IsActive("AlienShoot"))
  {
    orxFLOAT scale = (orxFLOAT)rand() / RAND_MAX;
    orxFLOAT shotRotation = radian_min_val + scale * (radian_max_val - radian_min_val);

    orxObject_SetRotation(sunGunObject, shotRotation);
    orxObject_Enable(sunGunObject, orxTRUE);
  }
  else
  {
    orxObject_Enable(sunGunObject, orxFALSE);
  }

  if (orxInput_IsActive("RotateLeft"))
  {
    orxFLOAT currentRotation = orxObject_GetRotation(redShipObject);
    orxFLOAT rotationChange = (0.08f + _pstClockInfo->fDT) - (_pstClockInfo->fDT / orxMATH_KF_2_PI);
    orxObject_SetRotation(redShipObject, currentRotation - rotationChange);
  }

  if (orxInput_IsActive("RotateRight"))
  {
    orxFLOAT currentRotation = orxObject_GetRotation(redShipObject);
    orxFLOAT rotationChange = (0.08f + _pstClockInfo->fDT) - (_pstClockInfo->fDT / orxMATH_KF_2_PI);
    orxObject_SetRotation(redShipObject, currentRotation + rotationChange);
  }

  // TODO: Variables for shot include rate, speed, longevity, gravity, bounce

  if (orxInput_IsActive("Shoot"))
  {
    orxVECTOR v = orxVECTOR_0;
    orxObject_Enable(redGunObject, orxTRUE);
  }
  else
  {
    orxObject_Enable(redGunObject, orxFALSE);
  }

  if (orxInput_HasBeenDeactivated("Thrust"))
  {
    redInertia = true;
  }

  if (redInertia)
  {
    orxFLOAT currentRotation = orxObject_GetRotation(redShipObject);
    orxFLOAT x = orxMath_Cos(currentRotation);
    orxFLOAT y = orxMath_Sin(currentRotation);
    orxVECTOR currentPosition = orxVECTOR_0;
    orxVECTOR newPosition = orxVECTOR_0;
    orxObject_GetPosition(redShipObject, &currentPosition);
    orxVECTOR components = orxVECTOR_0;
    orxVector_Set(&components, (x * 1.9f) * inertiaPercentage, (y * 1.9f) * inertiaPercentage, 0);
    orxVector_Add(&newPosition, &components, &currentPosition);
    orxObject_SetPosition(redShipObject, &newPosition);
    inertiaPercentage -= 0.02f;
    if (inertiaPercentage <= 0.0f)
    {
      redInertia = false;
      inertiaPercentage = 1.0f;
    }
  }

  if (orxInput_IsActive("Thrust"))
  {
    redFuel--;
    char fuelStr[20];
    sprintf(fuelStr, "%u", redFuel);
    orxText_SetString(text, fuelStr);
    orxFLOAT currentRotation = orxObject_GetRotation(redShipObject);
    orxVECTOR currentPosition = orxVECTOR_0;
    orxObject_GetPosition(redShipObject, &currentPosition);
    orxVECTOR newPosition = orxVECTOR_0;

    orxFLOAT test = (orxFLOAT)screenHeight;

    bool clamp = true;
    bool borderHit = false;

    // Check and wrap horizontally
    if (currentPosition.fX < -screenWidth / 2.0f)
    {
      if (!clamp)
      {
        currentPosition.fX += screenWidth; // Move to the right side
        borderHit = true;
      }
      else
      {
        currentPosition.fX += 2.3f;
      }
    }
    else if (currentPosition.fX > screenWidth / 2.0f)
    {
      if (!clamp)
      {
        currentPosition.fX -= screenWidth; // Move to the left side
        borderHit = true;
      }
      else
      {
        currentPosition.fX -= 2.3f;
      }
    }

    // Check and wrap vertically (adjust for your game's Y-axis orientation)
    if (currentPosition.fY < -screenHeight / 2.0f)
    {
      if (!clamp)
      {
        currentPosition.fY += screenHeight; // Move to the bottom
        borderHit = true;
      }
      else
      {
        currentPosition.fY += 2.3f; // bounce ship off edge
      }
    }
    else if (currentPosition.fY > screenHeight / 2.0f)
    {
      if (!clamp)
      {
        currentPosition.fY -= screenHeight; // Move to the top
        borderHit = true;
      }
      else
      {
        currentPosition.fY -= 2.3f;
      }
    }

    if ((!clamp && borderHit) || !borderHit)
    {
      orxFLOAT x = orxMath_Cos(currentRotation);
      orxFLOAT y = orxMath_Sin(currentRotation);
      orxVECTOR components = orxVECTOR_0;
      orxVector_Set(&components, x * 1.9f, y * 1.9f, 0);

      orxVector_Add(&newPosition, &components, &currentPosition);
      orxObject_SetPosition(redShipObject, &newPosition);
    }
  }
}

orxSTATUS orxFASTCALL AnimationEventHandler(const orxEVENT *_pstEvent)
{
  orxANIM_EVENT_PAYLOAD *pstPayload;

  pstPayload = (orxANIM_EVENT_PAYLOAD *)_pstEvent->pstPayload;
  orxVECTOR newShipPos = orxVECTOR_0;
  newShipPos.fX = -400;
  newShipPos.fY = 0;
  newShipPos.fZ = 0;
  orxVECTOR newShipSpeed = orxVECTOR_0;

  switch (_pstEvent->eID)
  {
  case orxANIM_EVENT_START:
    break;

  case orxANIM_EVENT_STOP:
    break;

  case orxANIM_EVENT_CUT:
    break;

  case orxANIM_EVENT_LOOP:
    // orxLOG("Animation <%s>@<%s> has looped!", pstPayload->zAnimName, orxObject_GetName(orxOBJECT(_pstEvent->hRecipient)));
    if (orxString_Compare(pstPayload->zAnimName, "ExplosionFire") == 0)
    {
      orxObject_Enable(explosionObject, orxFALSE);
      orxObject_SetPosition(blueShipObject, &newShipPos);
      orxObject_SetRelativeSpeed(blueShipObject, &newShipSpeed);
      orxObject_SetAngularVelocity(blueShipObject, 0.0f);
      orxObject_Enable(blueShipObject, orxTRUE);
    }

    break;
  }

  return orxSTATUS_SUCCESS;
}

orxSTATUS orxFASTCALL PhysicsEventHandler(const orxEVENT *_pstEvent)
{

  if (_pstEvent->eID == orxPHYSICS_EVENT_CONTACT_ADD)
  {
    orxOBJECT *pstRecipientObject, *pstSenderObject;
    orxVECTOR vContactPoint;

    pstSenderObject = orxOBJECT(_pstEvent->hSender);
    pstRecipientObject = orxOBJECT(_pstEvent->hRecipient);

    orxSTRING senderObjectName = (orxSTRING)orxObject_GetName(pstSenderObject);
    orxSTRING recipientObjectName = (orxSTRING)orxObject_GetName(pstRecipientObject);

    orxLOG("%s"
           " collided with "
           "%s",
           senderObjectName, recipientObjectName);

    if (orxString_Compare(senderObjectName, "Blue") == 0)
    {
      if (orxString_Compare(recipientObjectName, "Shot") == 0)
      {
        orxObject_SetLifeTime(pstRecipientObject, 0);
        orxObject_GetPosition(pstRecipientObject, &vContactPoint);
        orxObject_SetPosition(explosionObject, &vContactPoint);
        orxObject_Enable(explosionObject, orxTRUE);
        orxObject_Enable(blueShipObject, orxFALSE);
      }
    }
  }
  return orxSTATUS_SUCCESS;
}

/** Camera Update function, it has been registered to be called every tick of the core clock, after physics & objects have been updated
 */
void orxFASTCALL CameraUpdate(const orxCLOCK_INFO *_pstClockInfo, void *_pContext)
{
  // Get MainCamera proxy object
  orxConfig_PushSection("MainCamera");
  orxOBJECT *pstMainCamera = orxOBJECT(orxStructure_Get(orxConfig_GetU64("ID")));
  orxConfig_PopSection();
  if (pstMainCamera)
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
  for (orxS32 i = 0, iCount = orxConfig_GetListCount("ViewportList"); i < iCount; i++)
  {
    orxViewport_CreateFromConfig(orxConfig_GetListString("ViewportList", i));
  }

  // Register the Update & CameraUpdate functions to the core clock
  orxClock_Register(orxClock_Get(orxCLOCK_KZ_CORE), Update, orxNULL, orxMODULE_ID_MAIN, orxCLOCK_PRIORITY_NORMAL);
  orxClock_Register(orxClock_Get(orxCLOCK_KZ_CORE), CameraUpdate, orxNULL, orxMODULE_ID_MAIN, orxCLOCK_PRIORITY_LOWER);

  // Create the scene
  orxObject_CreateFromConfig("Sound");
  redShipObject = orxObject_CreateFromConfig("Red");
  blueShipObject = orxObject_CreateFromConfig("Blue");
  redSpawnerObject = orxSpawner_CreateFromConfig("RedShotSpawner");
  blueSpawnerObject = orxSpawner_CreateFromConfig("BlueShotSpawner");
  redGunObject = (orxOBJECT *)orxObject_GetChild(redShipObject);
  blueGunObject = (orxOBJECT *)orxObject_GetChild(blueShipObject);

  sunObject = orxObject_CreateFromConfig("SunObject");
  sunGunObject = (orxOBJECT *)orxObject_GetChild(sunObject);
  sunSpawnerObject = orxSpawner_CreateFromConfig("AlienShotSpawner");
  orxObject_Enable(sunGunObject, orxFALSE);

  explosionObject = orxObject_CreateFromConfig("ExplosionObject");
  orxObject_Enable(redGunObject, orxFALSE);
  orxObject_Enable(blueGunObject, orxFALSE);
  orxObject_Enable(explosionObject, orxFALSE);
  vMonolithObject = orxObject_CreateFromConfig("VMonolith");
  textObject = orxObject_CreateFromConfig("TextObject");
  textGraphic = orxGRAPHIC(orxOBJECT_GET_STRUCTURE(textObject, GRAPHIC));
  textStructure = orxGraphic_GetData(textGraphic);
  text = orxTEXT(textStructure);
  textFont = orxText_GetFont(text);

  orxDISPLAY_VIDEO_MODE videoMode;
  /* Passing orxU32_UNDEFINED as the index retrieves the current desktop video mode */
  orxDisplay_GetVideoMode(orxU32_UNDEFINED, &videoMode);

  /* The width can then be accessed */
  screenWidth = (orxFLOAT)videoMode.u32Width;
  screenHeight = (orxFLOAT)videoMode.u32Height;

  // Done!
  orxEvent_AddHandler(orxEVENT_TYPE_PHYSICS, PhysicsEventHandler);
  orxEvent_AddHandler(orxEVENT_TYPE_ANIM, AnimationEventHandler);

  orxVECTOR center = orxVECTOR_0;

  orxOBJECT *star = orxObject_CreateFromConfig("Star");
  orxObject_SetPosition(star, &center);

  srand((unsigned int)time(NULL));
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
