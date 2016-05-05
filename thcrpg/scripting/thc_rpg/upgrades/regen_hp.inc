/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          regen_hp.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to steadily regain HP until health is fully replenished
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
new Module:g_moduleregen_hp;

new g_regen_hpUpgradeConfigCache[UpgradeConfigs];

new Handle:g_hPlayerIsRegeneratingHP[MAXPLAYERS+1] = {INVALID_HANDLE,...};

/**
 * Register this module.
 */
regen_hp_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "regen_hp");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "RegenerateHP");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to steadily regain HP until health is fully replenished");

    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;

    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;

    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 5;
    moduledata[ModuleData_Cost] = 5;
    moduledata[ModuleData_iCost] = 10;

    // Send this array of data to the module manager.
    g_moduleregen_hp = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleregen_hp, "Event_OnEventsRegister", "regen_hp_OnEventsRegister");
    #endif  
  
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleregen_hp, "regen_hp_OnConfigReload", "configs/thc_rpg/regen_hp.txt");
    #endif
}

/**
 * Register all events here.
 */
public regen_hp_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleregen_hp, "Event_OnMapStart",             "regen_hp_OnMapStart");
    EventMgr_RegisterEvent(g_moduleregen_hp, "Event_OnClientDisconnect",     "regen_hp_OnClientDisconnect");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_moduleregen_hp, "Event_PlayerSpawn",            "regen_hp_PlayerSpawn");
        EventMgr_RegisterEvent(g_moduleregen_hp, "Event_PlayerHurt",             "regen_hp_PlayerHurt");
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public regen_hp_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public regen_hp_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleregen_hp, LogType_Normal, "regen_hpConfigReload", "Reloaded regen_hp module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleregen_hp, CM_CONFIGINDEX_FIRST, "regen_hpModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:regen_hpModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "regen_hp", false))
    {
        g_regen_hpUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_regen_hpUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_regen_hpUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_regen_hpUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_regen_hpUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_regen_hpUpgradeConfigCache[UpgradeConfig_Interval] = KvGetFloat(kv, "interval");
        g_regen_hpUpgradeConfigCache[UpgradeConfig_Amount] = KvGetFloat(kv, "amount");

        if(g_regen_hpUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_moduleregen_hp);

        ModuleMgr_WriteCell(g_moduleregen_hp, ModuleData_TeamLock, g_regen_hpUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_moduleregen_hp, ModuleData_MaxLevel, g_regen_hpUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleregen_hp, ModuleData_Cost, g_regen_hpUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleregen_hp, ModuleData_iCost, g_regen_hpUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduleregen_hp);
}
#endif

/**
 * The map has started.
 */
public regen_hp_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleregen_hp, CM_CONFIGINDEX_FIRST, "regen_hpModule_ConfigCache");
    #endif
}

/**
 * Client is disconnecting from the server.
 *
 * @param client    The client index.
 */
public regen_hp_OnClientDisconnect(client)
{
    if(g_hPlayerIsRegeneratingHP[client]!=INVALID_HANDLE)
    {
        KillTimer(g_hPlayerIsRegeneratingHP[client]);
        g_hPlayerIsRegeneratingHP[client]=INVALID_HANDLE;
    }
}

/**
 * Client has spawned.
 * 
 * @param client    The client index.
 * 
 */
public regen_hp_PlayerSpawn(client)
{
    if(g_hPlayerIsRegeneratingHP[client]!=INVALID_HANDLE)
    {
        KillTimer(g_hPlayerIsRegeneratingHP[client]);
        g_hPlayerIsRegeneratingHP[client]=INVALID_HANDLE;
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
 
public regen_hp_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    if(victim)
        if(GetClientTeam(victim)!=g_regen_hpUpgradeConfigCache[UpgradeConfig_TeamLock]&&health<GetModifiedHealth(victim))
            if(GetPlayerUpgradeLevel(victim,g_moduleregen_hp)&&g_hPlayerIsRegeneratingHP[victim]==INVALID_HANDLE)
                g_hPlayerIsRegeneratingHP[victim] = CreateTimer(g_regen_hpUpgradeConfigCache[UpgradeConfig_Interval],Timer_OnRegen_hpInterval,victim,TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
}

public Action:Timer_OnRegen_hpInterval(Handle:timer,any:client)
{
    if(Util_IsClientInGame(client))
    {
        new newhp=GetClientHealth(client)+RoundToNearest(g_regen_hpUpgradeConfigCache[UpgradeConfig_Amount]*GetPlayerUpgradeLevel(client,g_moduleregen_hp));
        if(newhp>GetModifiedHealth(client))
        {
            KillTimer(g_hPlayerIsRegeneratingHP[client]);
            g_hPlayerIsRegeneratingHP[client]=INVALID_HANDLE;
            newhp=GetModifiedHealth(client);
        }
        SetEntityHealth(client,newhp);
        return Plugin_Continue;
    }

    g_hPlayerIsRegeneratingHP[client]=INVALID_HANDLE;
    return Plugin_Stop;
}
