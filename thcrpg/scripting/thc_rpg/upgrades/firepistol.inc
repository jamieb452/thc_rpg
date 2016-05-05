/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          firepistol.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to light your enemy on fire by shooting him with a pistol
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

new g_firepUpgradeConfigCache[UpgradeConfigs];

new Handle:g_hFirepPlayerIsOnFire[MAXPLAYERS+1] = {INVALID_HANDLE,...};

/**
 * Register this module.
 */
firep_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "firep");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "FirePistol");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to light your enemy on fire by shooting him with a pistol");

    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;

    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = g_modulefrostp;
    interlocks[1] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;
    
    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 10;
    moduledata[ModuleData_Cost] = 10;
    moduledata[ModuleData_iCost] = 10;

    // Send this array of data to the module manager.
    g_modulefirep = ModuleMgr_Register(moduledata);
 
    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_modulefirep, "Event_OnEventsRegister", "firep_OnEventsRegister");
    #endif
  
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_modulefirep, "firep_OnConfigReload", "configs/thc_rpg/firepistol.txt");
    #endif
}

/**
 * Register all events here.
 */
public firep_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_modulefirep, "Event_OnMapStart",                   "firep_OnMapStart");
    EventMgr_RegisterEvent(g_modulefirep, "Event_OnClientDisconnect",           "firep_OnClientDisconnect");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_modulefirep, "Event_PlayerSpawn",              "firep_PlayerSpawn");
        EventMgr_RegisterEvent(g_modulefirep, "Event_PlayerHurt",               "firep_PlayerHurt");
        EventMgr_RegisterEvent(g_modulefirep, "Event_PlayerDeath",              "firep_PlayerDeath");
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public firep_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public firep_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulefirep, LogType_Normal, "firepConfigReload", "Reloaded firep module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulefirep, CM_CONFIGINDEX_FIRST, "firepModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:firepModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "firepistol", false))
    {
        g_firepUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_firepUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_firepUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_firepUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_firepUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_firepUpgradeConfigCache[UpgradeConfig_Amount] = KvGetFloat(kv, "amount");

        if(g_firepUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_modulefirep);

        ModuleMgr_WriteCell(g_modulefirep, ModuleData_TeamLock, g_firepUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_modulefirep, ModuleData_MaxLevel, g_firepUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_modulefirep, ModuleData_Cost, g_firepUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_modulefirep, ModuleData_iCost, g_firepUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_modulefirep);
}
#endif

/**
 * The map has started.
 */
public firep_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulefirep, CM_CONFIGINDEX_FIRST, "firepModule_ConfigCache");
    #endif
}

/**
 * Client is disconnecting from the server.
 *
 * @param client    The client index.
 */
public firep_OnClientDisconnect(client)
{
    if(g_hFirepPlayerIsOnFire[client] != INVALID_HANDLE)
	{
		KillTimer(g_hFirepPlayerIsOnFire[client]);
		g_hFirepPlayerIsOnFire[client] = INVALID_HANDLE;
	}
}

/**
* Client has spawned.
*
* @param client    The client index.
*
*/
public firep_PlayerSpawn(client)
{
    if(IsValidPlayer(client)&&g_hFirepPlayerIsOnFire[client]!=INVALID_HANDLE)
	{
		KillTimer(g_hFirepPlayerIsOnFire[client]);
		g_hFirepPlayerIsOnFire[client]=INVALID_HANDLE;
		ExtinguishClient(client);
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
public firep_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    if(AreValidPlayers(victim,attacker))
        if(AreValidTeams(victim,attacker,g_CoreConfigCache[CoreConfig_freeforall],g_firepUpgradeConfigCache[UpgradeConfig_TeamLock]))
        {
            new level=GetPlayerUpgradeLevel(attacker,g_modulefirep);
            new weapon_ok=StrContains("glock usp p228 deagle fiveseven elite",weapon,false);
            if(level>0&&weapon_ok>-1)
            {
                // Extinguish the entity and ignite it again;)
                if(g_hFirepPlayerIsOnFire[victim] != INVALID_HANDLE)
                {
                    KillTimer(g_hFirepPlayerIsOnFire[victim]);
                    g_hFirepPlayerIsOnFire[victim] = INVALID_HANDLE;

                    ExtinguishClient(victim);
                }
                
                if(health<1)
                {
                    SetEntityHealth(victim, 0);
                }
                else
                {
                    IgniteEntity(victim,level*g_firepUpgradeConfigCache[UpgradeConfig_Amount],false,10.0,false);
                    g_hFirepPlayerIsOnFire[victim] = CreateTimer(level*g_firepUpgradeConfigCache[UpgradeConfig_Amount], Timer_OnFirepExtinguish, victim, TIMER_FLAG_NO_MAPCHANGE);
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
public firep_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
    if(IsValidPlayer(victim)&&g_hFirepPlayerIsOnFire[victim]!=INVALID_HANDLE)
    {
        ExtinguishClient(victim);
        KillTimer(g_hFirepPlayerIsOnFire[victim]);
        g_hFirepPlayerIsOnFire[victim] = INVALID_HANDLE;
    }
}

public Action:Timer_OnFirepExtinguish(Handle:timer, any:client)
{
	ExtinguishClient(client);
	g_hFirepPlayerIsOnFire[client] = INVALID_HANDLE;
	
	return Plugin_Stop;
}
