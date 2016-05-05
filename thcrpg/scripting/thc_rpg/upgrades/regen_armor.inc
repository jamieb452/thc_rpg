/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          regen_armor.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to steadily regain armor until it is fully replenished
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
new Module:g_moduleregen_armor;

new g_regen_armorUpgradeConfigCache[UpgradeConfigs];

new bool:g_bPlayerIsRegeneratingArmor[MAXPLAYERS+1];

/**
 * Register this module.
 */
regen_armor_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "regen_armor");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "RegenerateArmor");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to steadily regain armor until it is fully replenished");

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
    g_moduleregen_armor = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleregen_armor, "Event_OnEventsRegister", "regen_armor_OnEventsRegister");
    #endif  
  
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleregen_armor, "regen_armor_OnConfigReload", "configs/thc_rpg/regen_armor.txt");
    #endif
}

/**
 * Register all events here.
 */
public regen_armor_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleregen_armor, "Event_OnMapStart",             "regen_armor_OnMapStart");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_moduleregen_armor, "Event_PlayerSpawn",            "regen_armor_PlayerSpawn");
        EventMgr_RegisterEvent(g_moduleregen_armor, "Event_PlayerDeath",            "regen_armor_PlayerDeath");
        EventMgr_RegisterEvent(g_moduleregen_armor, "Event_PlayerHurt",             "regen_armor_PlayerHurt");
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public regen_armor_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public regen_armor_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleregen_armor, LogType_Normal, "regen_armorConfigReload", "Reloaded regen_armor module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleregen_armor, CM_CONFIGINDEX_FIRST, "regen_armorModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:regen_armorModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "regen_armor", false))
    {
        g_regen_armorUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_regen_armorUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_regen_armorUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_regen_armorUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_regen_armorUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_regen_armorUpgradeConfigCache[UpgradeConfig_Interval] = KvGetFloat(kv, "interval");
        g_regen_armorUpgradeConfigCache[UpgradeConfig_Amount] = KvGetFloat(kv, "amount");

        if(g_regen_armorUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_moduleregen_armor);

        ModuleMgr_WriteCell(g_moduleregen_armor, ModuleData_TeamLock, g_regen_armorUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_moduleregen_armor, ModuleData_MaxLevel, g_regen_armorUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleregen_armor, ModuleData_Cost, g_regen_armorUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleregen_armor, ModuleData_iCost, g_regen_armorUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduleregen_armor);
}
#endif

/**
 * The map has started.
 */
public regen_armor_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleregen_armor, CM_CONFIGINDEX_FIRST, "regen_armorModule_ConfigCache");
    #endif
}
 
 /**
 * Client has spawned.
 * 
 * @param client    The client index.
 * 
 */
public regen_armor_PlayerSpawn(client)
{
	if(!g_bPlayerIsRegeneratingArmor[client] && GetClientArmor(client)<100)
	{
		g_bPlayerIsRegeneratingArmor[client]=true;
		CreateTimer(g_regen_armorUpgradeConfigCache[UpgradeConfig_Interval],Timer_OnRegen_armorInterval,client,TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
	}
}

/**
 * Client has died
 * 
 * @param client    The client index.
 * 
 */

public regen_armor_PlayerDeath(client)
{
    g_bPlayerIsRegeneratingArmor[client]=false;
}

/**
 * Client has been damaged.
 * 
 * @param victim        The index of the hurt client.
 * @param attacker      The index of the attacking client.
 * @param health        How much regen_armor the client has after the damage.
 * @param armor         How much armor the client has after the damage.
 * @param weapon        The weapon classname used to hurt the victim. (No weapon_ prefix)
 * @param dmg_health    The amount of regen_armor the victim lost.
 * @param dmg_armor     The amount of armor the victim lost.
 * @param hitgroup      The hitgroup index of the victim that was damaged.
 */
public regen_armor_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    if(victim)
        if(GetClientTeam(victim)!=g_regen_armorUpgradeConfigCache[UpgradeConfig_TeamLock])
            if(GetPlayerUpgradeLevel(victim,g_moduleregen_armor)&&!g_bPlayerIsRegeneratingArmor[victim])
            {
                g_bPlayerIsRegeneratingArmor[victim]=true;
                CreateTimer(g_regen_armorUpgradeConfigCache[UpgradeConfig_Interval],Timer_OnRegen_armorInterval,victim,TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
            }
}

public Action:Timer_OnRegen_armorInterval(Handle:timer,any:client)
{
    if(IsClientInGame(client)&&g_bPlayerIsRegeneratingArmor[client])
    {
        new amount=GetClientArmor(client)+RoundToNearest(g_regen_armorUpgradeConfigCache[UpgradeConfig_Amount]*GetPlayerUpgradeLevel(client,g_moduleregen_armor));
        if(amount>100)
        {
            g_bPlayerIsRegeneratingArmor[client]=false;
            amount=100;
        }
        SetEntData(client,g_iArmorOffset,amount,4,true);
        return Plugin_Continue;
    }

    return Plugin_Stop;
}
