/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          project_events.inc
 *  Type:          Base
 *  Description:   Event hooking and definitions.  Any event-related stuff for your project should go here.
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
 * The max number of events that the project can have.
 */
#define PROJECT_MAX_EVENTS 256

/**
 * Event tag.
 */
enum ProjectEvent
{
    INVALID_EVENT = -1  /** Used as an invalid event. */
}

/**
 * Global variables to store each event's ID.
 */

// Cross-game events
new ProjectEvent:g_EvOnAllPluginsLoaded;
new ProjectEvent:g_EvOnPluginEnd;
new ProjectEvent:g_EvOnEventsRegister;
new ProjectEvent:g_EvOnEventsReady;
new ProjectEvent:g_EvOnAllModulesLoaded;
new ProjectEvent:g_EvOnModuleEnable;
new ProjectEvent:g_EvOnMyModuleEnable;
new ProjectEvent:g_EvOnModuleDisable;
new ProjectEvent:g_EvOnMyModuleDisable;
new ProjectEvent:g_EvOnMapStart;
new ProjectEvent:g_EvOnMapEnd;
new ProjectEvent:g_EvOnAutoConfigsBuffered;
new ProjectEvent:g_EvOnConfigsExecuted;
new ProjectEvent:g_EvOnClientPutInServer;
new ProjectEvent:g_EvOnClientDisconnect;
new ProjectEvent:g_EvOnClientAuthorized;

// CS:S events.
#if defined PROJECT_GAME_CSS
new ProjectEvent:g_EvRoundStart;
new ProjectEvent:g_EvRoundFreezeEnd;
new ProjectEvent:g_EvRoundEnd;
new ProjectEvent:g_EvPlayerTeam;
new ProjectEvent:g_EvPlayerSpawn;
new ProjectEvent:g_EvPlayerHurt;
new ProjectEvent:g_EvPlayerDeath;
new ProjectEvent:g_EvPlayerJump;
new ProjectEvent:g_EvWeaponFire;
//new ProjectEvent:g_EvWeaponEntityFire;

// Custom events.
new ProjectEvent:g_EvBombPlanted;
new ProjectEvent:g_EvBombDefused;
new ProjectEvent:g_EvBombExploded;
new ProjectEvent:g_EvHostageRescued;
new ProjectEvent:g_EvHostageRescuedAll;
new ProjectEvent:g_EvSmokegrenadeDetonated;
new ProjectEvent:g_EvHEGrenadeDetonated;
new ProjectEvent:g_EvFlashBangDetonated;
#endif

// Custom events.
new ProjectEvent:g_EvOnUpgradePurchase;
new ProjectEvent:g_EvOnUpgradeSell;
new ProjectEvent:g_EvOnGameFrame;

/**
 * Datatypes that can be passed with each event to modules.
 * Don't touch this!
 */
enum EventDataTypes
{
    DataType_Cell,
    DataType_CellRef,
    DataType_Float,
    DataType_FloatRef,
    DataType_Array,
    DataType_ArrayRef,
    DataType_String,
    DataType_StringRef,
}

/**
 * Common data type info.
 */

// When forwarding custom events with no data, use this for the 'eventdata' parameter in EventMgr_Forward
stock any:g_CommonEventData1[][] = {{0}};

// If you are using g_CommonEventData1, then the next parameter should be this.  This indicates you are passing no data.
stock EventDataTypes:g_CommonDataType1[] = {};

// This indicates that you are passing a single cell into an event.  Common for events that only pass a client index.
stock EventDataTypes:g_CommonDataType2[] = {DataType_Cell};

stock EventDataTypes:g_CommonDataType3[] = {DataType_Cell,DataType_Cell};
stock EventDataTypes:g_CommonDataType4[] = {DataType_Cell,DataType_Cell,DataType_Cell,DataType_Cell};
stock EventDataTypes:g_CommonDataType5[] = {DataType_Cell,DataType_String};


// **********************************************
//            SourceMod Forwards
// **********************************************

/**
 * All plugins have loaded.
 */
public OnAllPluginsLoaded()
{
    // Forward event to all modules.
    EventMgr_Forward(g_EvOnAllPluginsLoaded, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * The map has started.
 */
public OnMapStart()
{
    // Forward event to all modules.
    EventMgr_Forward(g_EvOnMapStart, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * The map has ended.
 */
public OnMapEnd()
{
    // Forward event to all modules.
    EventMgr_Forward(g_EvOnMapEnd, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * This is called before OnConfigsExecuted but any time after OnMapStart.
 * Per-map settings should be set here. 
 */
public OnAutoConfigsBuffered()
{
    // Forward event to all modules.
    EventMgr_Forward(g_EvOnAutoConfigsBuffered, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * All convars are set, cvar-dependent code should use this.
 */
public OnConfigsExecuted()
{
    // Forward event to all modules.
    EventMgr_Forward(g_EvOnConfigsExecuted, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * Client has joined the server.
 * 
 * @param client    The client index.
 */
public OnClientPutInServer(client)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = client;
    
    EventMgr_Forward(g_EvOnClientPutInServer, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
}

/**
 * Client is disconnecting from the server.
 * 
 * @param client    The client index.
 */
public OnClientDisconnect(client)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = client;
    
    EventMgr_Forward(g_EvOnClientDisconnect, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
}

/**
 * Client got Steam-Id
 * 
 * @param client    The client index.
 * @param auth      Client Auth String
 */
public OnClientAuthorized(client, const String:auth[])
{
    // Forward event to all modules.
    new any:eventdata[2][64];
    eventdata[0][0] = client;
    strcopy(eventdata[1], sizeof(eventdata[]), auth);
    
    EventMgr_Forward(g_EvOnClientAuthorized, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType5);
}

/**
 * Called before every server frame.
 * Note that you should avoid doing expensive computations here,
 * and you should declare large local arrays using 'decl' instead of 'new' 
 */
public OnGameFrame()
{
    g_iGameframes++;

    if(g_iGameframes>=50)
    {
        g_iGameframes=0;

        // Forward event to all modules.
        EventMgr_Forward(g_EvOnGameFrame, g_CommonEventData1, 0, 0, g_CommonDataType1);
    }
}

// **********************************************
//             Game Event Creating
// **********************************************

/**
 * This is called from the event manager in OnPluginStart.
 * Create the events you want to forward to modules here.
 */
stock EventMgr_CreateEvents()
{
    // Hook events to manage and forward to modules.
    
    // Cross-game events.
    g_EvOnAllPluginsLoaded =        EventMgr_CreateEvent("Event_OnAllPluginsLoaded");       /** All plugins have loaded. */
    g_EvOnPluginEnd =               EventMgr_CreateEvent("Event_OnPluginEnd");              /** Plugin is ending. */
    g_EvOnEventsRegister =          EventMgr_CreateEvent("Event_OnEventsRegister");         /** All events are created, modules should register them here. */
    g_EvOnEventsReady =             EventMgr_CreateEvent("Event_OnEventsReady");            /** All events are created AND registered by here.  Event priority can be changed here. */
    g_EvOnAllModulesLoaded =        EventMgr_CreateEvent("Event_OnAllModulesLoaded");       /** All modules have been registered. */
    g_EvOnModuleEnable =            EventMgr_CreateEvent("Event_OnModuleEnable");           /** A module has been enabled. */
    g_EvOnMyModuleEnable =          EventMgr_CreateEvent("Event_OnMyModuleEnable");         /** The module that hooked this event callback has been enabled. */
    g_EvOnModuleDisable =           EventMgr_CreateEvent("Event_OnModuleDisable");          /** A module has been disabled. */
    g_EvOnMyModuleDisable =         EventMgr_CreateEvent("Event_OnMyModuleDisable");        /** The module that hooked this event callback has been disabled. */
    g_EvOnMapStart =                EventMgr_CreateEvent("Event_OnMapStart");               /** The map has started. */
    g_EvOnMapEnd =                  EventMgr_CreateEvent("Event_OnMapEnd");                 /** The map has ended. */
    g_EvOnAutoConfigsBuffered =     EventMgr_CreateEvent("Event_OnAutoConfigsBuffered");    /** This is called before OnConfigsExecuted but any time after OnMapStart. */
    g_EvOnConfigsExecuted =         EventMgr_CreateEvent("Event_OnConfigsExecuted");        /** All convars are set, cvar-dependent code should use this. */
    g_EvOnClientPutInServer =       EventMgr_CreateEvent("Event_OnClientPutInServer");      /** Client has joined the server. */
    g_EvOnClientDisconnect =        EventMgr_CreateEvent("Event_OnClientDisconnect");       /** Client is disconnecting from the server. */
    g_EvOnClientAuthorized =        EventMgr_CreateEvent("Event_OnClientAuthorized");       /** Client got Steam-Id. */
    g_EvOnUpgradePurchase =        	EventMgr_CreateEvent("Event_OnUpgradePurchase");		/** Upgrade got purchased. !!! Forward is called in upgradesystem.inc !!! */
    g_EvOnUpgradeSell =        		EventMgr_CreateEvent("Event_OnUpgradeSell");        	/** Upgrade got sold. !!! Forward is called in upgradesystem.inc !!! */
    g_EvOnGameFrame =               EventMgr_CreateEvent("Event_OnGameFrame");              /** Called before every server frame. */
                                                                                            /** Note that you should avoid doing expensive computations here, */
                                                                                            /** and you should declare large local arrays using 'decl' instead of 'new' */

    // CS:S events.
    #if defined PROJECT_GAME_CSS
        g_EvRoundStart =            EventMgr_CreateEvent("Event_RoundStart");               /** Round has started. */
        g_EvRoundFreezeEnd =        EventMgr_CreateEvent("Event_RoundFreezeEnd");           /** Pre-round freezetime has finished. */
        g_EvRoundEnd =              EventMgr_CreateEvent("Event_RoundEnd");                 /** Round has ended. */
        g_EvPlayerTeam =            EventMgr_CreateEvent("Event_PlayerTeam");               /** Client has joined a team. */
        g_EvPlayerSpawn =           EventMgr_CreateEvent("Event_PlayerSpawn");              /** Client has spawned. */
        g_EvPlayerHurt =            EventMgr_CreateEvent("Event_PlayerHurt");               /** Client has been damaged. */
        g_EvPlayerDeath =           EventMgr_CreateEvent("Event_PlayerDeath");              /** Client has been killed. */
        g_EvPlayerJump =            EventMgr_CreateEvent("Event_PlayerJump");               /** Client has jumped. */
        g_EvWeaponFire =            EventMgr_CreateEvent("Event_WeaponFire");               /** Client has fired a weapon. */
        //g_EvWeaponEntityFire =      EventMgr_CreateEvent("Event_WeaponEntityFire");         /** Client has fired a weapon. (returns weapon entity index) */
        g_EvBombPlanted =           EventMgr_CreateEvent("Event_BombPlanted");              /** Client has planted the bomb. */
        g_EvBombDefused =           EventMgr_CreateEvent("Event_BombDefused");              /** Client has defused the bomb. */
        g_EvBombExploded =          EventMgr_CreateEvent("Event_BombExploded");             /** The bomb has exploded. */
        g_EvHostageRescued =        EventMgr_CreateEvent("Event_HostageRescued");           /** Client has rescued a hostage. */
        g_EvHostageRescuedAll =     EventMgr_CreateEvent("Event_HostageRescuedAll");        /** All hostages got rescued. */
        g_EvSmokegrenadeDetonated = EventMgr_CreateEvent("Event_SmokegrenadeDetonated");    /** Smokegrenade has detonated. */
        g_EvHEGrenadeDetonated =    EventMgr_CreateEvent("Event_HEGrenadeDetonated");       /** HE Grenade has detonated. */
        g_EvFlashBangDetonated =    EventMgr_CreateEvent("Event_FlashBangDetonated");       /** FlashBang has detonated. */
    #endif
    
    // HL2DM events.
    #if defined PROJECT_GAME_HL2DM
    
    #endif
    
    // DOD events.
    #if defined PROJECT_GAME_DOD
    
    #endif
    
    // TF2 events.
    #if defined PROJECT_GAME_TF2
    
    #endif
    
    // L4D events.
    #if defined PROJECT_GAME_L4D
    #endif
    
    // L4D2 events.
    #if defined PROJECT_GAME_L4D2
    #endif
}

// **********************************************
//              Game Event Hooking
// **********************************************

/**
 * This is called from the event manager in OnPluginStart.
 * Hook the events you want to forward to modules here.
 */
stock EventMgr_HookEvents()
{
    // Hook events to manage and forward to modules.
    
    // CS:S events.
    #if defined PROJECT_GAME_CSS
        HookEvent("round_start", GameEvent_RoundStart);
        HookEvent("round_freeze_end", GameEvent_RoundFreezeEnd);
        HookEvent("round_end", GameEvent_RoundEnd);
        HookEvent("player_team", GameEvent_PlayerTeam);
        HookEvent("player_spawn", GameEvent_PlayerSpawn);
        HookEvent("player_hurt", GameEvent_PlayerHurt);
        HookEvent("player_death", GameEvent_PlayerDeath);
        HookEvent("player_jump", GameEvent_PlayerJump);
        HookEvent("weapon_fire", GameEvent_WeaponFire);
        HookEvent("bomb_planted", GameEvent_BombPlanted);
        HookEvent("bomb_defused", GameEvent_BombDefused);
        HookEvent("bomb_exploded", GameEvent_BombExploded);
        HookEvent("hostage_rescued", GameEvent_HostageRescued);
        HookEvent("hostage_rescued_all", GameEvent_HostageRescuedAll);
        HookEvent("smokegrenade_detonate", GameEvent_SmokegrenadeDetonated);
        HookEvent("hegrenade_detonate", GameEvent_HEGrenadeDetonated);
        HookEvent("flashbang_detonate", GameEvent_FlashBangDetonated);
    #endif
    
    // HL2DM events.
    #if defined PROJECT_GAME_HL2DM
    
    #endif
    
    // DOD events.
    #if defined PROJECT_GAME_DOD
    
    #endif
    
    // TF2 events.
    #if defined PROJECT_GAME_TF2
    
    #endif
    
    // L4D events.
    #if defined PROJECT_GAME_L4D
    #endif
    
    // L4D2 events.
    #if defined PROJECT_GAME_L4D2
    #endif
}

// **********************************************
//           CS:S Game Event Callbacks
// **********************************************

#if defined PROJECT_GAME_CSS

/**
 * Round has started.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_RoundStart(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    EventMgr_Forward(g_EvRoundStart, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * Pre-round freezetime has finished.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_RoundFreezeEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    EventMgr_Forward(g_EvRoundFreezeEnd, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * Round has ended.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_RoundEnd(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = GetEventInt(event, "winner");

    EventMgr_Forward(g_EvRoundEnd, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
}

/**
 * Client has joined a team.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_PlayerTeam(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_Cell, DataType_Cell, DataType_Cell};
    new any:eventdata[sizeof(eventdatatypes)][1];
    
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    eventdata[1][0] = GetEventInt(event, "team");
    eventdata[2][0] = GetEventInt(event, "oldteam");
    eventdata[3][0] = GetEventBool(event, "disconnect");
    
    EventMgr_Forward(g_EvPlayerTeam, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
}

/**
 * Client has spawned.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_PlayerSpawn(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    
    EventMgr_Forward(g_EvPlayerSpawn, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
}

/**
 * Client has been damaged.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_PlayerHurt(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_Cell, DataType_Cell, DataType_Cell, DataType_String, DataType_Cell, DataType_Cell, DataType_Cell};
    new any:eventdata[sizeof(eventdatatypes)][32];
    
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    eventdata[1][0] = GetClientOfUserId(GetEventInt(event, "attacker"));
    eventdata[2][0] = GetEventInt(event, "health");
    eventdata[3][0] = GetEventInt(event, "armor");
    GetEventString(event, "weapon", eventdata[4], sizeof(eventdata[]));
    eventdata[5][0] = GetEventInt(event, "dmg_health");
    eventdata[6][0] = GetEventInt(event, "dmg_armor");
    eventdata[7][0] = GetEventInt(event, "hitgroup");
    
    EventMgr_Forward(g_EvPlayerHurt, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
}

/**
 * Client has been killed.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_PlayerDeath(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_Cell, DataType_String, DataType_Cell};
    new any:eventdata[sizeof(eventdatatypes)][32];
    
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    eventdata[1][0] = GetClientOfUserId(GetEventInt(event, "attacker"));
    GetEventString(event, "weapon", eventdata[2], sizeof(eventdata[]));
    eventdata[3][0] = GetEventInt(event, "headshot");
    
    EventMgr_Forward(g_EvPlayerDeath, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
}

/**
 * Client has jumped.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_PlayerJump(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    
    EventMgr_Forward(g_EvPlayerJump, eventdata, 1, 1, g_CommonDataType2);
}

/**
 * Client has fired a weapon.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_WeaponFire(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules. (WeaponFire)
    static EventDataTypes:eventdatatypes[] = {DataType_Cell, DataType_String};
    new any:eventdata[sizeof(eventdatatypes)][32];
    
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    GetEventString(event, "weapon", eventdata[1], sizeof(eventdata[]));
    
    EventMgr_Forward(g_EvWeaponFire, eventdata, sizeof(eventdata), sizeof(eventdata[]), eventdatatypes);
    
    /*
    // Forward event to all modules. (WeaponEntityFire)
    new eventdata2[sizeof(eventdatatypes)][1];
    static EventDataTypes:eventdatatypes2[] = {DataType_Cell, DataType_Cell};
    
    decl String:classname[32];
    new weaponentity;
    
    // Loop through all entities.
    new maxentities = GetMaxEntities();
    for (new entity = MaxClients; entity < maxentities; entity++)
    {
        if (!IsValidEntity(entity))
            continue;
        
        GetEdictClassname(entity, classname, sizeof(classname));
        if (StrContains(classname, eventdata[1], false) == -1)
            continue;
        
        if (eventdata[0][0] == GetEntPropEnt(entity, Prop_Data, "m_hOwner"))
        {
            weaponentity = entity;
            break;
        }
    }
    
    eventdata2[0][0] = eventdata[0][0];
    eventdata2[1][0] = weaponentity;
    
    EventMgr_Forward(g_EvWeaponEntityFire, eventdata2, sizeof(eventdata2), sizeof(eventdata2[]), eventdatatypes2);
    */
}

/**
 * Client has planted the bomb.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_BombPlanted(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    
    EventMgr_Forward(g_EvBombPlanted, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
}

/**
 * Client has defused the bomb.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_BombDefused(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    
    EventMgr_Forward(g_EvBombDefused, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
}

/**
 * The bomb has exploded.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_BombExploded(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    
    EventMgr_Forward(g_EvBombExploded, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
}

/**
 * Client has rescued a hostage.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_HostageRescued(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    new any:eventdata[1][1];
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    
    EventMgr_Forward(g_EvHostageRescued, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType2);
}

/**
 * All hostages got rescued.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_HostageRescuedAll(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.   
    EventMgr_Forward(g_EvHostageRescuedAll, g_CommonEventData1, 0, 0, g_CommonDataType1);
}

/**
 * Smokegrenade has detonated.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking 
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_SmokegrenadeDetonated(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    new any:eventdata[4][1];
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    eventdata[1][0] = GetEventFloat(event,"x");
    eventdata[2][0] = GetEventFloat(event,"y");
    eventdata[3][0] = GetEventFloat(event,"z");
    
    EventMgr_Forward(g_EvSmokegrenadeDetonated, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType4);
}

/**
 * HEGrenade has detonated.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_HEGrenadeDetonated(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    new any:eventdata[4][1];
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    eventdata[1][0] = GetEventFloat(event,"x");
    eventdata[2][0] = GetEventFloat(event,"y");
    eventdata[3][0] = GetEventFloat(event,"z");

    EventMgr_Forward(g_EvHEGrenadeDetonated, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType4);
}

/**
 * FlashBang has detonated.
 *
 * @param event			Handle to event. This could be INVALID_HANDLE if every plugin hooking
 *						this event has set the hook mode EventHookMode_PostNoCopy.
 * @param name			String containing the name of the event.
 * @param dontBroadcast	True if event was not broadcast to clients, false otherwise.
 */
public GameEvent_FlashBangDetonated(Handle:event, const String:name[], bool:dontBroadcast)
{
    // Forward event to all modules.
    new any:eventdata[4][1];
    eventdata[0][0] = GetClientOfUserId(GetEventInt(event, "userid"));
    eventdata[1][0] = GetEventFloat(event,"x");
    eventdata[2][0] = GetEventFloat(event,"y");
    eventdata[3][0] = GetEventFloat(event,"z");

    EventMgr_Forward(g_EvFlashBangDetonated, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType4);
}
#endif
