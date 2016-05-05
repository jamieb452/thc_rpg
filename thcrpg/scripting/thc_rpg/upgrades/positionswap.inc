/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          positionswap.inc
 *  Type:          Upgrade
 *  Description:   Gives you the chance to swap position with your attacker
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
new Module:g_moduleposswap;

new g_posswapUpgradeConfigCache[UpgradeConfigs];

new Float:g_fPosswapPlayerLocation[MAXPLAYERS][3];

/**
 * Register this module.
 */
posswap_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "posswap");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "PositionSwap");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the chance to swap position with your attacker");

    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;

    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;
    
    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 5;
    moduledata[ModuleData_Cost] = 20;
    moduledata[ModuleData_iCost] = 15;

    // Send this array of data to the module manager.
    g_moduleposswap = ModuleMgr_Register(moduledata);
    
    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleposswap, "Event_OnEventsRegister", "posswap_OnEventsRegister");
    #endif
        
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleposswap, "posswap_OnConfigReload", "configs/thc_rpg/positionswap.txt");
    #endif
}

/**
 * Register all events here.
 */
public posswap_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleposswap, "Event_OnMapStart",              "posswap_OnMapStart");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_moduleposswap, "Event_PlayerHurt",              "posswap_PlayerHurt");
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public posswap_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public posswap_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleposswap, LogType_Normal, "posswapConfigReload", "Reloaded posswap module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleposswap, CM_CONFIGINDEX_FIRST, "posswapModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:posswapModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "positionswap", false))
    {
        g_posswapUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_posswapUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_posswapUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_posswapUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_posswapUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");

        if(g_posswapUpgradeConfigCache[UpgradeConfig_Disable]==1)
          ModuleMgr_Disable(g_moduleposswap);

        ModuleMgr_WriteCell(g_moduleposswap, ModuleData_MaxLevel, g_posswapUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleposswap, ModuleData_Cost, g_posswapUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleposswap, ModuleData_iCost, g_posswapUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduleposswap);
}
#endif

/**
 * The map has started.
 */
public posswap_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleposswap, CM_CONFIGINDEX_FIRST, "posswapModule_ConfigCache");
    #endif
}

/**
 * Client has been damaged.
 * 
 * @param victim        The index of the hurt client.
 * @param attacker      The index of the attacking client.
 * @param health        How much posswap the client has after the damage.
 * @param armor         How much armor the client has after the damage.
 * @param weapon        The weapon classname used to hurt the victim. (No weapon_ prefix)
 * @param dmg_health    The amount of posswap the victim lost.
 * @param dmg_armor     The amount of armor the victim lost.
 * @param hitgroup      The hitgroup index of the victim that was damaged.
 */
public posswap_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    if(AreValidPlayers(victim,attacker))
    {
        if(AreValidTeams(victim,attacker,g_CoreConfigCache[CoreConfig_freeforall],g_posswapUpgradeConfigCache[UpgradeConfig_TeamLock]))
        {
            new level=GetPlayerUpgradeLevel(victim,g_moduleposswap);
            if(level>0)
                if(GetChance(level,g_posswapUpgradeConfigCache[UpgradeConfig_MaxLevel]))
                {
                    // Get the victims position
                    GetClientAbsOrigin(victim, g_fPosswapPlayerLocation[victim]);
                    // Get the attackers position
                    GetClientAbsOrigin(attacker, g_fPosswapPlayerLocation[attacker]);
                    // Teleport the players
                    TeleportEntity(victim, g_fPosswapPlayerLocation[attacker], NULL_VECTOR, NULL_VECTOR);
                    TeleportEntity(attacker, g_fPosswapPlayerLocation[victim], NULL_VECTOR, NULL_VECTOR);
                }
        }
    }
}
