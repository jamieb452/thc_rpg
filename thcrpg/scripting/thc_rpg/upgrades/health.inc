/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          health.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to receive extra health
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
new Module:g_modulehealth;

new g_healthUpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
health_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "health");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "Health+");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to receive extra health");

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
    g_modulehealth = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_modulehealth, "Event_OnEventsRegister", "health_OnEventsRegister");
    #endif
    
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_modulehealth, "health_OnConfigReload", "configs/thc_rpg/health.txt");
    #endif
}

/**
 * Register all events here.
 */
public health_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_modulehealth, "Event_OnMyModuleEnable",        "health_OnMyModuleEnable");
    EventMgr_RegisterEvent(g_modulehealth, "Event_OnMyModuleDisable",       "health_OnMyModuleDisable");
    EventMgr_RegisterEvent(g_modulehealth, "Event_OnMapStart",              "health_OnMapStart");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_modulehealth, "Event_PlayerSpawn",         "health_PlayerSpawn");
    #endif
        
    // Custom events
    EventMgr_RegisterEvent(g_modulehealth, "Event_OnUpgradePurchase",       "health_OnUpgradePurchase");
    EventMgr_RegisterEvent(g_modulehealth, "Event_OnUpgradeSell",           "health_OnUpgradeSell");
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public health_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public health_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulehealth, LogType_Normal, "HealthConfigReload", "Reloaded health module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulehealth, CM_CONFIGINDEX_FIRST, "healthModule_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:healthModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "health", false))
    {
        g_healthUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_healthUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_healthUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_healthUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_healthUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_healthUpgradeConfigCache[UpgradeConfig_Amount] = KvGetFloat(kv, "amount");

        if(g_healthUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_modulehealth);

        ModuleMgr_WriteCell(g_modulehealth, ModuleData_TeamLock, g_healthUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_modulehealth, ModuleData_MaxLevel, g_healthUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_modulehealth, ModuleData_Cost, g_healthUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_modulehealth, ModuleData_iCost, g_healthUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_modulehealth);
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
public Action:health_OnMyModuleEnable(String:refusalmsg[], maxlen)
{
    new client;
    new Handle:vecPlayer;
    
    new size=GetArraySize(g_hVecPlayers);
    for(new x=0;x<size;x++)
    {
        vecPlayer=GetArrayCell(g_hVecPlayers,x);
        client=GetArrayCell(vecPlayer,VECTOR_PLAYERS_CLIENT);
        if(IsValidPlayer(client))
            health_Action(client, GetPlayerUpgradeLevel(client,g_modulehealth));
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
public Action:health_OnMyModuleDisable(String:refusalmsg[], maxlen)
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
            SetModifiedHealth(client,GetSpawnHealth(client));
            SetEntityHealth(client,GetSpawnHealth(client));
        }
    }
    
    return Plugin_Continue;
}

/**
 * The map has started.
 */
public health_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulehealth, CM_CONFIGINDEX_FIRST, "healthModule_ConfigCache");
    #endif
}

/**
 * Client has spawned.
 *
 * @param client    The client index.
 *
 */
public health_PlayerSpawn(client)
{
    health_Action(client, GetPlayerUpgradeLevel(client,g_modulehealth));
}

/**
 * Client has purchased an Upgrade
 */
public health_OnUpgradePurchase(client, level)
{
    health_Action(client, level);
}

/**
 * Client has sold an Upgrade
 */
public health_OnUpgradeSell(client, level)
{
    health_Action(client, level);
}

public health_Action(client, level)
{
    if(IsValidPlayer(client))
        if(GetClientTeam(client)!=g_healthUpgradeConfigCache[UpgradeConfig_TeamLock])
            if(level>0)
            {
                new amount=GetSpawnHealth(client)+RoundToNearest(level*g_healthUpgradeConfigCache[UpgradeConfig_Amount]);
                SetModifiedHealth(client, amount);
                SetEntityHealth(client, amount);
            }
}
