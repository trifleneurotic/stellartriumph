/**
 * @file striumph.c
 * @date 8-Dec-2025
 */

#include "orx.h"
#include "orxExtensions.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#ifdef __orxMSVC__

/* Requesting high performance dedicated GPU on hybrid laptops */
__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#endif // __orxMSVC__

static orxFLOAT screenWidth = 0.0f;
static orxFLOAT screenHeight = 0.0f;

static orxFLOAT radian_min_val = 0.0f;
static orxFLOAT radian_max_val = orxMATH_KF_PI_BY_2 * 3.0f;

static bool redInertia = false;
static bool blueInertia = false;
static bool blueHit = false;
static bool redHit = false;
static bool menuTestShowing = false;
static bool nameObtained = false;
static bool vMonolith = true;
static bool alienEnabled = false;

static orxFLOAT inertiaPercentage = 1.0f;

static orxOBJECT *redShipObject = orxNULL;
static orxOBJECT *blueShipObject = orxNULL;
static orxOBJECT *alienObject = orxNULL;

static orxOBJECT *redGunObject = orxNULL;
static orxOBJECT *blueGunObject = orxNULL;
static orxOBJECT *alienGunObject = orxNULL;

static orxOBJECT *asteroidGunObject = orxNULL;
static orxOBJECT *asteroidShooterObject = orxNULL;

static orxOBJECT *alienShipGunObject = orxNULL;
static orxOBJECT *alienShipShooterObject = orxNULL;

static orxSPAWNER *redSpawnerObject = orxNULL;
static orxSPAWNER *blueSpawnerObject = orxNULL;
static orxSPAWNER *alienSpawnerObject = orxNULL;
static orxSPAWNER *alienShipSpawnerObject = orxNULL;
static orxSPAWNER *asteroidSpawnerObject = orxNULL;

static orxOBJECT *vMonolithObject = orxNULL;
static orxOBJECT *asteroidObject = orxNULL;

static orxOBJECT *sunObject = orxNULL;
static orxOBJECT *explosionObject = orxNULL;
static orxOBJECT *redFuelCountObject = orxNULL;
static orxOBJECT *blueFuelCountObject = orxNULL;

static orxU32 redFuel = 1000;
static orxGRAPHIC *redFuelCountGraphic = orxNULL;
static orxSTRUCTURE *redFuelCountStructure = orxNULL;
static orxTEXT *redFuelCount = orxNULL;
static orxFONT *redFuelCountFont = orxNULL;

static orxU32 blueFuel = 1000;
static orxGRAPHIC *blueFuelCountGraphic = orxNULL;
static orxSTRUCTURE *blueFuelCountStructure = orxNULL;
static orxTEXT *blueFuelCount = orxNULL;
static orxFONT *blueFuelCountFont = orxNULL;

static orxU32 redShots = 100;
static orxGRAPHIC *redShotCountGraphic = orxNULL;
static orxSTRUCTURE *redShotCountStructure = orxNULL;
static orxTEXT *redShotCount = orxNULL;
static orxFONT *redShotCountFont = orxNULL;
static orxOBJECT *redShotCountObject = orxNULL;

static orxU32 blueShots = 100;
static orxGRAPHIC *blueShotCountGraphic = orxNULL;
static orxSTRUCTURE *blueShotCountStructure = orxNULL;
static orxTEXT *blueShotCount = orxNULL;
static orxFONT *blueShotCountFont = orxNULL;
static orxOBJECT *blueShotCountObject = orxNULL;

static orxOBJECT *vMonolithButton = orxNULL;
static orxOBJECT *startButton = orxNULL;
static orxOBJECT *pressedButton = orxNULL;
static orxOBJECT *menuTestObject = orxNULL;
static orxSOUND *snd = orxNULL;

static orxU32 screenWidthInt = 0;
static orxU32 screenHeightInt = 0;

void orxFASTCALL GameReset()
{
  orxVECTOR redShipStart = orxVECTOR_0;
  orxVECTOR redNewShipSpeed = orxVECTOR_0;
  redShipStart.fX = -100;
  redShipStart.fY = 0;
  redShipStart.fZ = 0;
  orxObject_SetPosition(redShipObject, &redShipStart);
  orxObject_SetRotation(redShipObject, 0.0f);
  orxObject_SetAngularVelocity(redShipObject, 0.0f);
  orxObject_SetRelativeSpeed(redShipObject, &redNewShipSpeed);

  orxVECTOR blueShipStart = orxVECTOR_0;
  orxVECTOR blueNewShipSpeed = orxVECTOR_0;
  blueShipStart.fX = -300;
  blueShipStart.fY = 0;
  blueShipStart.fZ = 0;
  orxObject_SetPosition(blueShipObject, &blueShipStart);
  orxObject_SetRotation(blueShipObject, 0.0f);
  orxObject_SetAngularVelocity(blueShipObject, 0.0f);
  orxObject_SetRelativeSpeed(blueShipObject, &blueNewShipSpeed);

  if (vMonolith)
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
  orxObject_Enable(vMonolithButton, orxFALSE);
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
      orxObject_Enable(vMonolithButton, orxFALSE);
      orxObject_Enable(menuTestObject, orxFALSE);
      orxObject_Enable(startButton, orxFALSE);
      orxObject_Enable(pressedButton, orxFALSE);
    }
    else
    {
      if (orxRender_GetWorldPosition(orxMouse_GetPosition(&vPos), orxNULL, &vPos) != orxNULL)
      {
        if (orxInput_HasBeenActivated("Select"))
        {
          obj = orxObject_Pick(&vPos, orxSTRINGID_UNDEFINED);
          orxLOG("%s", (orxSTRING)orxObject_GetName(obj));

          if (orxString_Compare(orxObject_GetName(obj), "MenuTestButtonVMonolith") == 0 || orxString_Compare(orxObject_GetName(obj), "MenuTestButtonPressed") == 0)
          {
            if (!vMonolith)
            {
              orxObject_Enable(vMonolithButton, orxFALSE);
              orxObject_Enable(pressedButton, orxTRUE);
            }
            else
            {
              orxObject_Enable(vMonolithButton, orxTRUE);
              orxObject_Enable(pressedButton, orxFALSE);
            }
            vMonolith = !vMonolith;
          }
          else if ((orxString_Compare(orxObject_GetName(obj), "MenuTestButtonStart") == 0))
          {
            GameReset();
          }
        }
      }
    }
  }
  else
  {
    if (alienEnabled)
    {
      orxVECTOR alienPosition = orxVECTOR_0;
      orxObject_GetPosition(alienObject, &alienPosition);

      if(alienGunObject == orxNULL)
      {
        alienGunObject = orxObject_CreateFromConfig("AlienGun");
      }

      if (alienPosition.fX < -screenWidth / 2.0f - 50.0f)
      {
        orxObject_Enable(alienObject, orxFALSE);
        alienEnabled = false;

        orxObject_Enable(alienGunObject, orxFALSE);
        alienGunObject = orxNULL;
        orxLOG("Alien left the screen. Disabling alien and alien gun.");
      }

      orxFLOAT alienShotChance = fabs(_pstClockInfo->fTime - nearbyintf(_pstClockInfo->fTime));

      if (alienShotChance > 0.4f)
      {
        orxLOG("Alien shooter activated.");
        orxFLOAT scale = (orxFLOAT)rand() / RAND_MAX;
        orxFLOAT shotRotation = radian_min_val + scale * (radian_max_val - radian_min_val);

        orxObject_SetPosition(alienGunObject, &alienPosition);
        orxObject_SetRotation(alienGunObject, shotRotation);
        orxObject_Enable(alienGunObject, orxTRUE);
      }
      else
      {
        if(alienGunObject != orxNULL)
        {
          orxObject_Enable(alienGunObject, orxFALSE);
        }
      }

    }
    else
    {
      if( alienGunObject != orxNULL)
      orxObject_Enable(alienGunObject, orxFALSE);
    }

    if (orxInput_IsActive("RedRotateLeft"))
    {
      orxFLOAT currentRotation = orxObject_GetRotation(redShipObject);
      orxFLOAT rotationChange = (0.08f + _pstClockInfo->fDT) - (_pstClockInfo->fDT / orxMATH_KF_2_PI);
      orxObject_SetRotation(redShipObject, currentRotation - rotationChange);
      orxLOG("Red Rotate Left active. Current rotation: %f", orxObject_GetRotation(redShipObject));
    }

    if (orxInput_IsActive("RedRotateRight"))
    {
      orxFLOAT currentRotation = orxObject_GetRotation(redShipObject);
      orxFLOAT rotationChange = (0.08f + _pstClockInfo->fDT) - (_pstClockInfo->fDT / orxMATH_KF_2_PI);
      orxObject_SetRotation(redShipObject, currentRotation + rotationChange);
      orxLOG("Red Rotate Right active. Current rotation: %f", orxObject_GetRotation(redShipObject));
    }

    if (orxInput_IsActive("BlueRotateLeft"))
    {
      orxFLOAT currentRotation = orxObject_GetRotation(blueShipObject);
      orxFLOAT rotationChange = (0.08f + _pstClockInfo->fDT) - (_pstClockInfo->fDT / orxMATH_KF_2_PI);
      orxObject_SetRotation(blueShipObject, currentRotation - rotationChange);
      orxLOG("Blue Rotate Left active. Current rotation: %f", orxObject_GetRotation(blueShipObject));
    }

    if (orxInput_IsActive("BlueRotateRight"))
    {
      orxFLOAT currentRotation = orxObject_GetRotation(blueShipObject);
      orxFLOAT rotationChange = (0.08f + _pstClockInfo->fDT) - (_pstClockInfo->fDT / orxMATH_KF_2_PI);
      orxObject_SetRotation(blueShipObject, currentRotation + rotationChange);
      orxLOG("Blue Rotate Right active. Current rotation: %f", orxObject_GetRotation(blueShipObject));
    }

    // TODO: Variables for shot include rate, speed, longevity, gravity, bounce

    if (orxInput_HasBeenActivated("MenuTest"))
    {
      if (!menuTestShowing)
      {
        orxLOG("Entered MenuTest!");
        menuTestShowing = true;
        if (vMonolith)
        {
          orxObject_Enable(pressedButton, orxTRUE);
        }
        else
        {
          orxObject_Enable(vMonolithButton, orxTRUE);
        }
        orxObject_Enable(vMonolithButton, orxTRUE);
        orxObject_Enable(menuTestObject, orxTRUE);
        orxObject_Enable(startButton, orxTRUE);
      }
      else
      {
        orxLOG("Exiting MenuTest!");
        menuTestShowing = false;
        orxObject_Enable(vMonolithButton, orxFALSE);
        orxObject_Enable(menuTestObject, orxFALSE);
        orxObject_Enable(startButton, orxFALSE);
        orxObject_Enable(pressedButton, orxFALSE);
      }
    }

    if (orxInput_IsActive("RedShoot") && redShots > 0)
    {
      orxObject_Enable(redGunObject, orxTRUE);
    }
    else
    {
      orxObject_Enable(redGunObject, orxFALSE);
    }

    if (orxInput_IsActive("BlueShoot") && blueShots > 0)
    {
      orxObject_Enable(blueGunObject, orxTRUE);
    }
    else
    {
      orxObject_Enable(blueGunObject, orxFALSE);
    }

    if (orxInput_HasBeenDeactivated("RedShoot") && redShots > 0)
    {
      redShots--;
      char shotStr[20];
      sprintf(shotStr, "%u", redShots);
      orxText_SetString(redShotCount, shotStr);
    }

    if (orxInput_HasBeenDeactivated("BlueShoot") && blueShots > 0)
    {
      blueShots--;
      char shotStr[20];
      sprintf(shotStr, "%u", blueShots);
      orxText_SetString(blueShotCount, shotStr);
    }

    if (orxInput_HasBeenDeactivated("RedThrust") && redFuel > 0)
    {
      redInertia = true;
    }

    if (orxInput_HasBeenDeactivated("BlueThrust") && blueFuel > 0)
    {
      blueInertia = true;
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

    if (blueInertia)
    {
      orxFLOAT currentRotation = orxObject_GetRotation(blueShipObject);
      orxFLOAT x = orxMath_Cos(currentRotation);
      orxFLOAT y = orxMath_Sin(currentRotation);
      orxVECTOR currentPosition = orxVECTOR_0;
      orxVECTOR newPosition = orxVECTOR_0;
      orxObject_GetPosition(blueShipObject, &currentPosition);
      orxVECTOR components = orxVECTOR_0;
      orxVector_Set(&components, (x * 1.9f) * inertiaPercentage, (y * 1.9f) * inertiaPercentage, 0);
      orxVector_Add(&newPosition, &components, &currentPosition);
      orxObject_SetPosition(blueShipObject, &newPosition);
      inertiaPercentage -= 0.02f;
      if (inertiaPercentage <= 0.0f)
      {
        blueInertia = false;
        inertiaPercentage = 1.0f;
      }
    }

    if (orxInput_IsActive("RedThrust") && redFuel > 0)
    {
      redFuel--;
      char fuelStr[20];
      sprintf(fuelStr, "%u", redFuel);
      orxText_SetString(redFuelCount, fuelStr);
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

        orxLOG("Red thrust active. Current rotation: %f, components: %f, %f", currentRotation, components.fX, components.fY);

        orxVector_Add(&newPosition, &components, &currentPosition);
        orxObject_SetPosition(redShipObject, &newPosition);
      }
    }

    if (orxInput_IsActive("BlueThrust") && blueFuel > 0)
    {
      blueFuel--;
      char fuelStr[20];
      sprintf(fuelStr, "%u", blueFuel);
      orxText_SetString(blueFuelCount, fuelStr);
      orxFLOAT blueCurrentRotation = orxObject_GetRotation(blueShipObject);
      orxVECTOR blueCurrentPosition = orxVECTOR_0;
      orxObject_GetPosition(blueShipObject, &blueCurrentPosition);
      orxVECTOR blueNewPosition = orxVECTOR_0;

      bool clamp = true;
      bool borderHit = false;

      // Check and wrap horizontally
      if (blueCurrentPosition.fX < -screenWidth / 2.0f)
      {
        if (!clamp)
        {
          blueCurrentPosition.fX += screenWidth; // Move to the right side
          borderHit = true;
        }
        else
        {
          blueCurrentPosition.fX += 2.3f;
        }
      }
      else if (blueCurrentPosition.fX > screenWidth / 2.0f)
      {
        if (!clamp)
        {
          blueCurrentPosition.fX -= screenWidth; // Move to the left side
          borderHit = true;
        }
        else
        {
          blueCurrentPosition.fX -= 2.3f;
        }
      }

      // Check and wrap vertically (adjust for your game's Y-axis orientation)
      if (blueCurrentPosition.fY < -screenHeight / 2.0f)
      {
        if (!clamp)
        {
          blueCurrentPosition.fY += screenHeight; // Move to the bottom
          borderHit = true;
        }
        else
        {
          blueCurrentPosition.fY += 2.3f; // bounce ship off edge
        }
      }
      else if (blueCurrentPosition.fY > screenHeight / 2.0f)
      {
        if (!clamp)
        {
          blueCurrentPosition.fY -= screenHeight; // Move to the top
          borderHit = true;
        }
        else
        {
          blueCurrentPosition.fY -= 2.3f;
        }
      }

      if ((!clamp && borderHit) || !borderHit)
      {
        orxFLOAT x = orxMath_Cos(blueCurrentRotation);
        orxFLOAT y = orxMath_Sin(blueCurrentRotation);
        orxVECTOR components = orxVECTOR_0;
        orxVector_Set(&components, x * 1.9f, y * 1.9f, 0);

          orxLOG("Blue thrust active. Current rotation: %f, components: %f, %f", blueCurrentRotation, components.fX, components.fY);

        orxVector_Add(&blueNewPosition, &components, &blueCurrentPosition);
        orxObject_SetPosition(blueShipObject, &blueNewPosition);
      }
    }
  }
}

orxSTATUS orxFASTCALL SpawnerEventHandler(const orxEVENT *_pstEvent)
{
  if (_pstEvent->eID == orxSPAWNER_EVENT_SPAWN)
  {
    orxSPAWNER *pstSpawner;
    orxOBJECT *pstSpawnedObject;
    pstSpawner = (orxSPAWNER *)_pstEvent->hSender;
    pstSpawnedObject = (orxOBJECT *)_pstEvent->hRecipient;
    orxSTRING senderObjectName = (orxSTRING)orxObject_GetName(pstSpawnedObject);
    orxLOG("%s", senderObjectName);
    if (orxString_Compare(senderObjectName, "Alien") == 0)
    {
      orxLOG("Alien spawned.");
      // unsigned int yPosition = rand() % (unsigned int)(screenHeight) - (unsigned int)(screenHeight / 2.0f);
      orxVECTOR alienPosition = orxVECTOR_0;
      alienPosition.fX = screenWidth / 2.0f + 50.0f;
      alienPosition.fY = 100.0f;
      alienPosition.fZ = 0;
      orxVECTOR alienSpeed = orxVECTOR_0;
      alienSpeed.fX = -200.0f;
      alienSpeed.fY = 0.0f;
      alienSpeed.fZ = 0.0f;
      orxObject_SetRelativeSpeed(pstSpawnedObject, &alienSpeed);
      orxObject_SetPosition(pstSpawnedObject, &alienPosition);
      orxObject_Enable(pstSpawnedObject, orxTRUE);
      alienObject = pstSpawnedObject;
      alienEnabled = true;
    }
    if (orxString_Compare(senderObjectName, "Asteroid") == 0)
    {
      orxFLOAT scale = (orxFLOAT)rand() / RAND_MAX;
      orxFLOAT asteroidRotation = radian_min_val + scale * (radian_max_val - radian_min_val);
      orxObject_SetRotation(pstSpawnedObject, asteroidRotation);

      orxVECTOR currentPosition = orxVECTOR_0;
      orxObject_GetPosition(asteroidShooterObject, &currentPosition);
      orxVECTOR newPosition = orxVECTOR_0;

      if(currentPosition.fX < 0 && currentPosition.fY > 0)
      {
         newPosition.fX = currentPosition.fX - 50.0f;
         newPosition.fY = -currentPosition.fY - 50.0f;
         orxLOG("Spawning asteroid in bottom left");
         orxLOG("Current position: %f, %f", currentPosition.fX, currentPosition.fY);
         orxLOG("New position: %f, %f", newPosition.fX, newPosition.fY);
      }
      else if(currentPosition.fX < 0 && currentPosition.fY < 0)
      {
         newPosition.fX = -currentPosition.fX - 50.0f;
         newPosition.fY = currentPosition.fY - 50.0f;
         orxLOG("Spawning asteroid in top left");
         orxLOG("Current position: %f, %f", currentPosition.fX, currentPosition.fY);
         orxLOG("New position: %f, %f", newPosition.fX, newPosition.fY);
      }
      else if(currentPosition.fX > 0 && currentPosition.fY < 0)
      {
         newPosition.fX = currentPosition.fX - 50.0f;
         newPosition.fY = -currentPosition.fY - 50.0f;
         orxLOG("Spawning asteroid in top right");
         orxLOG("Current position: %f, %f", currentPosition.fX, currentPosition.fY);
         orxLOG("New position: %f, %f", newPosition.fX, newPosition.fY);
      }
      else
      {
         newPosition.fX = -currentPosition.fX - 50.0f;
         newPosition.fY = currentPosition.fY - 50.0f;
         orxLOG("Spawning asteroid in bottom right");
         orxLOG("Current position: %f, %f", currentPosition.fX, currentPosition.fY);
         orxLOG("New position: %f, %f", newPosition.fX, newPosition.fY);
      }

      orxObject_SetPosition(asteroidShooterObject, &newPosition);
      scale = (orxFLOAT)rand() / RAND_MAX;
      orxFLOAT asteroidGunRotation = radian_min_val + scale * (radian_max_val - radian_min_val);
      orxObject_SetRotation(asteroidGunObject, asteroidGunRotation);

     }
  return orxSTATUS_SUCCESS;
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

      orxObject_Enable(explosionObject, orxFALSE);
      if (redHit)
      {
        orxObject_SetPosition(redShipObject, &newShipPos);
        orxObject_SetRelativeSpeed(redShipObject, &newShipSpeed);
        orxObject_SetAngularVelocity(redShipObject, 0.0f);
        orxObject_Enable(redShipObject, orxTRUE);
        redHit = false;
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

    if (orxString_Compare(senderObjectName, "Blue") == 0 && !orxInput_IsActive("BlueShoot"))
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

    if (orxString_Compare(senderObjectName, "Red") == 0 && !orxInput_IsActive("RedShoot"))
    {
      if (orxString_Compare(recipientObjectName, "Shot") == 0)
      {
        orxObject_SetLifeTime(pstRecipientObject, 0);
        orxObject_GetPosition(pstRecipientObject, &vContactPoint);
        orxObject_SetPosition(explosionObject, &vContactPoint);
        orxObject_Enable(explosionObject, orxTRUE);
        orxObject_Enable(redShipObject, orxFALSE);
        redHit = true;
      }
    }

     if (orxString_Compare(senderObjectName, "Red") == 0)
    {
      if (orxString_Compare(recipientObjectName, "Asteroid") == 0)
      {
        orxObject_SetLifeTime(pstRecipientObject, 0);
        orxObject_GetPosition(pstRecipientObject, &vContactPoint);
        orxObject_SetPosition(explosionObject, &vContactPoint);
        orxObject_Enable(explosionObject, orxTRUE);
        orxObject_Enable(redShipObject, orxFALSE);
        redHit = true;
      }
    }

     if (orxString_Compare(senderObjectName, "Blue") == 0)
    {
      if (orxString_Compare(recipientObjectName, "Asteroid") == 0)
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
    orxVECTOR vPosition;
    orxObject_SetPosition(pstMainCamera, orxVector_Round(&vPosition, orxObject_GetPosition(pstMainCamera, &vPosition)));
  }
}

void orxFASTCALL InputCheck(const orxCLOCK_INFO *_pstClockInfo, void *_pstContext)
{
	orxINPUT_TYPE inputType;
	orxENUM buttonAxisOrKeyID;
	orxFLOAT value;
	orxINPUT_MODE mode = orxINPUT_MODE_FULL;

	if (orxInput_GetActiveBinding(&inputType, &buttonAxisOrKeyID, &value))
	{
		const orxSTRING pressedInputName = orxInput_GetBindingName(inputType, buttonAxisOrKeyID, mode);
		//orxLOG("Input: %s, Type: %d, ID: %d, Value %f", pressedInputName, inputType, buttonAxisOrKeyID, value);
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

  orxClock_Register(orxClock_Get(orxCLOCK_KZ_CORE), InputCheck, orxNULL, orxMODULE_ID_MAIN, orxCLOCK_PRIORITY_NORMAL);

  // Create the scene
  snd = orxSound_CreateFromConfig("AppearSound");

  redShipObject = orxObject_CreateFromConfig("Red");
  blueShipObject = orxObject_CreateFromConfig("Blue");
  asteroidShooterObject = orxObject_CreateFromConfig("AsteroidShooter");
  alienShipShooterObject = orxObject_CreateFromConfig("AlienShipShooter");

  redSpawnerObject = orxSpawner_CreateFromConfig("RedShotSpawner");
  blueSpawnerObject = orxSpawner_CreateFromConfig("BlueShotSpawner");
  asteroidSpawnerObject = orxSpawner_CreateFromConfig("AsteroidSpawner");
  alienSpawnerObject = orxSpawner_CreateFromConfig("AlienShotSpawner");
  alienShipSpawnerObject = orxSpawner_CreateFromConfig("AlienShipSpawner");

  redGunObject = (orxOBJECT *)orxObject_GetChild(redShipObject);
  blueGunObject = (orxOBJECT *)orxObject_GetChild(blueShipObject);
  asteroidGunObject = (orxOBJECT *)orxObject_GetChild(asteroidShooterObject);
  alienShipGunObject = (orxOBJECT *)orxObject_GetChild(alienShipShooterObject);

  orxDISPLAY_VIDEO_MODE videoMode;
  /* Passing orxU32_UNDEFINED as the index retrieves the current desktop video mode */
  orxDisplay_GetVideoMode(orxU32_UNDEFINED, &videoMode);

  /* The width can then be accessed */
  screenWidth = (orxFLOAT)videoMode.u32Width;
  screenHeight = (orxFLOAT)videoMode.u32Height;

  orxVECTOR asteroidShooterPosition = orxVECTOR_0;
  asteroidShooterPosition.fX = screenWidth / 2.0f - 50.0f;
  asteroidShooterPosition.fY = -screenHeight / 2.0f + 50.0f;
  asteroidShooterPosition.fZ = 0;
  orxObject_SetPosition(asteroidShooterObject, &asteroidShooterPosition);
  orxFLOAT scale = (orxFLOAT)rand() / RAND_MAX;
  orxFLOAT asteroidGunRotation = radian_min_val + scale * (radian_max_val - radian_min_val);
  orxObject_SetRotation(asteroidGunObject, asteroidGunRotation);

  orxObject_Enable(asteroidGunObject, orxTRUE);
  orxObject_Enable(redGunObject, orxFALSE);
  orxObject_Enable(blueGunObject, orxFALSE);
  orxObject_Enable(alienShipGunObject, orxTRUE);

  menuTestObject = orxObject_CreateFromConfig("MenuTest");
  orxObject_Enable(menuTestObject, orxFALSE);
  vMonolithButton = orxObject_CreateFromConfig("MenuTestButtonVMonolith");
  orxObject_Enable(vMonolithButton, orxFALSE);
  pressedButton = orxObject_CreateFromConfig("MenuTestButtonPressed");
  orxObject_Enable(pressedButton, orxFALSE);
  startButton = orxObject_CreateFromConfig("MenuTestButtonStart");
  orxObject_Enable(startButton, orxFALSE);

  sunObject = orxObject_CreateFromConfig("SunObject");

  explosionObject = orxObject_CreateFromConfig("ExplosionObject");
  orxObject_Enable(explosionObject, orxFALSE);

  vMonolithObject = orxObject_CreateFromConfig("VMonolith");

  redFuelCountObject = orxObject_CreateFromConfig("RedFuelCountObject");
  redFuelCountGraphic = orxGRAPHIC(orxOBJECT_GET_STRUCTURE(redFuelCountObject, GRAPHIC));
  redFuelCountStructure = orxGraphic_GetData(redFuelCountGraphic);
  redFuelCount = orxTEXT(redFuelCountStructure);
  redFuelCountFont = orxText_GetFont(redFuelCount);

  blueFuelCountObject = orxObject_CreateFromConfig("BlueFuelCountObject");
  blueFuelCountGraphic = orxGRAPHIC(orxOBJECT_GET_STRUCTURE(blueFuelCountObject, GRAPHIC));
  blueFuelCountStructure = orxGraphic_GetData(blueFuelCountGraphic);
  blueFuelCount = orxTEXT(blueFuelCountStructure);
  blueFuelCountFont = orxText_GetFont(blueFuelCount);

  redShotCountObject = orxObject_CreateFromConfig("RedShotCountObject");
  redShotCountGraphic = orxGRAPHIC(orxOBJECT_GET_STRUCTURE(redShotCountObject, GRAPHIC));
  redShotCountStructure = orxGraphic_GetData(redShotCountGraphic);
  redShotCount = orxTEXT(redShotCountStructure);
  redShotCountFont = orxText_GetFont(redShotCount);

  blueShotCountObject = orxObject_CreateFromConfig("BlueShotCountObject");
  blueShotCountGraphic = orxGRAPHIC(orxOBJECT_GET_STRUCTURE(blueShotCountObject, GRAPHIC));
  blueShotCountStructure = orxGraphic_GetData(blueShotCountGraphic);
  blueShotCount = orxTEXT(blueShotCountStructure);
  blueShotCountFont = orxText_GetFont(blueShotCount);


  // Done!
  orxEvent_AddHandler(orxEVENT_TYPE_PHYSICS, PhysicsEventHandler);
  orxEvent_AddHandler(orxEVENT_TYPE_ANIM, AnimationEventHandler);
  orxEvent_AddHandler(orxEVENT_TYPE_SPAWNER, SpawnerEventHandler);

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
