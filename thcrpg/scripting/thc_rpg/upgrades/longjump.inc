/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          longjump.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to jump a longer distance
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
new Module:g_modulelongjump;

new g_longjumpUpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
longjump_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "longjump");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "LongJump");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to jump a longer distance");

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
    g_modulelongjump = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_modulelongjump, "Event_OnEventsRegister", "longjump_OnEventsRegister");
    #endif
    
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_modulelongjump, "longjump_OnConfigReload", "configs/thc_rpg/longjump.txt");
    #endif
}

/**
 * Register all events here.
 */
public longjump_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_modulelongjump, "Event_OnMapStart",             "longjump_OnMapStart");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_modulelongjump, "Event_PlayerJump",             "longjump_PlayerJump");
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public longjump_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public longjump_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulelongjump, LogType_Normal, "longjumpConfigReload", "Reloaded longjump module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulelongjump, CM_CONFIGINDEX_FIRST, "longjumpModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:longjumpModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "longjump", false))
    {
        g_longjumpUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_longjumpUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_longjumpUpgradeConfigCache[UpgradeConfig_Effects] = KvGetNum(kv, "effects");
        g_longjumpUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_longjumpUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_longjumpUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_longjumpUpgradeConfigCache[UpgradeConfig_Percent] = KvGetFloat(kv, "percent");

        if(g_longjumpUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_modulelongjump);

        ModuleMgr_WriteCell(g_modulelongjump, ModuleData_TeamLock, g_longjumpUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_modulelongjump, ModuleData_MaxLevel, g_longjumpUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_modulelongjump, ModuleData_Cost, g_longjumpUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_modulelongjump, ModuleData_iCost, g_longjumpUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_modulelongjump);
}
#endif

/**
 * The map has started.
 */
public longjump_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulelongjump, CM_CONFIGINDEX_FIRST, "longjumpModule_ConfigCache");
    #endif
}

/**
 * Client has jumped.
 * 
 * @param client        The client index.
 */
public longjump_PlayerJump(client)
{
    if(IsValidPlayer(client))
        if(GetClientTeam(client)!=g_longjumpUpgradeConfigCache[UpgradeConfig_TeamLock])
        {
            new level=GetPlayerUpgradeLevel(client,g_modulelongjump);
            if(level>0)
            {
                new Float:velocity[3]={0.0,0.0,0.0};
                GetHorizontalVelocity(client,velocity);
                velocity[0]+=velocity[0]*level*g_longjumpUpgradeConfigCache[UpgradeConfig_Percent];
                velocity[1]+=velocity[1]*level*g_longjumpUpgradeConfigCache[UpgradeConfig_Percent];
                SetVelocityVector(client,velocity);

                // Make fire-colored beam that follows the player
                if(g_longjumpUpgradeConfigCache[UpgradeConfig_Effects])
                {
                    new color[4]={255,127,0,127};
                    BeamFollowEffect("@all",client,0.3,1.0,1.0,color);
                }
            }
        }
}
