/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          bouncybullets.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to push your enemies around by shooting them
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
new Module:g_modulebouncyb;

new g_bouncybUpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
bouncyb_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "bouncyb");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "BouncyBullets");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to push your enemies around by shooting them");

    new Module:dependencies[MM_DATA_DEPENDENCIES];
    dependencies[0] = g_moduleCore;
    dependencies[1] = INVALID_MODULE;
    moduledata[ModuleData_Dependencies] = dependencies;

    new Module:interlocks[MM_DATA_INTERLOCKS];
    interlocks[0] = INVALID_MODULE;
    moduledata[ModuleData_Interlocks] = interlocks;
    
    moduledata[ModuleData_TeamLock] = 0;
    moduledata[ModuleData_MaxLevel] = 5;
    moduledata[ModuleData_Cost] = 15;
    moduledata[ModuleData_iCost] = 20;

    // Send this array of data to the module manager.
    g_modulebouncyb = ModuleMgr_Register(moduledata);
 
    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_modulebouncyb, "Event_OnEventsRegister", "bouncyb_OnEventsRegister");
    #endif
  
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_modulebouncyb, "bouncyb_OnConfigReload", "configs/thc_rpg/bouncybullets.txt");
    #endif
}

/**
 * Register all events here.
 */
public bouncyb_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_modulebouncyb, "Event_OnMapStart",              "bouncyb_OnMapStart");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_modulebouncyb, "Event_PlayerHurt",              "bouncyb_PlayerHurt");
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public bouncyb_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public bouncyb_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulebouncyb, LogType_Normal, "bouncybConfigReload", "Reloaded bouncyb module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulebouncyb, CM_CONFIGINDEX_FIRST, "bouncybModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:bouncybModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "bouncybullets", false))
    {
        g_bouncybUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_bouncybUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_bouncybUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_bouncybUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_bouncybUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_bouncybUpgradeConfigCache[UpgradeConfig_Amount] = KvGetFloat(kv, "amount");

        if(g_bouncybUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_modulebouncyb);

        ModuleMgr_WriteCell(g_modulebouncyb, ModuleData_TeamLock, g_bouncybUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_modulebouncyb, ModuleData_MaxLevel, g_bouncybUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_modulebouncyb, ModuleData_Cost, g_bouncybUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_modulebouncyb, ModuleData_iCost, g_bouncybUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_modulebouncyb);
}
#endif

/**
 * The map has started.
 */
public bouncyb_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulebouncyb, CM_CONFIGINDEX_FIRST, "bouncybModule_ConfigCache");
    #endif
}

/**
 * Client has been damaged.
 * 
 * @param victim        The index of the hurt client.
 * @param attacker      The index of the attacking client.
 * @param health        How much bouncyb the client has after the damage.
 * @param armor         How much armor the client has after the damage.
 * @param weapon        The weapon classname used to hurt the victim. (No weapon_ prefix)
 * @param dmg_health    The amount of health the victim lost.
 * @param dmg_armor     The amount of armor the victim lost.
 * @param hitgroup      The hitgroup index of the victim that was damaged.
 */
public bouncyb_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
    if(AreValidPlayers(victim,attacker))
        if(AreValidTeams(victim,attacker,g_CoreConfigCache[CoreConfig_freeforall],g_bouncybUpgradeConfigCache[UpgradeConfig_TeamLock]))
        {
            new level=GetPlayerUpgradeLevel(attacker,g_modulebouncyb);
            new weapon_ok=StrContains("knife",weapon,false);
            if(level>0&&weapon_ok==-1)
            {
                new Float:victimloc[3],Float:attackerloc[3],Float:fv[3],Float:velocity[3];
                GetClientAbsOrigin(victim,victimloc);
                GetClientAbsOrigin(attacker,attackerloc);
                CreateVecFromPoints(attackerloc,victimloc,fv);
                Normalize(fv,velocity);
                velocity[0]*=level*g_bouncybUpgradeConfigCache[UpgradeConfig_Amount];
                velocity[1]*=level*g_bouncybUpgradeConfigCache[UpgradeConfig_Amount];
                velocity[2]*=level*g_bouncybUpgradeConfigCache[UpgradeConfig_Amount];
                SetVelocityVector(victim,velocity);
            }
        }
}
