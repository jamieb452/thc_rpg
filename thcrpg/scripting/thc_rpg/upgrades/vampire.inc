/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          vampire.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to receive a fraction of the damage done to enemies
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
new Module:g_modulevampire;

new g_vampireUpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
vampire_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];

    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "vampire");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "Vampire");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to receive a fraction of the damage done to enemies");
    
    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;

    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;
    
    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 10;
    moduledata[ModuleData_Cost] = 15;
    moduledata[ModuleData_iCost] = 10;

    // Send this array of data to the module manager.
    g_modulevampire = ModuleMgr_Register(moduledata);
        
    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_modulevampire, "Event_OnEventsRegister", "vampire_OnEventsRegister");
    #endif
    
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_modulevampire, "vampire_OnConfigReload", "configs/thc_rpg/vampire.txt");
    #endif
}

/**
 * Register all events here.
 */
public vampire_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_modulevampire, "Event_OnMapStart",              "vampire_OnMapStart");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_modulevampire, "Event_PlayerHurt",              "vampire_PlayerHurt");
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public vampire_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public vampire_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulevampire, LogType_Normal, "vampireConfigReload", "Reloaded vampire module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulevampire, CM_CONFIGINDEX_FIRST, "vampireModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:vampireModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "vampire", false))
    {
        g_vampireUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_vampireUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_vampireUpgradeConfigCache[UpgradeConfig_Effects] = KvGetNum(kv, "effects");
        g_vampireUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_vampireUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_vampireUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_vampireUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");
        g_vampireUpgradeConfigCache[UpgradeConfig_Unlimited] = KvGetNum(kv, "unlimited");
        KvGetString(kv, "sound", g_vampireUpgradeConfigCache[UpgradeConfig_Sound], sizeof(g_vampireUpgradeConfigCache[UpgradeConfig_Sound]));

        if(g_vampireUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_modulevampire);

        ModuleMgr_WriteCell(g_modulevampire, ModuleData_TeamLock, g_vampireUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_modulevampire, ModuleData_MaxLevel, g_vampireUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_modulevampire, ModuleData_Cost, g_vampireUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_modulevampire, ModuleData_iCost, g_vampireUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_modulevampire);
}
#endif

/**
 * The map has started.
 */
public vampire_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulevampire, CM_CONFIGINDEX_FIRST, "vampireModule_ConfigCache");
    #endif
}

/**
 * Client has been damaged.
 * 
 * @param victim        The index of the hurt client.
 * @param attacker      The index of the attacking client.
 * @param health        How much vampire the client has after the damage.
 * @param armor         How much armor the client has after the damage.
 * @param weapon        The weapon classname used to hurt the victim. (No weapon_ prefix)
 * @param dmg_health    The amount of vampire the victim lost.
 * @param dmg_armor     The amount of armor the victim lost.
 * @param hitgroup      The hitgroup index of the victim that was damaged.
 */
public vampire_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    if(AreValidPlayers(victim,attacker))
        if(AreValidTeams(victim,attacker,g_CoreConfigCache[CoreConfig_freeforall],g_vampireUpgradeConfigCache[UpgradeConfig_TeamLock]))
        {
            new level=GetPlayerUpgradeLevel(attacker,g_modulevampire);
            if(level>0)
            {
                new amount=GetClientHealth(attacker)+RoundToNearest(dmg_health*level*g_vampireUpgradeConfigCache[UpgradeConfig_Percent]);
                if(amount>GetModifiedHealth(attacker)&&g_vampireUpgradeConfigCache[UpgradeConfig_Unlimited]==0)
					amount=GetModifiedHealth(attacker);
                SetEntityHealth(attacker,amount);

                // Make red beam effect and play life sleeching sound
                if(g_vampireUpgradeConfigCache[UpgradeConfig_Effects])
                {
                    new color[4]={255,0,0,127};
                    new Float:victimloc[3],Float:attackerloc[3];
                    GetClientAbsOrigin(victim,victimloc);
                    GetClientAbsOrigin(attacker,attackerloc);
                    victimloc[2]+=60;
                    attackerloc[2]+=60;
                    BeamEffect("@all",attackerloc,victimloc,0.5,1.0,10.0,color,0.0,0);
                    if(!StrEqual(g_vampireUpgradeConfigCache[UpgradeConfig_Sound], "off", false))
                    {
                        ClientCommand(victim,"play %s", g_vampireUpgradeConfigCache[UpgradeConfig_Sound]);
                        ClientCommand(attacker,"play %s", g_vampireUpgradeConfigCache[UpgradeConfig_Sound]);
                    }
                }
            }
        }
}
