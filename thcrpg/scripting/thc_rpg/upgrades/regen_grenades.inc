/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          regen_grenades.inc
 *  Type:          Upgrade
 *  Description:   Gives you the chance to regain grenades
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
new Module:g_moduleregen_gren;

new g_regen_grenUpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
regen_gren_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "regen_gren");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "RegenerateGrenade");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the chance to regain grenades");

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
    g_moduleregen_gren = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleregen_gren, "Event_OnEventsRegister", "regen_gren_OnEventsRegister");
    #endif  
  
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleregen_gren, "regen_gren_OnConfigReload", "configs/thc_rpg/regen_grenades.txt");
    #endif
}

/**
 * Register all events here.
 */
public regen_gren_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleregen_gren, "Event_OnMapStart",             "regen_gren_OnMapStart");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_moduleregen_gren, "Event_SmokegrenadeDetonated",   "regen_gren_SmokegrenDetonated");
        EventMgr_RegisterEvent(g_moduleregen_gren, "Event_HEGrenadeDetonated",      "regen_gren_HEGrenadeDetonated");
        EventMgr_RegisterEvent(g_moduleregen_gren, "Event_FlashBangDetonated",      "regen_gren_FlashBangDetonated");
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public regen_gren_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public regen_gren_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleregen_gren, LogType_Normal, "regen_grenConfigReload", "Reloaded regen_gren module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleregen_gren, CM_CONFIGINDEX_FIRST, "regen_grenModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:regen_grenModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "regen_grenades", false))
    {
        g_regen_grenUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_regen_grenUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_regen_grenUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_regen_grenUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_regen_grenUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");

        if(g_regen_grenUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_moduleregen_gren);

        ModuleMgr_WriteCell(g_moduleregen_gren, ModuleData_TeamLock, g_regen_grenUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_moduleregen_gren, ModuleData_MaxLevel, g_regen_grenUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleregen_gren, ModuleData_Cost, g_regen_grenUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleregen_gren, ModuleData_iCost, g_regen_grenUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduleregen_gren);
}
#endif

/**
 * The map has started.
 */
public regen_gren_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleregen_gren, CM_CONFIGINDEX_FIRST, "regen_grenModule_ConfigCache");
    #endif
}

/**
 * Weapon: weapon_hegrenade      Offset: 11  Count: 1
 * Weapon: weapon_flashbang      Offset: 12  Count: 1
 * Weapon: weapon_smokegrenade   Offset: 13  Count: 1
*/

public regen_gren_SmokegrenDetonated(thrower,Float:x,Float:y,Float:z)
{
    if(thrower)
        if(GetClientTeam(thrower)!=g_regen_grenUpgradeConfigCache[UpgradeConfig_TeamLock])
        {
            new level=GetPlayerUpgradeLevel(thrower,g_moduleregen_gren);
            if(level>0)
                if(GetChance(level,g_regen_grenUpgradeConfigCache[UpgradeConfig_MaxLevel]))
					GivePlayerItem(thrower, "weapon_smokegrenade");
        }
}

public regen_gren_HEGrenadeDetonated(thrower,Float:x,Float:y,Float:z)
{
    if(thrower)
        if(GetClientTeam(thrower)!=g_regen_grenUpgradeConfigCache[UpgradeConfig_TeamLock])
        {
            new level=GetPlayerUpgradeLevel(thrower,g_moduleregen_gren);
            if(level>0)
                if(GetChance(level,g_regen_grenUpgradeConfigCache[UpgradeConfig_MaxLevel]))
					GivePlayerItem(thrower, "weapon_hegrenade");
        }
}

public regen_gren_FlashBangDetonated(thrower,Float:x,Float:y,Float:z)
{
    if(thrower)
        if(GetClientTeam(thrower)!=g_regen_grenUpgradeConfigCache[UpgradeConfig_TeamLock])
        {
            new level=GetPlayerUpgradeLevel(thrower,g_moduleregen_gren);
            if(level>0)
                if(GetChance(level,g_regen_grenUpgradeConfigCache[UpgradeConfig_MaxLevel]))
					GivePlayerItem(thrower, "weapon_flashbang");
        }
}
