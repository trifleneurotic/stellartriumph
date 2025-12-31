/**
 * @file striumph.c
 * @date 8-Dec-2025
 */

#include "orx.h"
#include "orxExtensions.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdbool.h>
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
static orxOBJECT *asteroidGunObject = orxNULL;
static orxOBJECT *asteroidShooterObject = orxNULL;
static orxSPAWNER *redSpawnerObject = orxNULL;
static orxSPAWNER *blueSpawnerObject = orxNULL;
static orxSPAWNER *sunSpawnerObject = orxNULL;
static orxSPAWNER *asteroidSpawnerObject = orxNULL;
static orxOBJECT *vMonolithObject = orxNULL;
static orxOBJECT *asteroidObject = orxNULL;
static orxFLOAT screenWidth = 0.0f;
static orxFLOAT screenHeight = 0.0f;
static orxOBJECT *sunObject = orxNULL;
static orxOBJECT *explosionObject = orxNULL;
static orxOBJECT *textObject = orxNULL;
static orxU32 redFuel = 1000;
static orxU32 redShots = 100;
static orxGRAPHIC *textGraphic = orxNULL;
static orxSTRUCTURE *textStructure = orxNULL;
static orxTEXT *text = orxNULL;
static orxFONT *textFont = orxNULL;
static orxFLOAT radian_min_val = 0.0f;
static orxFLOAT radian_max_val = orxMATH_KF_PI_BY_2 * 3.0f;
static bool redInertia = false;
static bool blueHit = false;
static bool menuTestShowing = false;
static bool nameObtained = false;
static bool vMonolith = true;
static orxFLOAT inertiaPercentage = 1.0f;
static orxGRAPHIC *shotCountGraphic = orxNULL;
static orxSTRUCTURE *shotCountStructure = orxNULL;
static orxTEXT *shotCount = orxNULL;
static orxFONT *shotCountFont = orxNULL;
static orxOBJECT *shotCountObject = orxNULL;
static orxOBJECT *sunButton = orxNULL;
static orxOBJECT *startButton = orxNULL;
static orxOBJECT *pressedButton = orxNULL;
static orxOBJECT *menuTestObject = orxNULL;
static orxSOUND *snd = orxNULL;

void orxFASTCALL GameReset()
{
	orxVECTOR redShipStart = orxVECTOR_0;
	redShipStart.fX = -100;
	redShipStart.fY = 0;
	redShipStart.fZ = 0;
	orxObject_SetPosition(redShipObject, &redShipStart);
	orxObject_SetRotation(redShipObject, 0.0f);

	if(vMonolith)
	{
		orxLOG("Enabling monolith!");
		orxObject_Enable(vMonolithObject, orxTRUE);
	}
	else
	{
		orxLOG("Disabling monolith!");
		orxObject_Enable(vMonolithObject, orxFALSE);
	}
	  	  orxLOG("Resetting...");
		  menuTestShowing = false;
		  orxObject_Enable(sunButton, orxFALSE);
		  orxObject_Enable(menuTestObject, orxFALSE);
		  orxObject_Enable(startButton, orxFALSE);
		  orxObject_Enable(pressedButton, orxFALSE);

	orxSound_Play(snd);
}
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

  if (menuTestShowing)
  {
	orxVECTOR vPos;
	orxOBJECT *obj = orxNULL;

	 if (orxInput_HasBeenActivated("MenuTest"))
         {
                  orxLOG("Exiting MenuTest!");
                  menuTestShowing = false;
                  orxObject_Enable(sunButton, orxFALSE);
                  orxObject_Enable(menuTestObject, orxFALSE);
		  orxObject_Enable(startButton, orxFALSE);
		  orxObject_Enable(pressedButton, orxFALSE);
          }
         else
          {

  	if(orxRender_GetWorldPosition(orxMouse_GetPosition(&vPos), orxNULL, &vPos) != orxNULL)
  	{
		if(orxInput_HasBeenActivated("Select"))
		{
    			obj = orxObject_Pick(&vPos, orxSTRINGID_UNDEFINED);
			orxLOG("%s", (orxSTRING)orxObject_GetName(obj));

			if(orxString_Compare(orxObject_GetName(obj),"MenuTestButtonSun") == 0 || orxString_Compare(orxObject_GetName(obj),"MenuTestButtonPressed") == 0) {
			if(!vMonolith) {
  			orxObject_Enable(sunButton, orxFALSE);
  			orxObject_Enable(pressedButton, orxTRUE);
			} else {
  			orxObject_Enable(sunButton, orxTRUE);
  			orxObject_Enable(pressedButton, orxFALSE);
			}
			vMonolith = !vMonolith;
			} else if ((orxString_Compare(orxObject_GetName(obj),"MenuTestButtonStart") == 0)) {
				GameReset();
				}
		}
  	}
  }}
  else {
  if (orxInput_IsActive("AsteroidShoot"))
  {
    orxLOG("Asteroid shooter activated.");
    orxVECTOR asteroidShooterPosition = orxVECTOR_0;
    asteroidShooterPosition.fX = 100;
    asteroidShooterPosition.fY = 100;
    asteroidShooterPosition.fZ = 0;
    orxObject_SetPosition(asteroidShooterObject, &asteroidShooterPosition);
    orxFLOAT scale = (orxFLOAT)rand() / RAND_MAX;
    orxFLOAT asteroidGunRotation = radian_min_val + scale * (radian_max_val - radian_min_val);

    orxObject_SetRotation(asteroidGunObject, asteroidGunRotation);
    orxObject_Enable(asteroidGunObject, orxTRUE);
  }
  else
  {
    orxObject_Enable(asteroidGunObject, orxFALSE);
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

  if (orxInput_HasBeenActivated("MenuTest"))
  {
	  if (!menuTestShowing)
	  {
	  	  orxLOG("Entered MenuTest!");
		  menuTestShowing = true;
		  orxObject_Enable(sunButton, orxTRUE);
		  orxObject_Enable(menuTestObject, orxTRUE);
		  orxObject_Enable(startButton, orxTRUE);
	  }
	  else
	  {
	  	  orxLOG("Exiting MenuTest!");
		  menuTestShowing = false;
		  orxObject_Enable(sunButton, orxFALSE);
		  orxObject_Enable(menuTestObject, orxFALSE);
		  orxObject_Enable(startButton, orxFALSE);
	  }
  }

  if (orxInput_IsActive("Shoot"))
  {
    orxObject_Enable(redGunObject, orxTRUE);
  }
  else
  {
    orxObject_Enable(redGunObject, orxFALSE);
  }

  if (orxInput_HasBeenDeactivated("Shoot") && redShots > 0)
  {
    redShots--;
    char shotStr[20];
    sprintf(shotStr, "%u", redShots);
    orxText_SetString(shotCount, shotStr);
  }

  if (orxInput_HasBeenDeactivated("Thrust") && redFuel > 0)
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

  if (orxInput_IsActive("Thrust") && redFuel > 0)
  {
    redFuel--;
    char fuelStr[20];
    sprintf(fuelStr, "%u", redFuel);
    orxText_SetString(text, fuelStr);
    orxFLOAT currentRotation = orxObject_GetRotation(redShipObject);
    orxVECTOR currentPosition = orxVECTOR_0;
    orxObject_GetPosition(redShipObject, &currentPosition);
    orxVECTOR newPosition = orxVECTOR_0;

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
    if (orxString_Compare(pstPayload->zAnimName, "ExplosionFire") == 0)
    {
      orxObject_Enable(explosionObject, orxFALSE);
      if (blueHit)
      {
      orxObject_SetPosition(blueShipObject, &newShipPos);
      orxObject_SetRelativeSpeed(blueShipObject, &newShipSpeed);
      orxObject_SetAngularVelocity(blueShipObject, 0.0f);
      orxObject_Enable(blueShipObject, orxTRUE);
      blueHit = false;
      }
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

    if (orxString_Compare(senderObjectName, "Asteroid") == 0)
    {
      if (orxString_Compare(recipientObjectName, "Shot") == 0)
      {
        orxObject_SetLifeTime(pstRecipientObject, 0);
        orxObject_GetPosition(pstRecipientObject, &vContactPoint);
        orxObject_SetPosition(explosionObject, &vContactPoint);
        orxObject_Enable(explosionObject, orxTRUE);
        orxObject_Enable(pstSenderObject, orxFALSE);
      }
    }

    if (orxString_Compare(senderObjectName, "Blue") == 0)
    {
      if (orxString_Compare(recipientObjectName, "Shot") == 0)
      {
        orxObject_SetLifeTime(pstRecipientObject, 0);
        orxObject_GetPosition(pstRecipientObject, &vContactPoint);
        orxObject_SetPosition(explosionObject, &vContactPoint);
        orxObject_Enable(explosionObject, orxTRUE);
        orxObject_Enable(blueShipObject, orxFALSE);
	blueHit = true;
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
  snd = orxSound_CreateFromConfig("AppearSound");
  redShipObject = orxObject_CreateFromConfig("Red");
  blueShipObject = orxObject_CreateFromConfig("Blue");
  asteroidShooterObject = orxObject_CreateFromConfig("AsteroidShooter");
  redSpawnerObject = orxSpawner_CreateFromConfig("RedShotSpawner");
  blueSpawnerObject = orxSpawner_CreateFromConfig("BlueShotSpawner");
  asteroidSpawnerObject = orxSpawner_CreateFromConfig("AsteroidSpawner");
  redGunObject = (orxOBJECT *)orxObject_GetChild(redShipObject);
  blueGunObject = (orxOBJECT *)orxObject_GetChild(blueShipObject);
  asteroidGunObject = (orxOBJECT *)orxObject_GetChild(asteroidShooterObject);
  orxObject_Enable(asteroidGunObject, orxFALSE);

  
  menuTestObject = orxObject_CreateFromConfig("MenuTest");
  orxObject_Enable(menuTestObject, orxFALSE);
  sunButton = orxObject_CreateFromConfig("MenuTestButtonSun");
  orxObject_Enable(sunButton, orxFALSE);
  pressedButton = orxObject_CreateFromConfig("MenuTestButtonPressed");
  orxObject_Enable(pressedButton, orxFALSE);
  startButton = orxObject_CreateFromConfig("MenuTestButtonStart");
  orxObject_Enable(startButton, orxFALSE);


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

  shotCountObject = orxObject_CreateFromConfig("ShotCountObject");
  shotCountGraphic = orxGRAPHIC(orxOBJECT_GET_STRUCTURE(shotCountObject, GRAPHIC));
  shotCountStructure = orxGraphic_GetData(shotCountGraphic);
  shotCount = orxTEXT(shotCountStructure);
  shotCountFont = orxText_GetFont(shotCount);

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
  orxSound_Play(snd);
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
