/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          gravity.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to jump higher/farther because of a lowered gravity
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
new Module:g_modulegravity;

new g_gravityUpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
gravity_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "gravity");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "Gravity-");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to jump higher/farther because of a lowered gravity");

    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;

    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;
    
    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 10;
    moduledata[ModuleData_Cost] = 10;
    moduledata[ModuleData_iCost] = 15;

    // Send this array of data to the module manager.
    g_modulegravity = ModuleMgr_Register(moduledata);
 
    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_modulegravity, "Event_OnEventsRegister", "gravity_OnEventsRegister");
    #endif
     
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_modulegravity, "gravity_OnConfigReload", "configs/thc_rpg/gravity.txt");
    #endif
}

/**
 * Register all events here.
 */
public gravity_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_modulegravity, "Event_OnMyModuleEnable",        "gravity_OnMyModuleEnable");
    EventMgr_RegisterEvent(g_modulegravity, "Event_OnMyModuleDisable",       "gravity_OnMyModuleDisable");
    EventMgr_RegisterEvent(g_modulegravity, "Event_OnMapStart",              "gravity_OnMapStart");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_modulegravity, "Event_PlayerSpawn",             "gravity_PlayerSpawn");
    #endif

    // Custom events
    EventMgr_RegisterEvent(g_modulegravity, "Event_OnUpgradePurchase",       "gravity_OnUpgradePurchase");
    EventMgr_RegisterEvent(g_modulegravity, "Event_OnUpgradeSell",           "gravity_OnUpgradeSell");
    EventMgr_RegisterEvent(g_modulegravity, "Event_OnGameFrame",             "gravity_OnGameFrame");
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public gravity_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public gravity_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulegravity, LogType_Normal, "gravityConfigReload", "Reloaded gravity module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulegravity, CM_CONFIGINDEX_FIRST, "gravityModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:gravityModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "gravity", false))
    {
        g_gravityUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_gravityUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_gravityUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_gravityUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_gravityUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_gravityUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");

        if(g_gravityUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_modulegravity);

        ModuleMgr_WriteCell(g_modulegravity, ModuleData_TeamLock, g_gravityUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_modulegravity, ModuleData_MaxLevel, g_gravityUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_modulegravity, ModuleData_Cost, g_gravityUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_modulegravity, ModuleData_iCost, g_gravityUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_modulegravity);
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
public Action:gravity_OnMyModuleEnable(String:refusalmsg[], maxlen)
{
    new client;
    new Handle:vecPlayer;
    
    new size=GetArraySize(g_hVecPlayers);
    for(new x=0;x<size;x++)
    {
        vecPlayer=GetArrayCell(g_hVecPlayers,x);
        client=GetArrayCell(vecPlayer,VECTOR_PLAYERS_CLIENT);
        if(IsValidPlayer(client))
            gravity_Action(client, GetPlayerUpgradeLevel(client,g_modulegravity));
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
public Action:gravity_OnMyModuleDisable(String:refusalmsg[], maxlen)
{
    new client;
    new Handle:vecPlayer;
    
    new size=GetArraySize(g_hVecPlayers);
    for(new x=0;x<size;x++)
    {
        vecPlayer=GetArrayCell(g_hVecPlayers,x);
        client=GetArrayCell(vecPlayer,VECTOR_PLAYERS_CLIENT);
        if(IsValidPlayer(client))
            SetEntityGravity(client, 1.0);
    }

    return Plugin_Continue;
}

/**
 * The map has started.
 */
public gravity_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulegravity, CM_CONFIGINDEX_FIRST, "gravityModule_ConfigCache");
    #endif
}

/**
 * Client has spawned.
 * 
 * @param client    The client index.
 * 
 */
public gravity_PlayerSpawn(client)
{
    if(IsValidPlayer(client))
        gravity_Action(client, GetPlayerUpgradeLevel(client,g_modulegravity));
}

/**
 * Client has purchased an Upgrade
 */
public gravity_OnUpgradePurchase(client, level)
{
    if(IsValidPlayer(client))
        gravity_Action(client, level);
}

/**
 * Client has sold an Upgrade
 */
public gravity_OnUpgradeSell(client, level)
{
    if(IsValidPlayer(client))
        gravity_Action(client, level);
}

/**
 * Called before every server frame.
 * Note that you should avoid doing expensive computations here,
 * and you should declare large local arrays using 'decl' instead of 'new' 
 */
public gravity_OnGameFrame()
{
    if(!ModuleMgr_IsDisabled(g_modulegravity))
        for(new client=1;client<=MaxClients;client++)
            if(IsValidPlayer(client))
                gravity_Action(client, GetPlayerUpgradeLevel(client,g_modulegravity));
}

public gravity_Action(client, level)
{
    if(GetClientTeam(client)!=g_gravityUpgradeConfigCache[UpgradeConfig_TeamLock])
        if(level>0)
        {
            new Float:gravity=1.0-(level*g_gravityUpgradeConfigCache[UpgradeConfig_Percent]);
            if(gravity<0.0)
                gravity=0.0;
            SetEntityGravity(client, gravity);
        }
}
