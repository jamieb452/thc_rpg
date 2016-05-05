/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          stealth.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to become more invisible
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
new Module:g_modulestealth;

new g_stealthUpgradeConfigCache[UpgradeConfigs];

/**
 * Plugin is loading.
 * Register this module.
 */
stealth_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "stealth");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "Stealth");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to become more invisible");

    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;

    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;
    
    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 5;
    moduledata[ModuleData_Cost] = 15;
    moduledata[ModuleData_iCost] = 10;

    // Send this array of data to the module manager.
    g_modulestealth = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_modulestealth, "Event_OnEventsRegister", "stealth_OnEventsRegister");
    #endif
    
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_modulestealth, "stealth_OnConfigReload", "configs/thc_rpg/stealth.txt");
    #endif
}

/**
 * Register all events here.
 */
public stealth_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_modulestealth, "Event_OnMyModuleEnable",        "stealth_OnMyModuleEnable");
    EventMgr_RegisterEvent(g_modulestealth, "Event_OnMyModuleDisable",       "stealth_OnMyModuleDisable");
    EventMgr_RegisterEvent(g_modulestealth, "Event_OnMapStart",              "stealth_OnMapStart");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_modulestealth, "Event_PlayerSpawn",             "stealth_PlayerSpawn");
    #endif

    // Custom events
    EventMgr_RegisterEvent(g_modulestealth, "Event_OnUpgradePurchase",       "stealth_OnUpgradePurchase");
    EventMgr_RegisterEvent(g_modulestealth, "Event_OnUpgradeSell",           "stealth_OnUpgradeSell");
    EventMgr_RegisterEvent(g_modulestealth, "Event_OnGameFrame",             "stealth_OnGameFrame");
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public stealth_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public stealth_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulestealth, LogType_Normal, "stealthConfigReload", "Reloaded stealth module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulestealth, CM_CONFIGINDEX_FIRST, "stealthModule_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:stealthModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "stealth", false))
    {
        g_stealthUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_stealthUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_stealthUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_stealthUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_stealthUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_stealthUpgradeConfigCache[UpgradeConfig_MinAlpha] = KvGetNum(kv, "minalpha");

        if(g_stealthUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_modulestealth);

        ModuleMgr_WriteCell(g_modulestealth, ModuleData_TeamLock, g_stealthUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_modulestealth, ModuleData_MaxLevel, g_stealthUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_modulestealth, ModuleData_Cost, g_stealthUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_modulestealth, ModuleData_iCost, g_stealthUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_modulestealth);
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
public Action:stealth_OnMyModuleEnable(String:refusalmsg[], maxlen)
{
    new client;
    new Handle:vecPlayer;
    
    new size=GetArraySize(g_hVecPlayers);
    for(new x=0;x<size;x++)
    {
        vecPlayer=GetArrayCell(g_hVecPlayers,x);
        client=GetArrayCell(vecPlayer,VECTOR_PLAYERS_CLIENT);
        if(IsValidPlayer(client))
            stealth_Action(client, GetPlayerUpgradeLevel(client,g_modulestealth));
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
public Action:stealth_OnMyModuleDisable(String:refusalmsg[], maxlen)
{
    new client;
    new Handle:vecPlayer;
    
    new size=GetArraySize(g_hVecPlayers);
    for(new x=0;x<size;x++)
    {
        vecPlayer=GetArrayCell(g_hVecPlayers,x);
        client=GetArrayCell(vecPlayer,VECTOR_PLAYERS_CLIENT);
        if(IsValidPlayer(client))
            SetColor(client,-1,-1,-1,255);
    }

    return Plugin_Continue;
}

/**
 * The map has started.
 */
public stealth_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulestealth, CM_CONFIGINDEX_FIRST, "stealthModule_ConfigCache");
    #endif
}

/**
 * Client has spawned.
 *
 * @param client    The client index.
 *
 */
public stealth_PlayerSpawn(client)
{
    if(IsValidPlayer(client))
        stealth_Action(client, GetPlayerUpgradeLevel(client,g_modulestealth));
}

/**
 * Client has purchased an Upgrade
 */
public stealth_OnUpgradePurchase(client, level)
{
    if(IsValidPlayer(client))
        stealth_Action(client, level);
}

/**
 * Client has sold an Upgrade
 */
public stealth_OnUpgradeSell(client, level)
{
    if(IsValidPlayer(client))
        stealth_Action(client, level);
}

/**
 * Called before every server frame.
 * Note that you should avoid doing expensive computations here,
 * and you should declare large local arrays using 'decl' instead of 'new' 
 */
public stealth_OnGameFrame()
{
    if(!ModuleMgr_IsDisabled(g_modulestealth))
        for(new client=1;client<=MaxClients;client++)
            if(IsValidPlayer(client))
                stealth_Action(client, GetPlayerUpgradeLevel(client,g_modulestealth));
}

public stealth_Action(client, level)
{
    if(GetClientTeam(client)!=g_stealthUpgradeConfigCache[UpgradeConfig_TeamLock])
        if(level>0)
        {
            new alpha=(255/g_stealthUpgradeConfigCache[UpgradeConfig_MaxLevel])*(g_stealthUpgradeConfigCache[UpgradeConfig_MaxLevel]-level);
            if(alpha<g_stealthUpgradeConfigCache[UpgradeConfig_MinAlpha])
                alpha=g_stealthUpgradeConfigCache[UpgradeConfig_MinAlpha];
            SetColor(client,-1,-1,-1,alpha);
        }
}
