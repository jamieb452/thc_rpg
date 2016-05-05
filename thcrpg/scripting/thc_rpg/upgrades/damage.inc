/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          damage.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to give extra damage to your opponent
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
new Module:g_moduledamage;

new g_damageUpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
damage_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "damage");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "Damage+");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to give extra damage to your opponent");

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
    g_moduledamage = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduledamage, "Event_OnEventsRegister", "damage_OnEventsRegister");
    #endif  
  
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduledamage, "damage_OnConfigReload", "configs/thc_rpg/damage.txt");
    #endif
}

/**
 * Register all events here.
 */
public damage_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduledamage, "Event_OnMapStart",             "damage_OnMapStart");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_moduledamage, "Event_PlayerHurt",             "damage_PlayerHurt");
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public damage_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public damage_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduledamage, LogType_Normal, "damageConfigReload", "Reloaded damage module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduledamage, CM_CONFIGINDEX_FIRST, "damageModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:damageModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "damage", false))
    {
        g_damageUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_damageUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_damageUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_damageUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_damageUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_damageUpgradeConfigCache[UpgradeConfig_Mode] = KvGetNum(kv, "mode");
        g_damageUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");

        if(g_damageUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_moduledamage);

        ModuleMgr_WriteCell(g_moduledamage, ModuleData_TeamLock, g_damageUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_moduledamage, ModuleData_MaxLevel, g_damageUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduledamage, ModuleData_Cost, g_damageUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduledamage, ModuleData_iCost, g_damageUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduledamage);
}
#endif

/**
 * The map has started.
 */
public damage_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduledamage, CM_CONFIGINDEX_FIRST, "damageModule_ConfigCache");
    #endif
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
public damage_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    if(AreValidPlayers(victim,attacker))
        if(AreValidTeams(victim,attacker,g_CoreConfigCache[CoreConfig_freeforall],g_damageUpgradeConfigCache[UpgradeConfig_TeamLock]))
        {
            new level=GetPlayerUpgradeLevel(attacker,g_moduledamage);
            if(level>0)
            {
                new amount=health;
                if(g_damageUpgradeConfigCache[UpgradeConfig_Mode]==1)
                    amount-=RoundToNearest(level*g_damageUpgradeConfigCache[UpgradeConfig_Percent]);
                else
                    amount-=RoundToNearest(dmg_health*level*g_damageUpgradeConfigCache[UpgradeConfig_Percent]);
                if(amount<1)
                    amount=0;
                SetEntityHealth(victim, amount);
            }
        }
}
