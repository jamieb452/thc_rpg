/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          icestab.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to freeze your enemy by stabbing him
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
new Module:g_moduleicestab;

new g_icestabUpgradeConfigCache[UpgradeConfigs];

new Handle:g_hIcestabPlayerIsFrozen[MAXPLAYERS+1] = {INVALID_HANDLE,...};

new g_iIcestabAttacker[MAXPLAYERS+1];

/**
 * Register this module.
 */
icestab_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "icestab");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "IceStab");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to freeze your enemy by stabbing him");

    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;

    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;
    
    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 3;
    moduledata[ModuleData_Cost] = 20;
    moduledata[ModuleData_iCost] = 30;

    // Send this array of data to the module manager.
    g_moduleicestab = ModuleMgr_Register(moduledata);
 
    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleicestab, "Event_OnEventsRegister", "icestab_OnEventsRegister");
    #endif
 
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleicestab, "icestab_OnConfigReload", "configs/thc_rpg/icestab.txt");
    #endif
    
    for(new i=0;i<=MaxClients;i++)
        g_hIcestabPlayerIsFrozen[i]=INVALID_HANDLE;
}

/**
 * Register all events here.
 */
public icestab_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleicestab, "Event_OnMapStart",              "icestab_OnMapStart");
    EventMgr_RegisterEvent(g_moduleicestab, "Event_OnClientDisconnect",      "icestab_OnClientDisconnect");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_moduleicestab, "Event_PlayerSpawn",             "icestab_PlayerSpawn");
        EventMgr_RegisterEvent(g_moduleicestab, "Event_PlayerHurt",              "icestab_PlayerHurt");
        EventMgr_RegisterEvent(g_moduleicestab, "Event_PlayerDeath",             "icestab_PlayerDeath");
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public icestab_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public icestab_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleicestab, LogType_Normal, "icestabConfigReload", "Reloaded icestab module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleicestab, CM_CONFIGINDEX_FIRST, "icestabModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:icestabModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "icestab", false))
    {
        g_icestabUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_icestabUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_icestabUpgradeConfigCache[UpgradeConfig_Effects] = KvGetNum(kv, "effects");
        g_icestabUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_icestabUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_icestabUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_icestabUpgradeConfigCache[UpgradeConfig_BaseAmount] = KvGetFloat(kv, "baseamount");
        g_icestabUpgradeConfigCache[UpgradeConfig_Amount] = KvGetFloat(kv, "amount");
        g_icestabUpgradeConfigCache[UpgradeConfig_BaseSpeed] = KvGetFloat(kv, "basespeed");
        g_icestabUpgradeConfigCache[UpgradeConfig_Speed] = KvGetFloat(kv, "speed");
        g_icestabUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");
        g_icestabUpgradeConfigCache[UpgradeConfig_TotalFreeze] = KvGetNum(kv, "totalfreeze");

        if(g_icestabUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_moduleicestab);

        ModuleMgr_WriteCell(g_moduleicestab, ModuleData_TeamLock, g_icestabUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_moduleicestab, ModuleData_MaxLevel, g_icestabUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleicestab, ModuleData_Cost, g_icestabUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleicestab, ModuleData_iCost, g_icestabUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduleicestab);
}
#endif

/**
 * The map has started.
 */
public icestab_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleicestab, CM_CONFIGINDEX_FIRST, "icestabModule_ConfigCache");
    #endif
}

/**
 * Client is disconnecting from the server.
 *
 * @param client    The client index.
 */
public icestab_OnClientDisconnect(client)
{
    if(g_hIcestabPlayerIsFrozen[client]!=INVALID_HANDLE)
    {
        KillTimer(g_hIcestabPlayerIsFrozen[client]);
        g_hIcestabPlayerIsFrozen[client]=INVALID_HANDLE;
    }
}

/**
 * Client has spawned.
 * 
 * @param client    The client index.
 * 
 */
public icestab_PlayerSpawn(client)
{
    if(g_hIcestabPlayerIsFrozen[client]!=INVALID_HANDLE)
    {
        KillTimer(g_hIcestabPlayerIsFrozen[client]);
        g_hIcestabPlayerIsFrozen[client]=INVALID_HANDLE;
        Freeze(client,false,g_icestabUpgradeConfigCache[UpgradeConfig_TotalFreeze],g_icestabUpgradeConfigCache[UpgradeConfig_Effects]);
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
 * @param dmg_health    The amount of health the victim lost.
 * @param dmg_armor     The amount of armor the victim lost.
 * @param hitgroup      The hitgroup index of the victim that was damaged.
 */
public icestab_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    if(AreValidPlayers(victim,attacker)&&g_iIcestabAttacker[attacker]!=victim)
        if(AreValidTeams(victim,attacker,g_CoreConfigCache[CoreConfig_freeforall],g_icestabUpgradeConfigCache[UpgradeConfig_TeamLock]))
        {
            new level=GetPlayerUpgradeLevel(attacker,g_moduleicestab);
            if(level>0&&(StrEqual(weapon,"knife",false)==true))
            {
                g_iIcestabAttacker[victim]=attacker;
                // Freeze the player
                if(g_hIcestabPlayerIsFrozen[victim]==INVALID_HANDLE)
                {
                    Freeze(victim,true,g_icestabUpgradeConfigCache[UpgradeConfig_TotalFreeze],g_icestabUpgradeConfigCache[UpgradeConfig_Effects], g_icestabUpgradeConfigCache[UpgradeConfig_BaseSpeed]-g_icestabUpgradeConfigCache[UpgradeConfig_Speed]*level);
                    g_hIcestabPlayerIsFrozen[victim]=CreateTimer(level*g_icestabUpgradeConfigCache[UpgradeConfig_Amount],Timer_OnIcestabDefrost,victim,TIMER_FLAG_NO_MAPCHANGE);
                }
                else // reduce the damage of victim
                {
                    new refund=RoundToNearest(dmg_health*g_icestabUpgradeConfigCache[UpgradeConfig_Percent]);
                    SetEntityHealth(victim, health+refund);
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
public icestab_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
    if(IsValidPlayer(victim)&&g_hIcestabPlayerIsFrozen[victim]!=INVALID_HANDLE)
    {
        KillTimer(g_hIcestabPlayerIsFrozen[victim]);
        g_hIcestabPlayerIsFrozen[victim]=INVALID_HANDLE;
        Freeze(victim,false,g_icestabUpgradeConfigCache[UpgradeConfig_TotalFreeze],g_icestabUpgradeConfigCache[UpgradeConfig_Effects]);
        g_iIcestabAttacker[victim]=0;
    }
}

public Action:Timer_OnIcestabDefrost(Handle:timer,any:client)
{
    Freeze(client,false,g_icestabUpgradeConfigCache[UpgradeConfig_TotalFreeze],g_icestabUpgradeConfigCache[UpgradeConfig_Effects]);
    g_hIcestabPlayerIsFrozen[client]=INVALID_HANDLE;
    g_iIcestabAttacker[client]=0;
    
    return Plugin_Stop;
}
