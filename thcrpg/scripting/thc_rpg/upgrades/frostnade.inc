/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          frostnade.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to frost your enemies with a HE grenade 
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
// IS DEFINED IN PROJECT.INC

new g_frostnadeUpgradeConfigCache[UpgradeConfigs];

new Handle:g_hFrostnadePlayerIsFrozen[MAXPLAYERS+1] = {INVALID_HANDLE,...};

/**
 * Register this module.
 */
frostnade_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "frostnade");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "FrostNade");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to frost your enemies with a HE grenade");

    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;

    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = g_modulefirenade;
    interlocks[1] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;

    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 10;
    moduledata[ModuleData_Cost] = 10;
    moduledata[ModuleData_iCost] = 10;

    // Send this array of data to the module manager.
    g_modulefrostnade = ModuleMgr_Register(moduledata);
 
    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_modulefrostnade, "Event_OnEventsRegister", "frostnade_OnEventsRegister");
    #endif
  
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_modulefrostnade, "frostnade_OnConfigReload", "configs/thc_rpg/frostnade.txt");
    #endif
}

/**
 * Register all events here.
 */
public frostnade_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_modulefrostnade, "Event_OnMapStart",               "frostnade_OnMapStart");
    EventMgr_RegisterEvent(g_modulefrostnade, "Event_OnClientDisconnect",       "frostnade_OnClientDisconnect");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_modulefrostnade, "Event_PlayerSpawn",          "frostnade_PlayerSpawn");
        EventMgr_RegisterEvent(g_modulefrostnade, "Event_PlayerHurt",           "frostnade_PlayerHurt");
        EventMgr_RegisterEvent(g_modulefrostnade, "Event_PlayerDeath",          "frostnade_PlayerDeath");
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public frostnade_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public frostnade_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulefrostnade, LogType_Normal, "frostnadeConfigReload", "Reloaded frostnade module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulefrostnade, CM_CONFIGINDEX_FIRST, "frostnadeModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:frostnadeModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "frostnade", false))
    {
        g_frostnadeUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_frostnadeUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_frostnadeUpgradeConfigCache[UpgradeConfig_Effects] = KvGetNum(kv, "effects");
        g_frostnadeUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_frostnadeUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_frostnadeUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_frostnadeUpgradeConfigCache[UpgradeConfig_BaseAmount] = KvGetFloat(kv, "baseamount");
        g_frostnadeUpgradeConfigCache[UpgradeConfig_Amount] = KvGetFloat(kv, "amount");
        g_frostnadeUpgradeConfigCache[UpgradeConfig_BaseSpeed] = KvGetFloat(kv, "basespeed");
        g_frostnadeUpgradeConfigCache[UpgradeConfig_Speed] = KvGetFloat(kv, "speed");
        g_frostnadeUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");
        g_frostnadeUpgradeConfigCache[UpgradeConfig_TotalFreeze] = KvGetNum(kv, "totalfreeze");

        if(g_frostnadeUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_modulefrostnade);

        ModuleMgr_WriteCell(g_modulefrostnade, ModuleData_TeamLock, g_frostnadeUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_modulefrostnade, ModuleData_MaxLevel, g_frostnadeUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_modulefrostnade, ModuleData_Cost, g_frostnadeUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_modulefrostnade, ModuleData_iCost, g_frostnadeUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_modulefrostnade);
}
#endif

/**
 * The map has started.
 */
public frostnade_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulefrostnade, CM_CONFIGINDEX_FIRST, "frostnadeModule_ConfigCache");
    #endif
}

/**
 * Client is disconnecting from the server.
 *
 * @param client    The client index.
 */
public frostnade_OnClientDisconnect(client)
{
    if(g_hFrostnadePlayerIsFrozen[client]!=INVALID_HANDLE)
    {
        KillTimer(g_hFrostnadePlayerIsFrozen[client]);
        g_hFrostnadePlayerIsFrozen[client]=INVALID_HANDLE;
    }
}

/**
 * Client has spawned.
 *
 * @param client    The client index.
 *
 */
public frostnade_PlayerSpawn(client)
{
    if(IsValidPlayer(client)&&g_hFrostnadePlayerIsFrozen[client]!=INVALID_HANDLE)
    {
        KillTimer(g_hFrostnadePlayerIsFrozen[client]);
        g_hFrostnadePlayerIsFrozen[client]=INVALID_HANDLE;
        Freeze(client,false,g_frostnadeUpgradeConfigCache[UpgradeConfig_TotalFreeze],g_frostnadeUpgradeConfigCache[UpgradeConfig_Effects]);
    }
}

/**
 * Client has been damaged.
 * 
 * @param victim        The index of the hurt client.
 * @param attacker      The index of the attacking client.
 * @param health        How much health the client has after the damage.
 * @param armor         How much armor the client has after the damage.
 * @param weapon        The weapon classname used to hurt the victim. (No weapon_ prefix)
 * @param dmg_health    The amount of frostnade the victim lost.
 * @param dmg_armor     The amount of armor the victim lost.
 * @param hitgroup      The hitgroup index of the victim that was damaged.
 */
public frostnade_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    if(AreValidPlayers(victim,attacker))
        if(AreValidTeams(victim,attacker,g_CoreConfigCache[CoreConfig_freeforall],g_frostnadeUpgradeConfigCache[UpgradeConfig_TeamLock]))
        {
            new level=GetPlayerUpgradeLevel(attacker,g_modulefrostnade);
            new weapon_ok=StrContains("hegrenade",weapon,false);
            if(level>0&&weapon_ok>-1)
                // Freeze the player
                if(g_hFrostnadePlayerIsFrozen[victim]==INVALID_HANDLE)
                {
                    Freeze(victim,true,g_frostnadeUpgradeConfigCache[UpgradeConfig_TotalFreeze],g_frostnadeUpgradeConfigCache[UpgradeConfig_Effects], g_frostnadeUpgradeConfigCache[UpgradeConfig_BaseSpeed]-g_frostnadeUpgradeConfigCache[UpgradeConfig_Speed]*level);
                    g_hFrostnadePlayerIsFrozen[victim]=CreateTimer(g_frostnadeUpgradeConfigCache[UpgradeConfig_Amount]*level,Timer_OnFrostnadeDefrost,victim,TIMER_FLAG_NO_MAPCHANGE);
                }
                else // reduce the damage of victim
                {
                    new refund=RoundToNearest(dmg_health*g_frostnadeUpgradeConfigCache[UpgradeConfig_Percent]);
                    SetEntityHealth(victim, health+refund);
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
public frostnade_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
    if(IsValidPlayer(victim)&&g_hFrostnadePlayerIsFrozen[victim]!=INVALID_HANDLE)
    {
        KillTimer(g_hFrostnadePlayerIsFrozen[victim]);
        g_hFrostnadePlayerIsFrozen[victim]=INVALID_HANDLE;
        Freeze(victim,false,g_frostnadeUpgradeConfigCache[UpgradeConfig_TotalFreeze],g_frostnadeUpgradeConfigCache[UpgradeConfig_Effects]);
    }
}

public Action:Timer_OnFrostnadeDefrost(Handle:timer,any:client)
{
    Freeze(client,false,g_frostnadeUpgradeConfigCache[UpgradeConfig_TotalFreeze],g_frostnadeUpgradeConfigCache[UpgradeConfig_Effects]);
    g_hFrostnadePlayerIsFrozen[client]=INVALID_HANDLE;
    
    return Plugin_Stop;
}
