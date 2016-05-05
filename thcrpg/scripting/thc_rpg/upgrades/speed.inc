/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          speed.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to receive extra speed
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
new Module:g_modulespeed;

new g_speedUpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
speed_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "speed");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "Speed+");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to steadily regain HP until health is fully replenished");

    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;
    
    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;
    
    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 6;
    moduledata[ModuleData_Cost] = 10;
    moduledata[ModuleData_iCost] = 10;

    // Send this array of data to the module manager.
    g_modulespeed = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_modulespeed, "Event_OnEventsRegister", "speed_OnEventsRegister");
    #endif
    
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_modulespeed, "speed_OnConfigReload", "configs/thc_rpg/speed.txt");
    #endif
}

/**
 * Register all events here.
 */
public speed_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_modulespeed, "Event_OnMyModuleEnable",          "speed_OnMyModuleEnable");
    EventMgr_RegisterEvent(g_modulespeed, "Event_OnMyModuleDisable",         "speed_OnMyModuleDisable");
    EventMgr_RegisterEvent(g_modulespeed, "Event_OnMapStart",                "speed_OnMapStart");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_modulespeed, "Event_PlayerSpawn",               "speed_PlayerSpawn");
    #endif
        
    // Custom events
    EventMgr_RegisterEvent(g_modulespeed, "Event_OnUpgradePurchase",         "speed_OnUpgradePurchase");
    EventMgr_RegisterEvent(g_modulespeed, "Event_OnUpgradeSell",             "speed_OnUpgradeSell");
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public speed_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public speed_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulespeed, LogType_Normal, "SpeedConfigReload", "Reloaded speed module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulespeed, CM_CONFIGINDEX_FIRST, "speedModule_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:speedModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "speed", false))
    {
        g_speedUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_speedUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_speedUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_speedUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_speedUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_speedUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");

        if(g_speedUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_modulespeed);

        ModuleMgr_WriteCell(g_modulespeed, ModuleData_TeamLock, g_speedUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_modulespeed, ModuleData_MaxLevel, g_speedUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_modulespeed, ModuleData_Cost, g_speedUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_modulespeed, ModuleData_iCost, g_speedUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_modulespeed);
}
#endif

/**
 * The module that hooked this event callback has been enabled.
 *
 * @param refusalmsg    The string that is printed if Plugin_Handled is returned and it is non-empty.
 * @param maxlen        The max length of the string.
 *
 * @return      Return Plugin_Handled to stop enable, and Plugin_Continue to allow it.
 */
public Action:speed_OnMyModuleEnable(String:refusalmsg[], maxlen)
{
    new client;
    new Handle:vecPlayer;
    
    new size=GetArraySize(g_hVecPlayers);
    for(new x=0;x<size;x++)
    {
        vecPlayer=GetArrayCell(g_hVecPlayers,x);
        client=GetArrayCell(vecPlayer,VECTOR_PLAYERS_CLIENT);
        if(IsValidPlayer(client))
            speed_Action(client, GetPlayerUpgradeLevel(client,g_modulespeed));
    }

    return Plugin_Continue;
}

/**
 * The module that hooked this event callback has been disabled.
 *
 * @param refusalmsg    The string that is printed if Plugin_Handled is returned and it is non-empty.
 * @param maxlen        The max length of the string.
 *
 * @return      Return Plugin_Handled to stop disable, and Plugin_Continue to allow it.
 */
public Action:speed_OnMyModuleDisable(String:refusalmsg[], maxlen)
{
    new client;
    new Handle:vecPlayer;
    
    new size=GetArraySize(g_hVecPlayers);
    for(new x=0;x<size;x++)
    {
        vecPlayer=GetArrayCell(g_hVecPlayers,x);
        client=GetArrayCell(vecPlayer,VECTOR_PLAYERS_CLIENT);
        if(IsValidPlayer(client))
        {
            SetModifiedSpeed(client, GetSpawnSpeed(client));
            SetEntPropFloat(client, Prop_Data, "m_flLaggedMovementValue", GetSpawnSpeed(client));
        }
    }

    return Plugin_Continue;
}

/**
 * The map has started.
 */
public speed_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulespeed, CM_CONFIGINDEX_FIRST, "speedModule_ConfigCache");
    #endif
}

/**
 * Client has spawned.
 *
 * @param client    The client index.
 *
 */
public speed_PlayerSpawn(client)
{
    speed_Action(client, GetPlayerUpgradeLevel(client,g_modulespeed));
}

/**
 * Client has purchased an Upgrade
 */
public speed_OnUpgradePurchase(client, level)
{
    speed_Action(client, level);
}

/**
 * Client has sold an Upgrade
 */
public speed_OnUpgradeSell(client, level)
{
    speed_Action(client, level);
}

public speed_Action(client, level)
{
    if(IsValidPlayer(client))
        if(GetClientTeam(client)!=g_speedUpgradeConfigCache[UpgradeConfig_TeamLock])
            if(level>0)
            {
                new Float:amount=GetSpawnSpeed(client)+(level*g_speedUpgradeConfigCache[UpgradeConfig_Percent]);
                SetModifiedSpeed(client, amount);
                SetEntPropFloat(client, Prop_Data, "m_flLaggedMovementValue", amount);
            }
}
