/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          poisonsm.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to make your smokegrenades dealing damage to other players when within smoke radius
 *  Author:        ArsiRC
 *
 *  Copyright (C) 2009-2011  ArsiRC
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

/**
 * This module's identifier.
 */
new Module:g_modulepoisonsm;

new g_poisonsmUpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
poisonsm_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "poisonsm");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "PoisonSmoke");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to make your smokegrenades dealing damage to other players when within smoke radius");

    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;

    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;
    
    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 5;
    moduledata[ModuleData_Cost] = 10;
    moduledata[ModuleData_iCost] = 10;

    // Send this array of data to the module manager.
    g_modulepoisonsm = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_modulepoisonsm, "Event_OnEventsRegister", "poisonsm_OnEventsRegister");
    #endif
    
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_modulepoisonsm, "poisonsm_OnConfigReload", "configs/thc_rpg/poisonsmoke.txt");
    #endif
}

/**
 * Register all events here.
 */
public poisonsm_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_modulepoisonsm, "Event_OnMapStart",             "poisonsm_OnMapStart");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_modulepoisonsm, "Event_SmokegrenadeDetonated",  "poisonsm_SmokegrenadeDetonated");
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public poisonsm_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public poisonsm_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulepoisonsm, LogType_Normal, "poisonsmConfigReload", "Reloaded poisonsm module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulepoisonsm, CM_CONFIGINDEX_FIRST, "poisonsmModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:poisonsmModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "poisonsmoke", false))
    {
        g_poisonsmUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_poisonsmUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_poisonsmUpgradeConfigCache[UpgradeConfig_Effects] = KvGetNum(kv, "effects");
        g_poisonsmUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_poisonsmUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_poisonsmUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_poisonsmUpgradeConfigCache[UpgradeConfig_Interval] = KvGetFloat(kv, "interval");
        g_poisonsmUpgradeConfigCache[UpgradeConfig_Amount] = KvGetFloat(kv, "amount");
        g_poisonsmUpgradeConfigCache[UpgradeConfig_Suicide] = KvGetNum(kv, "suicide");

        if(g_poisonsmUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_modulepoisonsm);

        ModuleMgr_WriteCell(g_modulepoisonsm, ModuleData_TeamLock, g_poisonsmUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_modulepoisonsm, ModuleData_MaxLevel, g_poisonsmUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_modulepoisonsm, ModuleData_Cost, g_poisonsmUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_modulepoisonsm, ModuleData_iCost, g_poisonsmUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_modulepoisonsm);
}
#endif

/**
 * The map has started.
 */
public poisonsm_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulepoisonsm, CM_CONFIGINDEX_FIRST, "poisonsmModule_ConfigCache");
    #endif
}

public poisonsm_SmokegrenadeDetonated(thrower,Float:x,Float:y,Float:z)
{
    if(IsValidPlayer(thrower))
        if(GetClientTeam(thrower)!=g_poisonsmUpgradeConfigCache[UpgradeConfig_TeamLock])
            if(GetPlayerUpgradeLevel(thrower,g_modulepoisonsm))
            {
                new Handle:datapack=CreateDataPack();
                WritePackCell(datapack,thrower); // Write the client index of owner
                WritePackFloat(datapack,x); // Write the x value of the smokegrenade
                WritePackFloat(datapack,y); // Write the y value of the smokegrenade
                WritePackFloat(datapack,z); // Write the z value of the smokegrenade
                new Handle:hTimer=CreateTimer(g_poisonsmUpgradeConfigCache[UpgradeConfig_Interval],Timer_OnPoisonsmInterval,datapack,TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
                CreateTimer(20.0,Timer_KillTimer,hTimer,TIMER_FLAG_NO_MAPCHANGE);
            }
}

public Action:Timer_OnPoisonsmInterval(Handle:timer,Handle:datapack)
{
    ResetPack(datapack); // Set the reading position to the first cell
    new thrower=ReadPackCell(datapack);  // Get the client index of owner
    new Float:x=ReadPackFloat(datapack); // Get the x value of the smokegrenade
    new Float:y=ReadPackFloat(datapack); // Get the y value of the smokegrenade
    new Float:z=ReadPackFloat(datapack); // Get the z value of the smokegrenade
    
    new Float:victimloc[3];
    new amount;
    
    if(IsClientInGame(thrower))
    {
        new Float:smokeloc[3];
        smokeloc[0]=x;
        smokeloc[1]=y;
        smokeloc[2]=z;

        // Make green beam ring effect (Visible to teammates if freeforall is disabled)
        if(g_poisonsmUpgradeConfigCache[UpgradeConfig_Effects]&&g_CoreConfigCache[CoreConfig_freeforall]==0)
        {
            new color[4]={150,0,150,127};
            new team=GetClientTeam(thrower);
            if(team==2)
                BeamRingEffect("@t",smokeloc,0.0,225.0,g_poisonsmUpgradeConfigCache[UpgradeConfig_Interval],3.0,color,0.0,0);
            if(team==3)
                BeamRingEffect("@ct",smokeloc,0.0,225.0,g_poisonsmUpgradeConfigCache[UpgradeConfig_Interval],3.0,color,0.0,0);
        }
        for(new victim=1;victim<=MaxClients;victim++)
            if(IsClientInGame(victim))
                if(IsPlayerAlive(victim)&&thrower!=victim)
                {
                    GetClientAbsOrigin(victim,victimloc);
                    if(GetDistanceBetween(smokeloc,victimloc)<=225.0)
                    {
                        amount=GetClientHealth(victim)-RoundToNearest(g_poisonsmUpgradeConfigCache[UpgradeConfig_Amount]*GetPlayerUpgradeLevel(thrower,g_modulepoisonsm));
                        if(amount<1)
                        {
                            if(g_poisonsmUpgradeConfigCache[UpgradeConfig_Suicide]==0)
                                amount=1;
                            else
                                amount=0;
                        }
                        SetEntityHealth(victim,amount);
                    }
                }
        return Plugin_Continue;
    }

    CloseHandle(datapack);
    return Plugin_Stop;
}
