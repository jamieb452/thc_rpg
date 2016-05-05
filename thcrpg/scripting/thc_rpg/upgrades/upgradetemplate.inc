/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          <upgradename>.inc
 *  Type:          Upgrade
 *  Description:   <description>
 *  Author:        <author>
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
new Module:g_module<upgradeshortname>;

new g_<upgradeshortname>UpgradeConfigCache[UpgradeConfigs];

/**
 * Register this module.
 */
<upgradeshortname>_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "<upgradeshortname>");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "<upgradename>");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "<description>");

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
    g_module<upgradeshortname> = ModuleMgr_Register(moduledata);
    
    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_OnEventsRegister", "<upgradeshortname>_OnEventsRegister");
    #endif
        
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_module<upgradeshortname>, "<upgradeshortname>_OnConfigReload", "configs/thc_rpg/<upgradeshortname>.txt");
    #endif
}

/**
 * Register all events here.
 */
public <upgradeshortname>_OnEventsRegister()
{
    // Register all the events needed for this module.
    //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_OnAllPluginsLoaded",      "<upgradeshortname>_OnAllPluginsLoaded");
    //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_OnPluginEnd",             "<upgradeshortname>_OnPluginEnd");
    //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_OnAllModulesLoaded",      "<upgradeshortname>_OnAllModulesLoaded");
    //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_OnModuleEnable",          "<upgradeshortname>_OnModuleEnable");
    //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_OnMyModuleEnable",        "<upgradeshortname>_OnMyModuleEnable");
    //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_OnModuleDisable",         "<upgradeshortname>_OnModuleDisable");
    //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_OnMyModuleDisable",       "<upgradeshortname>_OnMyModuleDisable");
    EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_OnMapStart",              "<upgradeshortname>_OnMapStart");
    //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_OnMapEnd",                "<upgradeshortname>_OnMapEnd");
    //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_OnAutoConfigsBuffered",   "<upgradeshortname>_OnAutoConfigsBuffered");
    //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_OnConfigsExecuted",       "<upgradeshortname>_OnConfigsExecuted");
    //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_OnClientPutInServer",     "<upgradeshortname>_OnClientPutInServer");
    //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_OnClientDisconnect",      "<upgradeshortname>_OnClientDisconnect");
    #if defined PROJECT_GAME_CSS
        //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_RoundStart",              "<upgradeshortname>_RoundStart");
        //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_RoundFreezeEnd",          "<upgradeshortname>_RoundFreezeEnd");
        //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_RoundEnd",                "<upgradeshortname>_RoundEnd");
        //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_PlayerTeam",              "<upgradeshortname>_PlayerTeam");
        //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_PlayerSpawn",             "<upgradeshortname>_PlayerSpawn");
        //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_PlayerHurt",              "<upgradeshortname>_PlayerHurt");
        //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_PlayerDeath",             "<upgradeshortname>_PlayerDeath");
        //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_PlayerJump",              "<upgradeshortname>_PlayerJump");
        //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_WeaponFire",              "<upgradeshortname>_WeaponFire");
        //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_BombPlanted",             "<upgradeshortname>_BombPlanted");
        //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_BombDefused",             "<upgradeshortname>_BombDefused");
        //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_BombExploded",            "<upgradeshortname>_BombExploded");
        //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_HostageRescued",          "<upgradeshortname>_HostageRescued");
        //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_HostageRescuedAll",       "<upgradeshortname>_HostageRescuedAll");
        //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_SmokegrenadeDetonate",    "<upgradeshortname>_SmokegrenadeDetonate");
      #endif
    
    // Custom events
    //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_OnUpgradePurchase,        "<upgradeshortname>_OnUpgradePurchase");
    //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_OnUpgradeSell,            "<upgradeshortname>_OnUpgradeSell");
    //EventMgr_RegisterEvent(g_module<upgradeshortname>, "Event_OnGameFrame",             "<upgradeshortname>_OnGameFrame");
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public <upgradeshortname>_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public <upgradeshortname>_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_module<upgradeshortname>, LogType_Normal, "<upgradeshortname>ConfigReload", "Reloaded <upgradeshortname> module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_module<upgradeshortname>, CM_CONFIGINDEX_FIRST, "<upgradeshortname>Module_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:<upgradeshortname>Module_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "<upgradeshortname>", false))
    {
        g_<upgradeshortname>UpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_<upgradeshortname>UpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_<upgradeshortname>UpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_<upgradeshortname>UpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_<upgradeshortname>UpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");

        if(g_<upgradeshortname>UpgradeConfigCache[UpgradeConfig_Disable]==1)
          ModuleMgr_Disable(g_module<upgradeshortname>);

        ModuleMgr_WriteCell(g_module<upgradeshortname>, ModuleData_MaxLevel, g_<upgradeshortname>UpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_module<upgradeshortname>, ModuleData_Cost, g_<upgradeshortname>UpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_module<upgradeshortname>, ModuleData_iCost, g_<upgradeshortname>UpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_module<upgradeshortname>);
}
#endif


/**
 * All plugins have loaded.
 */
public <upgradeshortname>_OnAllPluginsLoaded()
{
}

/**
 * Plugin is ending.
 */
public <upgradeshortname>_OnPluginEnd()
{
}

/**
 * All modules have been registered.
 */
public <upgradeshortname>_OnAllModulesLoaded()
{
}

/**
 * A module has been enabled.
 *
 * @return      Return Plugin_Handled to stop enable, and Plugin_Continue to allow it.
 */
public Action:<upgradeshortname>_OnModuleEnable(Module:module)
{
}

/**
 * The module that hooked this event callback has been enabled.
 *
 * @param refusalmsg    The string that is printed if Plugin_Handled is returned and it is non-empty.
 * @param maxlen        The max length of the string.
 *
 * @return              Return Plugin_Handled to stop enable, and Plugin_Continue to allow it.
 */
public Action:<upgradeshortname>_OnMyModuleEnable(String:refusalmsg[], maxlen)
{
}

/**
 * A module has been disabled.
 */
public <upgradeshortname>_OnModuleDisable(Module:module)
{
}

/**
 * A module has been disabled.
 *
 * @return              Return Plugin_Handled to stop disable, and Plugin_Continue to allow it.
 */
public Action:<upgradeshortname>_OnModuleDisable(Module:module)
{
}

/**
 * The module that hooked this event callback has been disabled.
 *
 * @param refusalmsg    The string that is printed if Plugin_Handled is returned and it is non-empty.
 * @param maxlen        The max length of the string.
 *
 * @return              Return Plugin_Handled to stop disable, and Plugin_Continue to allow it.
 */
public Action:<upgradeshortname>_OnMyModuleDisable(String:refusalmsg[], maxlen)
{
}

/**
 * The map has started.
 */
public <upgradeshortname>_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_module<upgradeshortname>, CM_CONFIGINDEX_FIRST, "<upgradeshortname>Module_ConfigCache");
    #endif
}

/**
 * The map has ended.
 */
public <upgradeshortname>_OnMapEnd()
{
}

/**
 * This is called before OnConfigsExecuted but any time after OnMapStart.
 * Per-map settings should be set here. 
 */
public <upgradeshortname>_OnAutoConfigsBuffered()
{
}

/**
 * All convars are set, cvar-dependent code should use this.
 */
public <upgradeshortname>_OnConfigsExecuted()
{
}

/**
 * Client has joined the server.
 * 
 * @param client    The client index.
 */
public <upgradeshortname>_OnClientPutInServer(client)
{
}

/**
 * Client is disconnecting from the server.
 * 
 * @param client    The client index.
 */
public <upgradeshortname>_OnClientDisconnect(client)
{
}

/**
 * Round has started.
 */
public <upgradeshortname>_RoundStart()
{
}

/**
 * Pre-round has freezetime has finished.
 */
public <upgradeshortname>_RoundFreezeEnd()
{
}

/**
 * Round has ended.
 *
 * @param winner    The index of the winning team.
 */
public <upgradeshortname>_RoundEnd(winner)
{
}

/**
 * Client has joined a team.
 * 
 * @param client        The client index.
 * @param team          The client's new team.
 * @param oldteam       The team the client is switching from.
 * @param disconnect    True if the client is disconnecting, false if switching to a team.
 */
public <upgradeshortname>_PlayerTeam(client, team, oldteam, bool:disconnect)
{
}

/**
 * Client has spawned.
 * 
 * @param client    The client index.
 * 
 */
public <upgradeshortname>_PlayerSpawn(client)
{
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
public <upgradeshortname>_PlayerHurt(victim, attacker, health, armor, const String:weapon[], dmg_health, dmg_armor, hitgroup)
{
}

/**
 * Client has been killed.
 * 
 * @param victim    The index of the killed client.
 * @param attacker  The killer of the victim.
 * @param weapon    The weapon classname used to kill the victim. (No weapon_ prefix)
 * @param headshot  True if the death was by headshot, false if not.
 */
public <upgradeshortname>_PlayerDeath(victim, attacker, const String:weapon[], bool:headshot)
{
}

/**
 * Client has jumped.
 * 
 * @param client        The client index.
 */
public <upgradeshortname>_PlayerJump(client)
{
}

/**
 * Client has fired a weapon.
 * 
 * @param client        The client index.
 * @param weapon        The weapon classname fired. (No weapon_ prefix)
 */
public <upgradeshortname>_WeaponFire(client, const String:weapon[])
{
}

/**
 * Client has planted the bomb
 *
 * @param planter       The client who has planted the bomb
 */
public <upgradeshortname>_BombPlanted(planter)
{
}

/**
 * Client has defused the bomb
 *
 * @param defuser       The client who has defused the bomb
 */
public <upgradeshortname>_BombDefused(defuser)
{
}

/**
 * The bomb has exploded
 *
 * @param planter       The client who has planted the bomb
 */
public <upgradeshortname>_BombExploded(planter)
{
}

/**
 * Client has rescued a hostage
 *
 * @param rescuer       The client who has rescued a hostage
 */
public <upgradeshortname>_HostageRescued(rescuer)
{
}

/**
 * All hostages got rescued
 */
public <upgradeshortname>_HostageRescuedAll()
{
}

/**
 * Smokegrenade has detonated
 *
 * @param thrower       The client who has thrown the smokegrenade
 */
public <upgradeshortname>_SmokegrenadeDetonate(thrower)
{
}

/**
 * Client has purchased an Upgrade
 */
public <upgradeshortname>_OnUpgradePurchase(client, level)
{
}

/**
 * Client has sold an Upgrade
 */
public <upgradeshortname>_OnUpgradeSell(client, level)
{
}

/**
 * Called before every server frame.
 * Note that you should avoid doing expensive computations here,
 * and you should declare large local arrays using 'decl' instead of 'new' 
 */
public <upgradeshortname>_OnGameFrame()
{
}
