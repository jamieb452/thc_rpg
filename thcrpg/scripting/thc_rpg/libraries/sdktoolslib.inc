/*
 * ============================================================================
 *
 *  SourceMod Project Base
 *
 *  File:          sdktoolslib.inc
 *  Type:          Library
 *  Description:   Pre-defined sdktool hooks.
 *
 *  Copyright (C) 2009-2011  Greyscale
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

#if defined _sdktoolslib_included
 #endinput
#endif
#define _sdktoolslib_included

#include <sdktools>

/**
 * List of available sdktools functions.
 */
enum SDKLibCalls
{
    SDKLibCall_Placeholder,         /**  This is here so other games besides CS:S can compile. */
    #if defined PROJECT_GAME_CSS
    SDKLibCall_TerminateRound,      /** Terminate the round with a given reason. */
    SDKLibCall_CSWeaponDrop,        /** Force client to drop weapon. */
    SDKLibCall_FlashlightIsOn,      /** Check is a client's flashlight is on. */
    SDKLibCall_FlashlightTurnOn,    /** Turn a client's flashlight on. */
    SDKLibCall_FlashlightTurnOff,   /** Turn a client's flashlight off. */
    #endif
}

/**
 * Handles to all the pre-defined sdkcalls.
 */
new Handle:g_hSDKCall[SDKLibCalls];

/**
 * The name of the sdkcall.  This is the name that should be used in the gamedata file.
 */
stock String:g_strSDKCallName[SDKLibCalls][] =
{
    "Placeholder"
    #if defined PROJECT_GAME_CSS
   ,"TerminateRound",
    "CSWeaponDrop",
    "FlashlightIsOn",
    "FlashlightTurnOn",
    "FlashlightTurnOff"
    #endif
};

/**
 * The class of the sdkcall.  (The text before the "::FuncName")
 */
stock String:g_strSDKCallClass[SDKLibCalls][] =
{
    "Placeholder"
    #if defined PROJECT_GAME_CSS
   ,"CGameRules",
    "CBasePlayer",
    "CCSPlayer",
    "CCSPlayer",
    "CCSPlayer"
    #endif
};

/**
 * Check if a pre-defined SDKCall is available.
 * A gamedata file must be loaded and SDKToolsLib_SetupPredefined must be called.
 * 
 * @param sdkcall   The pre-defined SDKCall.  See enum SDKLibCalls.
 * 
 * @return          True if available to use its stock below, false if it hasn't been set up or it failed.
 */
stock bool:SDKToolsLib_IsAvailable(SDKLibCalls:sdkcall)
{
    return (g_hSDKCall[sdkcall] != INVALID_HANDLE);
}
/**
 * Prep a pre-defined SDKCall.
 * 
 * @param gameconfig    Handle to the loaded gamedata file.
 * @param sdkcall       The pre-defined SDKCall.  See enum SDKLibCalls.
 * 
 * @return              Handle to the call to be used by sdktools.inc.
 */
stock Handle:SDKToolsLib_SetupPredefined(Handle:gameconfig, SDKLibCalls:sdkcall)
{
    // If SDKCall is already available then return its handle.
    if (SDKToolsLib_IsAvailable(sdkcall))
        return g_hSDKCall[sdkcall];
    
    switch(sdkcall)
    {
        #if defined PROJECT_GAME_CSS
        case SDKLibCall_TerminateRound:
        {
            StartPrepSDKCall(SDKCall_GameRules);
            PrepSDKCall_SetFromConf(gameconfig, SDKConf_Signature, g_strSDKCallName[sdkcall]);
            PrepSDKCall_AddParameter(SDKType_Float, SDKPass_Plain);
            PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
            g_hSDKCall[sdkcall] = EndPrepSDKCall();
        }
        case SDKLibCall_CSWeaponDrop:
        {
            StartPrepSDKCall(SDKCall_Player);
            PrepSDKCall_SetFromConf(gameconfig, SDKConf_Signature, g_strSDKCallName[sdkcall]);
            PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
            PrepSDKCall_AddParameter(SDKType_Bool, SDKPass_Plain);
            PrepSDKCall_AddParameter(SDKType_Bool, SDKPass_Plain);
            g_hSDKCall[sdkcall] = EndPrepSDKCall();
        }
        case SDKLibCall_FlashlightIsOn:
        {
            StartPrepSDKCall(SDKCall_Player);
            PrepSDKCall_SetFromConf(gameconfig, SDKConf_Virtual, g_strSDKCallName[sdkcall]);
            PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
            g_hSDKCall[sdkcall] = EndPrepSDKCall();
        }
        case SDKLibCall_FlashlightTurnOn:
        {
            StartPrepSDKCall(SDKCall_Player);
            PrepSDKCall_SetFromConf(gameconfig, SDKConf_Virtual, g_strSDKCallName[sdkcall]);
            g_hSDKCall[sdkcall] = EndPrepSDKCall();
        }
        case SDKLibCall_FlashlightTurnOff:
        {
            StartPrepSDKCall(SDKCall_Player);
            PrepSDKCall_SetFromConf(gameconfig, SDKConf_Virtual, g_strSDKCallName[sdkcall]);
            g_hSDKCall[sdkcall] = EndPrepSDKCall();
        }
        #endif
        default:    ThrowError("Invalid pre-defined SDKCall.");
    }
    
    // Check if the function was successfully prepped.
    if (g_hSDKCall[sdkcall] == INVALID_HANDLE)
    {
        SetFailState("Game function \"%s::%s\" was not found.", g_strSDKCallClass[sdkcall], g_strSDKCallName[sdkcall]);
    }
    
    return g_hSDKCall[sdkcall];
}

#if defined PROJECT_GAME_CSS

/**
 * All round end reasons.
 * Used an enum instead of defines so the include order doesn't matter.
 */
enum RoundEndReasons
{
    ROUNDEND_TARGET_BOMBED = 0,         /** Target Successfully Bombed! */
    ROUNDEND_VIP_ESCAPED,               /** The VIP has escaped! */
    ROUNDEND_VIP_ASSASSINATED,          /** VIP has been assassinated! */
    ROUNDEND_TERRORISTS_ESCAPED,        /** The terrorists have escaped! */
    ROUNDEND_CTS_PREVENTESCAPE,         /** The CT's have prevented most of the terrorists from escaping! */
    ROUNDEND_ESC_TS_NEUTRALIZED,        /** Escaping terrorists have all been neutralized! */
    ROUNDEND_BOMB_DEFUSED,              /** The bomb has been defused! */
    ROUNDEND_CTS_WIN,                   /** Counter-Terrorists Win! */
    ROUNDEND_TERRORISTS_WIN,            /** Terrorists Win! */
    ROUNDEND_ROUND_DRAW,                /** Round Draw! */
    ROUNDEND_ALL_HOSTAGES_RESCUED,      /** All Hostages have been rescued! */
    ROUNDEND_TARGET_SAVED,              /** Target has been saved! */
    ROUNDEND_HOSTAGES_NOT_RESCUED,      /** Hostages have not been rescued! */
    ROUNDEND_TERRORISTS_NOT_ESCAPED,    /** Terrorists have not escaped! */
    ROUNDEND_VIP_NOT_ESCAPED,           /** VIP has not escaped! */
    ROUNDEND_GAME_COMMENCING            /** Game Commencing! */
}

/**
 * Terminates the round. (CS:S)
 * 
 * @param delay     Time to wait before new round starts.
 * @param reason    Round end reason.  See enum RoundEndReasons.
 */
stock SDKToolsLib_TerminateRound(Float:delay, RoundEndReasons:reason)
{
    // Compiler bug?
    SDKCall(Handle:g_hSDKCall[SDKLibCall_TerminateRound], delay, _:reason);
}

/**
 * Forces client to drop a weapon. (CS:S)
 * 
 * @param client    The client index.
 * @param weapon    The weapon entity index to force client to drop.
 */
stock SDKToolsLib_CSWeaponDrop(client, weapon)
{
    SDKCall(Handle:g_hSDKCall[SDKLibCall_CSWeaponDrop], client, weapon, true, false);
}

/**
 * Check if the client's flashlight is turned on. (CS:S)
 * 
 * @param client    The client index.
 * 
 * @return          True if the light is on, false if off.
 */
stock bool:SDKToolsLib_FlashlightIsOn(client)
{
    return bool:SDKCall(Handle:g_hSDKCall[SDKLibCall_FlashlightIsOn], client);
}

/**
 * Force a client's flashlight on. (CS:S)
 * 
 * @param client    The client index.
 */
stock SDKToolsLib_FlashlightTurnOn(client)
{
    SDKCall(Handle:g_hSDKCall[SDKLibCall_FlashlightTurnOn], client);
}

/**
 * Force a client's flashlight off. (CS:S)
 * 
 * @param client    The client index.
 */
stock SDKToolsLib_FlashlightTurnOff(client)
{
    SDKCall(Handle:g_hSDKCall[SDKLibCall_FlashlightTurnOff], client);
}

#endif
