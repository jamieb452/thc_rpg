/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          medic.inc
 *  Type:          Upgrade
 *  Description:   Gives you the ability to slowly raise the HP of nearby teammates
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
new Module:g_modulemedic;

new g_medicUpgradeConfigCache[UpgradeConfigs];

new Handle:g_hMedicPlayerIsHealing[MAXPLAYERS+1] = {INVALID_HANDLE,...};

/**
 * Register this module.
 */
medic_Register()
{
    // Define all the module's data as layed out by enum ModuleData in project.inc.
    new moduledata[ModuleData];
    
    moduledata[ModuleData_Disabled] = false;
    moduledata[ModuleData_Hidden] = false;
    strcopy(moduledata[ModuleData_ShortName], MM_DATA_SHORTNAME, "medic");
    strcopy(moduledata[ModuleData_FullName], MM_DATA_FULLNAME, "Medic");
    strcopy(moduledata[ModuleData_Description], MM_DATA_DESCRIPTION, "Gives you the ability to slowly raise the HP of nearby teammates");

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
    g_modulemedic = ModuleMgr_Register(moduledata);

    // Now register the events we're going to use.
    #if defined EVENT_MANAGER
        EventMgr_RegisterEvent(g_modulemedic, "Event_OnEventsRegister", "medic_OnEventsRegister");
    #endif

    // Register config file(s) that this module will use.
    #if defined CONFIG_MANAGER
        ConfigMgr_Register(g_modulemedic, "medic_OnConfigReload", "configs/thc_rpg/medic.txt");
    #endif
}

/**
 * Register all events here.
 */
public medic_OnEventsRegister()
{
    // Register all the events needed for this module.
    EventMgr_RegisterEvent(g_modulemedic, "Event_OnMyModuleEnable",             "medic_OnMyModuleEnable");
    EventMgr_RegisterEvent(g_modulemedic, "Event_OnMyModuleDisable",            "medic_OnMyModuleDisable");
    EventMgr_RegisterEvent(g_modulemedic, "Event_OnMapStart",                   "medic_OnMapStart");
    EventMgr_RegisterEvent(g_modulemedic, "Event_OnClientDisconnect",           "medic_OnClientDisconnect");
    #if defined PROJECT_GAME_CSS
        EventMgr_RegisterEvent(g_modulemedic, "Event_PlayerSpawn",              "medic_PlayerSpawn");
    #endif
        
    // Custom events
    EventMgr_RegisterEvent(g_modulemedic, "Event_OnUpgradePurchase",            "medic_OnUpgradePurchase");
    EventMgr_RegisterEvent(g_modulemedic, "Event_OnUpgradeSell",                "medic_OnUpgradeSell");
}

/**
 * All modules and events have been registered by this point.  Event priority can be changed here.
 */
public medic_OnEventsReady()
{
}

#if defined CONFIG_MANAGER
/**
 * Called when a registered config file (by this module) is manually reloaded.
 */
public medic_OnConfigReload(configindex)
{
    #if defined LOG_MANAGER
        LogMgr_Print(g_modulemedic, LogType_Normal, "medicConfigReload", "Reloaded medic module's config (index %d)", configindex);
    #endif

    ConfigMgr_CacheKv(g_modulemedic, CM_CONFIGINDEX_FIRST, "medicModule_ConfigCache");
}

/**
 * Read config values
 */
public KvCache:medicModule_ConfigCache(Handle:kv, sectionindex, const String:sectionname[])
{
    // Read Upgrade config
    if(StrEqual(sectionname, "medic", false))
    {
        g_medicUpgradeConfigCache[UpgradeConfig_Disable] = KvGetNum(kv, "disable");
        g_medicUpgradeConfigCache[UpgradeConfig_TeamLock] = KvGetNum(kv, "teamlock");
        g_medicUpgradeConfigCache[UpgradeConfig_Effects] = KvGetNum(kv, "effects");
        g_medicUpgradeConfigCache[UpgradeConfig_MaxLevel] = KvGetNum(kv, "maxlevel");
        g_medicUpgradeConfigCache[UpgradeConfig_Cost] = KvGetNum(kv, "cost");
        g_medicUpgradeConfigCache[UpgradeConfig_iCost] = KvGetNum(kv, "icost");
        g_medicUpgradeConfigCache[UpgradeConfig_Interval] = KvGetFloat(kv, "interval");
        g_medicUpgradeConfigCache[UpgradeConfig_Amount] = KvGetFloat(kv, "amount");

        if(g_medicUpgradeConfigCache[UpgradeConfig_Disable]==1)
            ModuleMgr_Disable(g_modulemedic);

        ModuleMgr_WriteCell(g_modulemedic, ModuleData_TeamLock, g_medicUpgradeConfigCache[UpgradeConfig_TeamLock]);
        ModuleMgr_WriteCell(g_modulemedic, ModuleData_MaxLevel, g_medicUpgradeConfigCache[UpgradeConfig_MaxLevel]);
        ModuleMgr_WriteCell(g_modulemedic, ModuleData_Cost, g_medicUpgradeConfigCache[UpgradeConfig_Cost]);
        ModuleMgr_WriteCell(g_modulemedic, ModuleData_iCost, g_medicUpgradeConfigCache[UpgradeConfig_iCost]);
    }
    else
        ModuleMgr_Disable(g_modulemedic);
}
#endif

/**
 * The module that hooked this event callback has been enabled.
 *
 * @param refusalmsg    The string that is printed if Plugin_Handled is returned and it is non-empty.
 * @param maxlen        The max length of the string.
 *
 * @return      Return Plugin_Handled to stop enable, and Plugin_Continue to allow it.
 */
public Action:medic_OnMyModuleEnable(String:refusalmsg[], maxlen)
{
    new size=GetArraySize(g_hVecPlayers);
    for(new x=0;x<size;x++)
    {
        new Handle:vecPlayer=GetArrayCell(g_hVecPlayers,x);
        new client=GetArrayCell(vecPlayer,VECTOR_PLAYERS_CLIENT);
        if(IsValidPlayer(client))
            if(GetClientTeam(client)!=g_medicUpgradeConfigCache[UpgradeConfig_TeamLock])
            {
                medic_StopHealing(client);
                if(GetPlayerUpgradeLevel(client,g_modulemedic)>0 && g_hMedicPlayerIsHealing[client]==INVALID_HANDLE)
                    g_hMedicPlayerIsHealing[client]=CreateTimer(g_medicUpgradeConfigCache[UpgradeConfig_Interval],Timer_OnMedicInterval,client,TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
            }
    }

    return Plugin_Continue;
}

/**
 * The module that hooked this event callback has been disabled.
 *
 * @param refusalmsg    The string that is printed if Plugin_Handled is returned and it is non-empty.
 * @param maxlen        The max length of the string.
 *
 * @return      Return Plugin_Handled to stop disable, and Plugin_Continue to allow it.
 */
public Action:medic_OnMyModuleDisable(String:refusalmsg[], maxlen)
{
    new size=GetArraySize(g_hVecPlayers);
    for(new x=0;x<size;x++)
    {
        new Handle:vecPlayer=GetArrayCell(g_hVecPlayers,x);
        new client=GetArrayCell(vecPlayer,VECTOR_PLAYERS_CLIENT);
        if(IsValidPlayer(client))
            medic_StopHealing(client);
    }

    return Plugin_Continue;
}

/**
 * The map has started.
 */
public medic_OnMapStart()
{
    #if defined CONFIG_MANAGER
        ConfigMgr_CacheKv(g_modulemedic, CM_CONFIGINDEX_FIRST, "medicModule_ConfigCache");
    #endif
}

/**
 * Client is disconnecting from the server.
 *
 * @param client    The client index.
 */
public medic_OnClientDisconnect(client)
{
    medic_StopHealing(client);
}

/**
 * Client has spawned.
 *
 * @param client    The client index.
 *
 */
public medic_PlayerSpawn(client)
{
    if(GetClientTeam(client)!=g_medicUpgradeConfigCache[UpgradeConfig_TeamLock])
    {
        medic_StopHealing(client);
        if(GetPlayerUpgradeLevel(client,g_modulemedic)>0 && g_hMedicPlayerIsHealing[client]==INVALID_HANDLE)
            g_hMedicPlayerIsHealing[client]=CreateTimer(g_medicUpgradeConfigCache[UpgradeConfig_Interval],Timer_OnMedicInterval,client,TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
    }
    else
        medic_StopHealing(client);
}

/**
 * Client has purchased an Upgrade
 */
public medic_OnUpgradePurchase(client, level)
{
    medic_StopHealing(client);
    g_hMedicPlayerIsHealing[client]=CreateTimer(g_medicUpgradeConfigCache[UpgradeConfig_Interval],Timer_OnMedicInterval,client,TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
}

/**
 * Client has sold an Upgrade
 */
public medic_OnUpgradeSell(client, level)
{
    medic_StopHealing(client);
    if(level>0 && g_hMedicPlayerIsHealing[client]==INVALID_HANDLE)
        g_hMedicPlayerIsHealing[client]=CreateTimer(g_medicUpgradeConfigCache[UpgradeConfig_Interval],Timer_OnMedicInterval,client,TIMER_REPEAT|TIMER_FLAG_NO_MAPCHANGE);
}

public Action:Timer_OnMedicInterval(Handle:timer,any:client)
{
    if(IsValidPlayer(client))
    {
        if(IsPlayerAlive(client))
        {
            new Float:clientloc[3];
            new Float:teammateloc[3];
            new color[4]={0,255,0,127};
            new newhp;
            new level=GetPlayerUpgradeLevel(client,g_modulemedic);
            new team=GetClientTeam(client);

            GetClientAbsOrigin(client,clientloc);

            // Make green beam ring effect (Visible to teammates if freeforall is disabled)
            if(g_medicUpgradeConfigCache[UpgradeConfig_Effects]&&g_CoreConfigCache[CoreConfig_freeforall]==0)
            {
                if(team==2)
                    BeamRingEffect("@t",clientloc,0.0,float(level)*120.0,g_medicUpgradeConfigCache[UpgradeConfig_Interval]*6,3.0,color,0.0,20);
                else if(team==3)
                    BeamRingEffect("@ct",clientloc,0.0,float(level)*120.0,g_medicUpgradeConfigCache[UpgradeConfig_Interval]*6,3.0,color,0.0,20);
            }

            for(new teammate=1;teammate<=MaxClients;teammate++)
            {
                if(IsClientInGame(teammate))
                {
                    GetClientAbsOrigin(teammate,teammateloc);
                    if(AreValidPlayers(client,teammate)&&(team==GetClientTeam(teammate)||g_CoreConfigCache[CoreConfig_freeforall])&&IsPlayerAlive(teammate)&&GetDistanceBetween(clientloc,teammateloc)<=level*120)
                    {
                        newhp=GetClientHealth(teammate)+RoundToNearest(level*g_medicUpgradeConfigCache[UpgradeConfig_Amount]);
                        if(newhp>GetModifiedHealth(teammate))
                            newhp=GetModifiedHealth(teammate);
                        SetEntityHealth(teammate,newhp);
                        // Make green beam visible only to the healer's teammates
                        if(g_medicUpgradeConfigCache[UpgradeConfig_Effects])
                        {
                            clientloc[2]+=45;
                            teammateloc[2]+=45;
                            if(g_CoreConfigCache[CoreConfig_freeforall]==0)
                            {
                                if(team==2)
                                    BeamEffect("@t",clientloc,teammateloc,0.2,2.0,2.0,color,0.0,0);
                                else if(team==3)
                                    BeamEffect("@ct",clientloc,teammateloc,0.2,2.0,2.0,color,0.0,0);
                            }
                            else
                                BeamEffect("@all",clientloc,teammateloc,0.2,2.0,2.0,color,0.0,0);
                        }
                    }
                }
            }
        }
        else
        {
            g_hMedicPlayerIsHealing[client]=INVALID_HANDLE;
            return Plugin_Stop;
        }
        return Plugin_Continue;
    }

    g_hMedicPlayerIsHealing[client]=INVALID_HANDLE;
    return Plugin_Stop;
}

public medic_StopHealing(client)
{
    if(g_hMedicPlayerIsHealing[client]!=INVALID_HANDLE)
    {
        KillTimer(g_hMedicPlayerIsHealing[client]);
        g_hMedicPlayerIsHealing[client]=INVALID_HANDLE;
    }
}
