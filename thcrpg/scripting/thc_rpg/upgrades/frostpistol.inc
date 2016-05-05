/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          frostpistol.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to freeze enemies by shooting them with a pistol
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

new g_frostpUpgradeConfigCache[UpgradeConfigs];

new Handle:g_hFrostpPlayerIsFrozen[MAXPLAYERS+1] = {INVALID_HANDLE,...};

/**
 * Register this module.
 */
frostp_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "frostp");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "FrostPistol");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to freeze enemies by shooting them with a pistol");

    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;

    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = g_modulefirep;
    interlocks[1] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;
    
    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 10;
    moduledata[ModuleData_Cost] = 20;
    moduledata[ModuleData_iCost] = 15;

    // Send this array of data to the module manager.
    g_modulefrostp = ModuleMgr_Register(moduledata);
 
    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_modulefrostp, "Event_OnEventsRegister", "frostp_OnEventsRegister");
    #endif
  
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_modulefrostp, "frostp_OnConfigReload", "configs/thc_rpg/frostpistol.txt");
    #endif

    for(new i=0;i<=MaxClients;i++)
        g_hFrostpPlayerIsFrozen[i]=INVALID_HANDLE;
}

/**
 * Register all events here.
 */
public frostp_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_modulefrostp, "Event_OnMapStart",                   "frostp_OnMapStart");
    EventMgr_RegisterEvent(g_modulefrostp, "Event_OnClientDisconnect",           "frostp_OnClientDisconnect");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_modulefrostp, "Event_PlayerSpawn",              "frostp_PlayerSpawn");
        EventMgr_RegisterEvent(g_modulefrostp, "Event_PlayerHurt",               "frostp_PlayerHurt");
        EventMgr_RegisterEvent(g_modulefrostp, "Event_PlayerDeath",              "frostp_PlayerDeath");
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public frostp_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public frostp_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulefrostp, LogType_Normal, "frostpConfigReload", "Reloaded frostp module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulefrostp, CM_CONFIGINDEX_FIRST, "frostpModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:frostpModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "frostpistol", false))
    {
        g_frostpUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_frostpUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_frostpUpgradeConfigCache[UpgradeConfig_Effects] = KvGetNum(kv, "effects");
        g_frostpUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_frostpUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_frostpUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_frostpUpgradeConfigCache[UpgradeConfig_BaseAmount] = KvGetFloat(kv, "baseamount");
        g_frostpUpgradeConfigCache[UpgradeConfig_Amount] = KvGetFloat(kv, "amount");
        g_frostpUpgradeConfigCache[UpgradeConfig_BaseSpeed] = KvGetFloat(kv, "basespeed");
        g_frostpUpgradeConfigCache[UpgradeConfig_Speed] = KvGetFloat(kv, "speed");
        g_frostpUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");
        g_frostpUpgradeConfigCache[UpgradeConfig_TotalFreeze] = KvGetNum(kv, "totalfreeze");

        if(g_frostpUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_modulefrostp);

        ModuleMgr_WriteCell(g_modulefrostp, ModuleData_TeamLock, g_frostpUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_modulefrostp, ModuleData_MaxLevel, g_frostpUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_modulefrostp, ModuleData_Cost, g_frostpUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_modulefrostp, ModuleData_iCost, g_frostpUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_modulefrostp);
}
#endif

/**
 * The map has started.
 */
public frostp_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulefrostp, CM_CONFIGINDEX_FIRST, "frostpModule_ConfigCache");
    #endif
}

/**
 * Client is disconnecting from the server.
 *
 * @param client    The client index.
 */
public frostp_OnClientDisconnect(client)
{
    if(g_hFrostpPlayerIsFrozen[client]!=INVALID_HANDLE)
    {
        KillTimer(g_hFrostpPlayerIsFrozen[client]);
        g_hFrostpPlayerIsFrozen[client]=INVALID_HANDLE;
    }
}

/**
 * Client has spawned.
 * 
 * @param client    The client index.
 * 
 */
public frostp_PlayerSpawn(client)
{
    if(g_hFrostpPlayerIsFrozen[client]!=INVALID_HANDLE)
    {
        KillTimer(g_hFrostpPlayerIsFrozen[client]);
        g_hFrostpPlayerIsFrozen[client]=INVALID_HANDLE;
        Freeze(client,false,g_frostpUpgradeConfigCache[UpgradeConfig_TotalFreeze],g_frostpUpgradeConfigCache[UpgradeConfig_Effects]);
    }
}

/**
 * Client has been damaged.
 * 
 * @param victim        The index of the hurt client.
 * @param attacker      The index of the attacking client.
 * @param health        How much frostp the client has after the damage.
 * @param armor         How much armor the client has after the damage.
 * @param weapon        The weapon classname used to hurt the victim. (No weapon_ prefix)
 * @param dmg_health    The amount of frostp the victim lost.
 * @param dmg_armor     The amount of armor the victim lost.
 * @param hitgroup      The hitgroup index of the victim that was damaged.
 */
public frostp_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    if(AreValidPlayers(victim,attacker))
        if(AreValidTeams(victim,attacker,g_CoreConfigCache[CoreConfig_freeforall],g_frostpUpgradeConfigCache[UpgradeConfig_TeamLock]))
        {
            new level=GetPlayerUpgradeLevel(attacker,g_modulefrostp);
            new weapon_ok=StrContains("glock usp p228 deagle fiveseven elite",weapon,false);
            if(level>0&&weapon_ok>-1)
                // Freeze the player
                if(g_hFrostpPlayerIsFrozen[victim]==INVALID_HANDLE)
                {
                    Freeze(victim,true,g_frostpUpgradeConfigCache[UpgradeConfig_TotalFreeze],g_frostpUpgradeConfigCache[UpgradeConfig_Effects], g_frostpUpgradeConfigCache[UpgradeConfig_BaseSpeed]-g_frostpUpgradeConfigCache[UpgradeConfig_Speed]*level);
                    g_hFrostpPlayerIsFrozen[victim]=CreateTimer(g_frostpUpgradeConfigCache[UpgradeConfig_BaseAmount]+g_frostpUpgradeConfigCache[UpgradeConfig_Amount]*level,Timer_OnFrostpDefrost,victim,TIMER_FLAG_NO_MAPCHANGE);
                }
                else // reduce the damage of victim
                {
                    new refund=RoundToNearest(dmg_health*g_frostpUpgradeConfigCache[UpgradeConfig_Percent]);
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
public frostp_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
    if(IsValidPlayer(victim)&&g_hFrostpPlayerIsFrozen[victim]!=INVALID_HANDLE)
    {
        KillTimer(g_hFrostpPlayerIsFrozen[victim]);
        g_hFrostpPlayerIsFrozen[victim]=INVALID_HANDLE;
        Freeze(victim,false,g_frostpUpgradeConfigCache[UpgradeConfig_TotalFreeze],g_frostpUpgradeConfigCache[UpgradeConfig_Effects]);
    }
}

public Action:Timer_OnFrostpDefrost(Handle:timer,any:client)
{
    Freeze(client,false,g_frostpUpgradeConfigCache[UpgradeConfig_TotalFreeze],g_frostpUpgradeConfigCache[UpgradeConfig_Effects]);
    g_hFrostpPlayerIsFrozen[client]=INVALID_HANDLE;
    
    return Plugin_Stop;
}
