/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          impulse.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to move faster when shooting
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
new Module:g_moduleimpulse;

new g_impulseUpgradeConfigCache[UpgradeConfigs];

new Handle:g_hPlayerIsImpulsed[MAXPLAYERS+1] = {INVALID_HANDLE,...};

/**
 * Register this module.
 */
impulse_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "impulse");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "Impulse");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to move faster when shooting");

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
    g_moduleimpulse = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleimpulse, "Event_OnEventsRegister", "impulse_OnEventsRegister");
    #endif
    
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleimpulse, "impulse_OnConfigReload", "configs/thc_rpg/impulse.txt");
    #endif
}

/**
 * Register all events here.
 */
public impulse_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleimpulse, "Event_OnMyModuleEnable",             "impulse_OnMyModuleEnable");
    EventMgr_RegisterEvent(g_moduleimpulse, "Event_OnMyModuleDisable",            "impulse_OnMyModuleDisable");
    EventMgr_RegisterEvent(g_moduleimpulse, "Event_OnMapStart",                   "impulse_OnMapStart");
    EventMgr_RegisterEvent(g_moduleimpulse, "Event_OnClientDisconnect",           "impulse_OnClientDisconnect");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_moduleimpulse, "Event_PlayerSpawn",              "impulse_PlayerSpawn");
        EventMgr_RegisterEvent(g_moduleimpulse, "Event_PlayerDeath",              "impulse_PlayerDeath");
        EventMgr_RegisterEvent(g_moduleimpulse, "Event_WeaponFire",               "impulse_WeaponFire");
    #endif

    // Custom events
    EventMgr_RegisterEvent(g_moduleimpulse, "Event_OnUpgradePurchase",            "impulse_OnUpgradePurchase");
    EventMgr_RegisterEvent(g_moduleimpulse, "Event_OnUpgradeSell",                "impulse_OnUpgradeSell");
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public impulse_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public impulse_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleimpulse, LogType_Normal, "impulseConfigReload", "Reloaded speed module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleimpulse, CM_CONFIGINDEX_FIRST, "impulseModule_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:impulseModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "impulse", false))
    {
        g_impulseUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_impulseUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_impulseUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_impulseUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_impulseUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_impulseUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");
        g_impulseUpgradeConfigCache[UpgradeConfig_Interval] = KvGetFloat(kv, "interval");

        if(g_impulseUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_moduleimpulse);

        ModuleMgr_WriteCell(g_moduleimpulse, ModuleData_TeamLock, g_impulseUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_moduleimpulse, ModuleData_MaxLevel, g_impulseUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleimpulse, ModuleData_Cost, g_impulseUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleimpulse, ModuleData_iCost, g_impulseUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduleimpulse);
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
public Action:impulse_OnMyModuleEnable(String:refusalmsg[], maxlen)
{
    new size=GetArraySize(g_hVecPlayers);
    for(new x=0;x<size;x++)
    {
        new Handle:vecPlayer=GetArrayCell(g_hVecPlayers,x);
        new client=GetArrayCell(vecPlayer,VECTOR_PLAYERS_CLIENT);
        if(IsValidPlayer(client))
            impulse_StopImpulse(client);
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
public Action:impulse_OnMyModuleDisable(String:refusalmsg[], maxlen)
{
    new size=GetArraySize(g_hVecPlayers);
    for(new x=0;x<size;x++)
    {
        new Handle:vecPlayer=GetArrayCell(g_hVecPlayers,x);
        new client=GetArrayCell(vecPlayer,VECTOR_PLAYERS_CLIENT);
        if(IsValidPlayer(client))
            impulse_StopImpulse(client);
    }

    return Plugin_Continue;
}

/**
 * The map has started.
 */
public impulse_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleimpulse, CM_CONFIGINDEX_FIRST, "impulseModule_ConfigCache");
    #endif
}

/**
 * Client is disconnecting from the server.
 *
 * @param client    The client index.
 */
public impulse_OnClientDisconnect(client)
{
    impulse_StopImpulse(client);
}

/**
 * Client has spawned.
 *
 * @param client    The client index.
 *
 */
public impulse_PlayerSpawn(client)
{
    impulse_StopImpulse(client);
}

/**
 * Client has been killed.
 *
 * @param victim    The index of the killed client.
 * @param attacker  The killer of the victim.
 * @param weapon    The weapon classname used to kill the victim. (No weapon_ prefix)
 * @param headshot  True if the death was by headshot, false if not.
 */
public impulse_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
    if(IsValidPlayer(victim))
        impulse_StopImpulse(victim);
}

/**
 * Client has fired a weapon.
 * 
 * @param client        The client index.
 * @param weapon        The weapon classname fired. (No weapon_ prefix)
 */
public impulse_WeaponFire(client, const String:weapon[])
{
    if(IsValidPlayer(client)&&g_hPlayerIsImpulsed[client]==INVALID_HANDLE)
        if(GetClientTeam(client)!=g_impulseUpgradeConfigCache[UpgradeConfig_TeamLock])
        {
            new level=GetPlayerUpgradeLevel(client,g_moduleimpulse);
            if(level>0)
            {
                new Float:amount=GetModifiedSpeed(client)+(level*g_impulseUpgradeConfigCache[UpgradeConfig_Percent]);
                SetEntPropFloat(client, Prop_Data, "m_flLaggedMovementValue", amount);
                g_hPlayerIsImpulsed[client]=CreateTimer(g_impulseUpgradeConfigCache[UpgradeConfig_Interval],Timer_OnImpulseStop,client,TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
            }
        }
}

/**
* Client has purchased an Upgrade
*/
public impulse_OnUpgradePurchase(client, level)
{
    if(IsValidPlayer(client))
        impulse_StopImpulse(client);
}

/**
* Client has sold an Upgrade
*/
public impulse_OnUpgradeSell(client, level)
{
    if(IsValidPlayer(client))
        impulse_StopImpulse(client);
}

public Action:Timer_OnImpulseStop(Handle:timer,any:client)
{
    SetEntPropFloat(client, Prop_Data, "m_flLaggedMovementValue", GetModifiedSpeed(client));
    g_hPlayerIsImpulsed[client]=INVALID_HANDLE;
    
    return Plugin_Stop;
}

public impulse_StopImpulse(client)
{
    if(IsValidPlayer(client))
        if(GetClientTeam(client)!=g_impulseUpgradeConfigCache[UpgradeConfig_TeamLock])
            SetEntPropFloat(client, Prop_Data, "m_flLaggedMovementValue", GetModifiedSpeed(client));

    if(g_hPlayerIsImpulsed[client]!=INVALID_HANDLE)
    {
        KillTimer(g_hPlayerIsImpulsed[client]);
        g_hPlayerIsImpulsed[client]=INVALID_HANDLE;
    }
}
