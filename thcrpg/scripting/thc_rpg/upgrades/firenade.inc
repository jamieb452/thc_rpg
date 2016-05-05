/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          firenade.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to light your enemies on fire with a HE grenade
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

new g_firenadeUpgradeConfigCache[UpgradeConfigs];

new Handle:g_hFirenadePlayerIsOnFire[MAXPLAYERS+1] = {INVALID_HANDLE,...};

/**
 * Register this module.
 */
firenade_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "firenade");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "FireNade");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to light your enemies on fire with a HE grenade");

    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;

    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = g_modulefrostnade;
    interlocks[1] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;

    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 10;
    moduledata[ModuleData_Cost] = 10;
    moduledata[ModuleData_iCost] = 10;

    // Send this array of data to the module manager.
    g_modulefirenade = ModuleMgr_Register(moduledata);
 
    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_modulefirenade, "Event_OnEventsRegister", "firenade_OnEventsRegister");
    #endif
  
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_modulefirenade, "firenade_OnConfigReload", "configs/thc_rpg/firenade.txt");
    #endif
}

/**
 * Register all events here.
 */
public firenade_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_modulefirenade, "Event_OnMapStart",                "firenade_OnMapStart");
    EventMgr_RegisterEvent(g_modulefirenade, "Event_OnClientDisconnect",        "firenade_OnClientDisconnect");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_modulefirenade, "Event_PlayerSpawn",           "firenade_PlayerSpawn");
        EventMgr_RegisterEvent(g_modulefirenade, "Event_PlayerHurt",            "firenade_PlayerHurt");
        EventMgr_RegisterEvent(g_modulefirenade, "Event_PlayerDeath",           "firenade_PlayerDeath");
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public firenade_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public firenade_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulefirenade, LogType_Normal, "firenadeConfigReload", "Reloaded firenade module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulefirenade, CM_CONFIGINDEX_FIRST, "firenadeModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:firenadeModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "firenade", false))
    {
        g_firenadeUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_firenadeUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_firenadeUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_firenadeUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_firenadeUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_firenadeUpgradeConfigCache[UpgradeConfig_Amount] = KvGetFloat(kv, "amount");

        if(g_firenadeUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_modulefirenade);

        ModuleMgr_WriteCell(g_modulefirenade, ModuleData_TeamLock, g_firenadeUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_modulefirenade, ModuleData_MaxLevel, g_firenadeUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_modulefirenade, ModuleData_Cost, g_firenadeUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_modulefirenade, ModuleData_iCost, g_firenadeUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_modulefirenade);
}
#endif

/**
 * The map has started.
 */
public firenade_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulefirenade, CM_CONFIGINDEX_FIRST, "firenadeModule_ConfigCache");
    #endif
}

/**
 * Client is disconnecting from the server.
 *
 * @param client    The client index.
 */
public firenade_OnClientDisconnect(client)
{
    if(g_hFirenadePlayerIsOnFire[client]!=INVALID_HANDLE)
    {
        KillTimer(g_hFirenadePlayerIsOnFire[client]);
        g_hFirenadePlayerIsOnFire[client] = INVALID_HANDLE;
    }
}

/**
* Client has spawned.
*
* @param client    The client index.
*
*/
public firenade_PlayerSpawn(client)
{
    if(g_hFirenadePlayerIsOnFire[client]!=INVALID_HANDLE)
    {
        KillTimer(g_hFirenadePlayerIsOnFire[client]);
        g_hFirenadePlayerIsOnFire[client]=INVALID_HANDLE;
        ExtinguishEntity(client);
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
public firenade_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    if(AreValidPlayers(victim,attacker))
        if(AreValidTeams(victim,attacker,g_CoreConfigCache[CoreConfig_freeforall],g_firenadeUpgradeConfigCache[UpgradeConfig_TeamLock]))
        {
            new level=GetPlayerUpgradeLevel(attacker,g_modulefirenade);
            if(level>0&&(StrEqual(weapon,"hegrenade",false)==true))
            {
                // Extinguish the entity and ignite it again;)
                if(g_hFirenadePlayerIsOnFire[victim] != INVALID_HANDLE)
                {
                    KillTimer(g_hFirenadePlayerIsOnFire[victim]);
                    g_hFirenadePlayerIsOnFire[victim] = INVALID_HANDLE;

                    ExtinguishEntity(victim);
                }

                if(health<1)
                {
                    SetEntityHealth(victim, 0);
                }
                else
                {
                    IgniteEntity(victim,level*g_firenadeUpgradeConfigCache[UpgradeConfig_Amount],false,10.0,false);
                    g_hFirenadePlayerIsOnFire[victim] = CreateTimer(level*g_firenadeUpgradeConfigCache[UpgradeConfig_Amount], Timer_OnFirenExtinguish, victim, TIMER_FLAG_NO_MAPCHANGE);
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
public firenade_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
    if(IsValidPlayer(victim)&&g_hFirenadePlayerIsOnFire[victim]!=INVALID_HANDLE)
    {
        ExtinguishEntity(victim);
        KillTimer(g_hFirenadePlayerIsOnFire[victim]);
    	g_hFirenadePlayerIsOnFire[victim] = INVALID_HANDLE;
    }
}

public Action:Timer_OnFirenExtinguish(Handle:timer, any:client)
{
	ExtinguishEntity(client);
	g_hFirenadePlayerIsOnFire[client] = INVALID_HANDLE;
	
	return Plugin_Stop;
}
