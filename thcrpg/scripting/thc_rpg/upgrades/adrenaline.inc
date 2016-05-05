/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          adrenaline.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to turn up your speed when you got hurt
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
new Module:g_moduleadren;

new g_adrenUpgradeConfigCache[UpgradeConfigs];

new Handle:g_hPlayerIsAdrenalined[MAXPLAYERS+1] = {INVALID_HANDLE,...};

/**
 * Register this module.
 */
adren_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "adren");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "Adrenaline");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to turn up your speed when you got hurt");

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
    g_moduleadren = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleadren, "Event_OnEventsRegister", "adren_OnEventsRegister");
    #endif
    
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleadren, "adren_OnConfigReload", "configs/thc_rpg/adrenaline.txt");
    #endif
}

/**
 * Register all events here.
 */
public adren_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleadren, "Event_OnMyModuleEnable",             "adren_OnMyModuleEnable");
    EventMgr_RegisterEvent(g_moduleadren, "Event_OnMyModuleDisable",            "adren_OnMyModuleDisable");
    EventMgr_RegisterEvent(g_moduleadren, "Event_OnMapStart",                   "adren_OnMapStart");
    EventMgr_RegisterEvent(g_moduleadren, "Event_OnClientDisconnect",           "adren_OnClientDisconnect");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_moduleadren, "Event_PlayerSpawn",              "adren_PlayerSpawn");
        EventMgr_RegisterEvent(g_moduleadren, "Event_PlayerHurt",               "adren_PlayerHurt");
        EventMgr_RegisterEvent(g_moduleadren, "Event_PlayerDeath",              "adren_PlayerDeath");
    #endif

    // Custom events
    EventMgr_RegisterEvent(g_moduleadren, "Event_OnUpgradePurchase",            "adren_OnUpgradePurchase");
    EventMgr_RegisterEvent(g_moduleadren, "Event_OnUpgradeSell",                "adren_OnUpgradeSell");
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public adren_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public adren_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleadren, LogType_Normal, "AdrenConfigReload", "Reloaded speed module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleadren, CM_CONFIGINDEX_FIRST, "adrenModule_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:adrenModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "adrenaline", false))
    {
        g_adrenUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_adrenUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_adrenUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_adrenUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_adrenUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_adrenUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");
        g_adrenUpgradeConfigCache[UpgradeConfig_Interval] = KvGetFloat(kv, "interval");
        g_adrenUpgradeConfigCache[UpgradeConfig_Effects] = KvGetNum(kv, "frozen");

        if(g_adrenUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_moduleadren);

        ModuleMgr_WriteCell(g_moduleadren, ModuleData_TeamLock, g_adrenUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_moduleadren, ModuleData_MaxLevel, g_adrenUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleadren, ModuleData_Cost, g_adrenUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleadren, ModuleData_iCost, g_adrenUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduleadren);
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
public Action:adren_OnMyModuleEnable(String:refusalmsg[], maxlen)
{
    new size=GetArraySize(g_hVecPlayers);
    for(new x=0;x<size;x++)
    {
        new Handle:vecPlayer=GetArrayCell(g_hVecPlayers,x);
        new client=GetArrayCell(vecPlayer,VECTOR_PLAYERS_CLIENT);
        if(IsValidPlayer(client))
            adren_StopAdrenaline(client);
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
public Action:adren_OnMyModuleDisable(String:refusalmsg[], maxlen)
{
    new size=GetArraySize(g_hVecPlayers);
    for(new x=0;x<size;x++)
    {
        new Handle:vecPlayer=GetArrayCell(g_hVecPlayers,x);
        new client=GetArrayCell(vecPlayer,VECTOR_PLAYERS_CLIENT);
        if(IsValidPlayer(client))
            adren_StopAdrenaline(client);
    }

    return Plugin_Continue;
}

/**
 * The map has started.
 */
public adren_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleadren, CM_CONFIGINDEX_FIRST, "adrenModule_ConfigCache");
    #endif
}

/**
 * Client is disconnecting from the server.
 *
 * @param client    The client index.
 */
public adren_OnClientDisconnect(client)
{
    adren_StopAdrenaline(client);
}

/**
 * Client has spawned.
 *
 * @param client    The client index.
 *
 */
public adren_PlayerSpawn(client)
{
    adren_StopAdrenaline(client);
}

/**
 * Client has been damaged.
 *
 * @param victim        The index of the hurt client.
 * @param attacker      The index of the attacking client.
 * @param health        How much health the client has after the damage.
 * @param armor         How much armor the client has after the mirrordmg.
 * @param weapon        The weapon classname used to hurt the victim. (No weapon_ prefix)
 * @param dmg_health    The amount of health the victim lost.
 * @param dmg_armor     The amount of armor the victim lost.
 * @param hitgroup      The hitgroup index of the victim that was damaged.
 */
public adren_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    if(AreValidPlayers(victim,attacker)&&g_hPlayerIsAdrenalined[victim]==INVALID_HANDLE)
        if(AreValidTeams(victim,attacker,g_CoreConfigCache[CoreConfig_freeforall],g_adrenUpgradeConfigCache[UpgradeConfig_TeamLock]))
        {
            new level=GetPlayerUpgradeLevel(victim,g_moduleadren);
            if(level>0)
            {
                // Do not speed up if frozen
                if((!g_adrenUpgradeConfigCache[UpgradeConfig_Effects]
                &&    g_hFrostpPlayerIsFrozen[victim]==INVALID_HANDLE
                && g_hFrostnadePlayerIsFrozen[victim]==INVALID_HANDLE
                && g_hIcestabPlayerIsFrozen[victim] == INVALID_HANDLE)
                || g_adrenUpgradeConfigCache[UpgradeConfig_Effects])
                {
                    new Float:amount=GetModifiedSpeed(victim)+(level*g_adrenUpgradeConfigCache[UpgradeConfig_Percent]);
                    SetEntPropFloat(victim, Prop_Data, "m_flLaggedMovementValue", amount);
                    g_hPlayerIsAdrenalined[victim]=CreateTimer(g_adrenUpgradeConfigCache[UpgradeConfig_Interval],Timer_OnAdrenalineStop,victim,TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
                }
            }
        }
}

/**
 * Client has been killed.
 *
 * @param victim    The index of the killed client.
 * @param attacker  The killer of the victim.
 * @param weapon    The weapon classname used to kill the victim. (No weapon_ prefix)
 * @param headshot  True if the death was by headshot, false if not.
 */
public adren_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
    if(IsValidPlayer(victim))
        adren_StopAdrenaline(victim);
}

/**
* Client has purchased an Upgrade
*/
public adren_OnUpgradePurchase(client, level)
{
    if(IsValidPlayer(client))
        adren_StopAdrenaline(client);
}

/**
* Client has sold an Upgrade
*/
public adren_OnUpgradeSell(client, level)
{
    if(IsValidPlayer(client))
        adren_StopAdrenaline(client);
}

public Action:Timer_OnAdrenalineStop(Handle:timer,any:client)
{
    SetEntPropFloat(client, Prop_Data, "m_flLaggedMovementValue", GetModifiedSpeed(client));
    g_hPlayerIsAdrenalined[client]=INVALID_HANDLE;
    
    return Plugin_Stop;
}

public adren_StopAdrenaline(client)
{
    if(IsValidPlayer(client))
        if(GetClientTeam(client)!=g_adrenUpgradeConfigCache[UpgradeConfig_TeamLock])
            SetEntPropFloat(client, Prop_Data, "m_flLaggedMovementValue", GetModifiedSpeed(client));
            
    if(g_hPlayerIsAdrenalined[client]!=INVALID_HANDLE)
    {
        KillTimer(g_hPlayerIsAdrenalined[client]);
        g_hPlayerIsAdrenalined[client]=INVALID_HANDLE;
    }
}
