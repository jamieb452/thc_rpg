/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          regen_ammo.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to steadily regain ammo for the active gun until it is fully replenished
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
new Module:g_moduleregen_ammo;

new g_regen_ammoUpgradeConfigCache[UpgradeConfigs];

new bool:g_bPlayerIsRegeneratingAmmo[MAXPLAYERS+1];

/**
 * Register this module.
 */
regen_ammo_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "regen_ammo");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "RegenerateAmmo");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to steadily regain ammo for the active gun until it is fully replenished");

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
    g_moduleregen_ammo = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_moduleregen_ammo, "Event_OnEventsRegister", "regen_ammo_OnEventsRegister");
    #endif  
  
    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_moduleregen_ammo, "regen_ammo_OnConfigReload", "configs/thc_rpg/regen_ammo.txt");
    #endif
}

/**
 * Register all events here.
 */
public regen_ammo_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_moduleregen_ammo, "Event_OnMapStart",           "regen_ammo_OnMapStart");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_moduleregen_ammo, "Event_WeaponFire",           "regen_ammo_WeaponFire");
        EventMgr_RegisterEvent(g_moduleregen_ammo, "Event_PlayerSpawn",          "regen_ammo_PlayerSpawn");
    #endif
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public regen_ammo_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public regen_ammo_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_moduleregen_ammo, LogType_Normal, "regen_ammoConfigReload", "Reloaded regen_ammo module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_moduleregen_ammo, CM_CONFIGINDEX_FIRST, "regen_ammoModule_ConfigCache");
}

/**
  * Read config values
  */
public KvCache:regen_ammoModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "regen_ammo", false))
    {
        g_regen_ammoUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_regen_ammoUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_regen_ammoUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_regen_ammoUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_regen_ammoUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_regen_ammoUpgradeConfigCache[UpgradeConfig_Interval] = KvGetFloat(kv, "interval");
        g_regen_ammoUpgradeConfigCache[UpgradeConfig_Amount] = KvGetFloat(kv, "amount");

        if(g_regen_ammoUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_moduleregen_ammo);

        ModuleMgr_WriteCell(g_moduleregen_ammo, ModuleData_TeamLock, g_regen_ammoUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_moduleregen_ammo, ModuleData_MaxLevel, g_regen_ammoUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_moduleregen_ammo, ModuleData_Cost, g_regen_ammoUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_moduleregen_ammo, ModuleData_iCost, g_regen_ammoUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_moduleregen_ammo);
}
#endif

/**
 * The map has started.
 */
public regen_ammo_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_moduleregen_ammo, CM_CONFIGINDEX_FIRST, "regen_ammoModule_ConfigCache");
    #endif
}

/**
 * Client has spawned.
 * 
 * @param client    The client index.
 * 
 */
public regen_ammo_PlayerSpawn(client)
{
    g_bPlayerIsRegeneratingAmmo[client]=false;
}

/**
 * Client has fired a weapon.
 * 
 * @param client        The client index.
 * @param weapon        The weapon classname fired. (No weapon_ prefix)
 */
/*
  Weapon: weapon_deagle         Offset: 1   Count: 35
  Weapon: weapon_ak47           Offset: 2   Count: 90
  Weapon: weapon_aug            Offset: 2   Count: 90
  Weapon: weapon_g3sg1          Offset: 2   Count: 90
  Weapon: weapon_scout          Offset: 2   Count: 90
  Weapon: weapon_sg550          Offset: 3   Count: 90
  Weapon: weapon_sg552          Offset: 3   Count: 90
  Weapon: weapon_famas          Offset: 3   Count: 90
  Weapon: weapon_galil          Offset: 3   Count: 90
  Weapon: weapon_m4a1           Offset: 3   Count: 90
  Weapon: weapon_m249           Offset: 4   Count: 200
  Weapon: weapon_awp            Offset: 5   Count: 30
  Weapon: weapon_elite          Offset: 6   Count: 120
  Weapon: weapon_glock          Offset: 6   Count: 120
  Weapon: weapon_mp5navy        Offset: 6   Count: 120
  Weapon: weapon_tmp            Offset: 6   Count: 120
  Weapon: weapon_xm1014         Offset: 7   Count: 32
  Weapon: weapon_m3             Offset: 7   Count: 32
  Weapon: weapon_mac10          Offset: 8   Count: 100
  Weapon: weapon_ump45          Offset: 8   Count: 100
  Weapon: weapon_usp            Offset: 8   Count: 100
  Weapon: weapon_p228           Offset: 9   Count: 52
  Weapon: weapon_p90            Offset: 10  Count: 100
  Weapon: weapon_fiveseven      Offset: 10  Count: 100
*/
public regen_ammo_WeaponFire(client, const String:weapon[])
{
    if(IsValidPlayer(client))
        if(GetClientTeam(client)!=g_regen_ammoUpgradeConfigCache[UpgradeConfig_TeamLock])
        {
            if(GetPlayerUpgradeLevel(client,g_moduleregen_ammo)&&!g_bPlayerIsRegeneratingAmmo[client])
            {
                g_bPlayerIsRegeneratingAmmo[client]=true;
                new max_ammo;
                new weapon_offset;
                if(StrEqual(weapon, "deagle"))
                {
                    max_ammo=35;
                    weapon_offset=1;
                }
                if(StrEqual(weapon, "ak47")||StrEqual(weapon, "aug")||StrEqual(weapon, "g3sg1")||StrEqual(weapon, "scout"))
                {
                    max_ammo=90;
                    weapon_offset=2;
                }
                if(StrEqual(weapon, "sg550")||StrEqual(weapon, "sg552")||StrEqual(weapon, "famas")||StrEqual(weapon, "galil")||StrEqual(weapon, "m4a1"))
                {
                    max_ammo=90;
                    weapon_offset=3;
                }
                if(StrEqual(weapon, "m249"))
                {
                    max_ammo=200;
                    weapon_offset=4;
                }
                if(StrEqual(weapon, "awp"))
                {
                    max_ammo=30;
                    weapon_offset=5;
                }
                if(StrEqual(weapon, "elite")||StrEqual(weapon, "glock")||StrEqual(weapon, "mp5navy")||StrEqual(weapon, "tmp"))
                {
                    max_ammo=120;
                    weapon_offset=6;
                }
                if(StrEqual(weapon, "xm1014")||StrEqual(weapon, "m3"))
                {
                    max_ammo=32;
                    weapon_offset=7;
                }
                if(StrEqual(weapon, "mac10")||StrEqual(weapon, "ump45")||StrEqual(weapon, "usp"))
                {
                    max_ammo=100;
                    weapon_offset=8;
                }
                if(StrEqual(weapon, "p228"))
                {
                    max_ammo=52;
                    weapon_offset=9;
                }
                if(StrEqual(weapon, "p90")||StrEqual(weapon, "fiveseven"))
                {
                    max_ammo=100;
                    weapon_offset=10;
                }
                new Handle:datapack=CreateDataPack();
                WritePackCell(datapack, client);
                WritePackCell(datapack, max_ammo);
                WritePackCell(datapack, weapon_offset);
                CreateTimer(g_regen_ammoUpgradeConfigCache[UpgradeConfig_Interval],Timer_OnRegen_ammoInterval,datapack,TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
            }
        }
}

public Action:Timer_OnRegen_ammoInterval(Handle:timer,Handle:datapack)
{
    ResetPack(datapack);
    new client=ReadPackCell(datapack);
    new max_ammo=ReadPackCell(datapack);
    new weapon_offset=ReadPackCell(datapack);

    if(IsClientInGame(client)&&g_bPlayerIsRegeneratingAmmo[client])
    {
        new amount=GetEntData(client,g_iAmmoOffset+(weapon_offset*4),4)+RoundToNearest(g_regen_ammoUpgradeConfigCache[UpgradeConfig_Amount]*GetPlayerUpgradeLevel(client,g_moduleregen_ammo));
        if(amount>max_ammo)
        {
            g_bPlayerIsRegeneratingAmmo[client]=false;
            amount=max_ammo;
        }
        SetEntData(client,g_iAmmoOffset+(weapon_offset*4),amount,4,true);
        return Plugin_Continue;
    }

    CloseHandle(datapack);
    return Plugin_Stop;
}
