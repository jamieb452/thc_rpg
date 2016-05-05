/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          admincmds.inc
 *  Type:          Core
 *  Description:   hooks all admin commands
 *                 contains admin functions and the admin menu
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

#pragma semicolon 1

#undef REQUIRE_PLUGIN
#include <adminmenu>

#define THC_RPG_ADMINFLAGS ADMFLAG_CUSTOM3

new Handle:hAdminMenu = INVALID_HANDLE;

enum AdminCmdEvent
{
    AdminCmdEvent_XP_min100			= 1,
    AdminCmdEvent_XP_min50			= 2,
    AdminCmdEvent_XP_min5			= 3,
    AdminCmdEvent_XP_plus5			= 4,
    AdminCmdEvent_XP_plus50			= 5,
    AdminCmdEvent_XP_plus100		= 6,
    AdminCmdEvent_Level_min50		= 7,
    AdminCmdEvent_Level_min10		= 8,
    AdminCmdEvent_Level_min5		= 9,
    AdminCmdEvent_Level_plus5		= 10,
    AdminCmdEvent_Level_plus10		= 11,
    AdminCmdEvent_Level_plus50		= 12,
    AdminCmdEvent_Credits_min100	= 13,
    AdminCmdEvent_Credits_min50		= 14,
    AdminCmdEvent_Credits_min5		= 15,
    AdminCmdEvent_Credits_plus5		= 16,
    AdminCmdEvent_Credits_plus50	= 17,
    AdminCmdEvent_Credits_plus100	= 18,
};

HookAdminCmds()
{
    // Hook all admin cmds
    RegAdminCmd("thc_rpg_xp",AdminCmd_XP,THC_RPG_ADMINFLAGS);
    RegAdminCmd("thc_rpg_level",AdminCmd_Level,THC_RPG_ADMINFLAGS);
    RegAdminCmd("thc_rpg_credits",AdminCmd_Credits,THC_RPG_ADMINFLAGS);
    RegAdminCmd("thc_rpg_forcepurchase",AdminCmd_ForcePurchase,THC_RPG_ADMINFLAGS);
    RegAdminCmd("thc_rpg_forcesell",AdminCmd_ForceSell,THC_RPG_ADMINFLAGS);
    RegAdminCmd("thc_rpg_resetplayer",AdminCmd_ResetPlayer,THC_RPG_ADMINFLAGS);
    RegAdminCmd("thc_rpg_resetallplayers",AdminCmd_ResetAllPlayers,THC_RPG_ADMINFLAGS);
    RegAdminCmd("thc_rpg_resetdb",AdminCmd_ResetDB,THC_RPG_ADMINFLAGS);
}

public Action:AdminCmd_XP(client,argc)
{
    if(argc>=3)
    {
        decl String:action[8],String:target[64],String:amount[8];
        decl String:adminname[64],String:name[64];
        new newamt, clients[MAXPLAYERS];
        
        GetCmdArg(1,action,sizeof(action));
        GetCmdArg(2,target,sizeof(target));
        GetCmdArg(3,amount,sizeof(amount));
        GetClientName(client,adminname,sizeof(adminname));
        new count=FindMatchingPlayers(target,clients);
        for(new x=0;x<count;x++)
        {
            if(IsValidPlayer(clients[x]))
            {
                GetClientName(clients[x],name,sizeof(name));
                if(StrEqual(action,"set")||StrEqual(action,"="))
                    newamt=StringToInt(amount)-GetXP(clients[x]);
                else if(StrEqual(action,"add")||StrEqual(action,"+"))
                    newamt=StringToInt(amount);
                else if(StrEqual(action,"take")||StrEqual(action,"-"))
                    newamt=-(StringToInt(amount));
                SetXP(clients[x],newamt,true);
                TransMgr_PrintText(clients[x], MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin XP has been set", adminname, newamt);
                TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin set XP", name, newamt);
            }
        }
        if(count==0)
            TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin invalid player");
    }
    else
        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin XP syntax");
    
    return Plugin_Handled;
}

public Action:AdminCmd_Level(client,argc)
{
    if(argc>=3)
    {
        decl String:action[8],String:target[64];
        decl String:level[8],String:adminname[64],String:name[64];
        new newlevel, clients[MaxClients];

        GetCmdArg(1,action,sizeof(action));
        GetCmdArg(2,target,sizeof(target));
        GetCmdArg(3,level,sizeof(level));
        GetClientName(client,adminname,sizeof(adminname));
        new count=FindMatchingPlayers(target,clients);
        for(new x=0;x<count;x++)
        {
            if(IsValidPlayer(clients[x]))
            {
                GetClientName(clients[x],name,sizeof(name));
                if(StrEqual(action,"set")||StrEqual(action,"="))
                    newlevel=StringToInt(level)-GetLevel(clients[x]);
                else if(StrEqual(action,"add")||StrEqual(action,"+"))
                    newlevel=StringToInt(level);
                else if(StrEqual(action,"take")||StrEqual(action,"-"))
                    newlevel=-(StringToInt(level));
                SetLevel(clients[x],newlevel,true);
                TransMgr_PrintText(clients[x], MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin level has been set",adminname,newlevel);
                TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin set level",name,newlevel);
            }
        }
        if(count==0)
            TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin invalid player");
    }
    else
        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin level syntax");
    
    return Plugin_Handled;
}

public Action:AdminCmd_Credits(client,argc)
{
    if(argc>=3)
    {
        decl String:action[8],String:target[64];
        decl String:amount[8],String:adminname[64],String:name[64];
        new newamt, clients[MaxClients];
        
        GetCmdArg(1,action,sizeof(action));
        GetCmdArg(2,target,sizeof(target));
        GetCmdArg(3,amount,sizeof(amount));
        GetClientName(client,adminname,sizeof(adminname));
        new count=FindMatchingPlayers(target,clients);
        for(new x=0;x<count;x++)
        {
            if(IsValidPlayer(clients[x]))
            {
                GetClientName(clients[x],name,sizeof(name));
                if(StrEqual(action,"set")||StrEqual(action,"="))
                    newamt=StringToInt(amount)-GetCredits(clients[x]);
                else if(StrEqual(action,"add")||StrEqual(action,"+"))
                    newamt=StringToInt(amount);
                else if(StrEqual(action,"take")||StrEqual(action,"-"))
                    newamt=-(StringToInt(amount));
                SetCredits(clients[x],newamt,true);
                TransMgr_PrintText(clients[x], MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin credits has been set",adminname,newamt);
                TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin set credits",name,newamt);
            }
        }
        if(count==0)
            TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin invalid player");
    }
    else
        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin credits syntax");
    
    return Plugin_Handled;
}

public Action:AdminCmd_ForcePurchase(client,argc)
{
    if(argc>=2)
    {
        decl String:target[64], String:givenupgrade[64];
        GetCmdArg(1,target,sizeof(target));
        GetCmdArg(2,givenupgrade,sizeof(givenupgrade));
        new Module:upgrade=ModuleMgr_FindByString(ModuleData_ShortName, givenupgrade, false);
        
        if(upgrade!=INVALID_MODULE && !ModuleMgr_IsDisabled(upgrade))
        {
            decl String:upgradename[MM_DATA_FULLNAME], String:adminname[64], String:name[64];
            new clients[MaxClients];
            
            GetUpgradeTranslatedFullName(client, upgrade, upgradename, sizeof(upgradename));

            GetClientName(client,adminname,sizeof(adminname));
            new count=FindMatchingPlayers(target,clients);
            for(new x=0;x<count;x++)
            {
                if(IsValidPlayer(clients[x]))
                {
                    GetClientName(clients[x],name,sizeof(name));
                    PurchaseUpgrade(clients[x],upgrade);
                    TransMgr_PrintText(clients[x], MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin forced to purchase",adminname,upgradename);
                    TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin force purchase",name,upgradename);
                }
            }
            if(count==0)
                TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin invalid player");
        }
        else
            TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin invalid upgrade");
    }
    else
        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin force purchase syntax");
    
    return Plugin_Handled;
}

public Action:AdminCmd_ForceSell(client,argc)
{
    if(argc>=2)
    {
        decl String:target[64], String:givenupgrade[64];
        GetCmdArg(1,target,sizeof(target));
        GetCmdArg(2,givenupgrade,sizeof(givenupgrade));
        new Module:upgrade=ModuleMgr_FindByString(ModuleData_ShortName, givenupgrade, false);

        if(upgrade!=INVALID_MODULE && !ModuleMgr_IsDisabled(upgrade))
        {
            decl String:upgradename[MM_DATA_FULLNAME], String:adminname[64], String:name[64];
            new clients[MaxClients];

            GetUpgradeTranslatedFullName(client, upgrade, upgradename, sizeof(upgradename));

            GetClientName(client,adminname,sizeof(adminname));
            new count=FindMatchingPlayers(target,clients);
            for(new x=0;x<count;x++)
            {
                if(IsValidPlayer(clients[x]))
                {
                    GetClientName(clients[x],name,sizeof(name));
                    SellUpgrade(clients[x],upgrade);
                    TransMgr_PrintText(clients[x], MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Admin forced to sell",adminname,upgradename);
                    TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin force sell",name,upgradename);
                }
            }
            if(count==0)
                TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin invalid player");
        }
        else
            TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin invalid upgrade");
    }
    else
        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Reply, INVALID_MODULE, false, "Admin force sell syntax");
    
    return Plugin_Handled;
}

public Action:AdminCmd_ResetPlayer(client,argc)
{
    if(argc>=1)
    {
        decl String:givenplayer[64], String:authstring[64], String:playername[64];
        GetCmdArg(1,givenplayer,sizeof(givenplayer));

        if(StrContains(givenplayer,"STEAM",false)>-1)
        {
            for(new i=1;i<=MaxClients;i++)
            {
                if(IsValidPlayer(i))
                {
                    GetClientAuthString(i,authstring,sizeof(authstring));
                    if(StrEqual(givenplayer,authstring,false))
                        ResetPlayer(i);
                }
            }
        }
        else
        {
            for(new i=1;i<=MaxClients;i++)
            {
                if(IsValidPlayer(i))
                {
                    GetClientName(i,playername,sizeof(playername));
                    if(StrEqual(givenplayer,playername,false))
                        ResetPlayer(i);
                }
            }
        }
    }

    return Plugin_Handled;
}

public Action:AdminCmd_ResetAllPlayers(client,argc)
{
    for(new i=1;i<=MaxClients;i++)
        if(IsValidPlayer(i))
            ResetPlayer(i);

    return Plugin_Handled;
}

public Action:AdminCmd_ResetDB(client,argc)
{
    // delete table from db
    ManageSQLTable(2);
    // remove all vector data
    ClearPlayersVector();
    // create table in db
    ManageSQLTable(1);
    // init all connected players
    for(new i=1;i<=MaxClients;i++)
        if(IsValidPlayer(i))
        {
            // reset global upgrade variables
            ResetUpgradeVariables(i);
            SetHurtXP(i,0);
            InitPlayer(i);
            LoadPlayerData(i);
        }

    return Plugin_Handled;
}

/******************************************************************************************
*                                   ADMIN MENU HANDLERS                                  *
******************************************************************************************/

CreateAdminMenu()
{
    new Handle:topmenu;
    if(LibraryExists("adminmenu") && ((topmenu=GetAdminTopMenu())!=INVALID_HANDLE))
    {
        OnAdminMenuReady(topmenu);
    }	
}

public OnLibraryRemoved(const String:name[])
{
    if(StrEqual(name,"adminmenu"))
    {
        hAdminMenu = INVALID_HANDLE;
    }
}

public OnAdminMenuReady(Handle:topmenu)
{
    // Block us from being called twice
    if(topmenu == hAdminMenu)
    {
        return;
    }
    hAdminMenu = topmenu;
    
    // Now add stuff to the menu: My very own category *yay*
    new TopMenuObject:rpgadmin_category = AddToTopMenu(
        hAdminMenu,						    // Menu
        "rpg_category",					    // Name
        TopMenuObject_Category,			    // Type
        Handle_RPGAdmin,				    // Callback
        INVALID_TOPMENUOBJECT,			    // Parent
        "",					                // cmdName
        THC_RPG_ADMINFLAGS	                // Admin flag
        );

    if(rpgadmin_category == INVALID_TOPMENUOBJECT)
    {
        // Error... lame...
        return;
    }

    AddToTopMenu(hAdminMenu,			    // Menu
                 "player_item",				// Name
                 TopMenuObject_Item,		// Type
                 Handle_PlayerItem,			// Callback
                 rpgadmin_category,			// Parent
                 "",					    // cmdName
                 THC_RPG_ADMINFLAGS	        // Admin flag
                );
    
    AddToTopMenu(hAdminMenu,			    // Menu
                 "resetall_item",			// Name
                 TopMenuObject_Item,		// Type
                 Handle_ResetAllItem,		// Callback
                 rpgadmin_category,			// Parent
                 "",					    // cmdName
                 THC_RPG_ADMINFLAGS	        // Admin flag
                );

    AddToTopMenu(hAdminMenu,			    // Menu
                 "resetdb_item",			// Name
                 TopMenuObject_Item,		// Type
                 Handle_ResetDBItem,		// Callback
                 rpgadmin_category,			// Parent
                 "",					    // cmdName
                 THC_RPG_ADMINFLAGS	        // Admin flag
                );
}

public Handle_RPGAdmin(Handle:topmenu, TopMenuAction:action, TopMenuObject:object_id, client, String:buffer[], maxlength)
{
    switch(action)
    {
        case TopMenuAction_DisplayTitle:
            Format(buffer, maxlength, "%T","admin_main_title", client);
        case TopMenuAction_DisplayOption:
            Format(buffer, maxlength, "%T","admin_main_title", client);
    }
}

public Handle_PlayerItem(Handle:topmenu, TopMenuAction:action, TopMenuObject:object_id, client, String:buffer[], maxlength)
{
    switch(action)
    {
        case TopMenuAction_DisplayTitle:
            Format(buffer, maxlength, "%T","admin_edit", client);
        case TopMenuAction_DisplayOption:
            Format(buffer, maxlength, "%T","admin_edit", client);
        case TopMenuAction_SelectOption:
            ShowPlayerSelectionMenu(client);
    }
}

public Handle_ResetAllItem(Handle:topmenu, TopMenuAction:action, TopMenuObject:object_id, client, String:buffer[], maxlength)
{
    switch(action)
    {
        case TopMenuAction_DisplayTitle:
            Format(buffer, maxlength, "%T","admin_resetallplayers", client);
        case TopMenuAction_DisplayOption:
            Format(buffer, maxlength, "%T","admin_resetallplayers", client);
        case TopMenuAction_SelectOption:
            ServerCommand("thc_rpg_resetallplayers");
    }
}

public Handle_ResetDBItem(Handle:topmenu, TopMenuAction:action, TopMenuObject:object_id, client, String:buffer[], maxlength)
{
    switch(action)
    {
        case TopMenuAction_DisplayTitle:
            Format(buffer, maxlength, "%T","admin_resetdb", client);
        case TopMenuAction_DisplayOption:
            Format(buffer, maxlength, "%T","admin_resetdb", client);
        case TopMenuAction_SelectOption:
            ServerCommand("thc_rpg_resetdb");
    }
}

/******************************************************************************************
*                           PLAYER SELECTION MENU HANDLERS                               *
******************************************************************************************/

void:ShowPlayerSelectionMenu(client)
{
    new Handle:hPlayerSelectionMenu = INVALID_HANDLE;

    hPlayerSelectionMenu = CreateMenu(Handle_PlayerSelectionMenu);

    SetMenuTitle(hPlayerSelectionMenu, "%T", "admin_choose_player_title", client);
    SetMenuExitButton(hPlayerSelectionMenu, true);
    SetMenuExitBackButton(hPlayerSelectionMenu, true);
    
    // Now add players to it
    AddTargetsToMenu(hPlayerSelectionMenu, 0);
    
    // Send the menu to our admin
    DisplayMenu(hPlayerSelectionMenu, client, 30);
}

public Handle_PlayerSelectionMenu(Handle:PlayerSelectionMenu, MenuAction:action, client, param)
{
    switch(action)
    {
        case MenuAction_Select:
        {
            decl String:info[5];
            GetMenuItem(PlayerSelectionMenu, param, info, sizeof(info));
            new target=GetClientOfUserId(StringToInt(info));

            ShowPlayerMenu(client, target);
        }
            
        case MenuAction_Cancel:
            // param gives us the reason why the menu was cancelled
            if(param == MenuCancel_ExitBack)
                RedisplayAdminMenu(hAdminMenu, client);
        
        case MenuAction_End:
            CloseHandle(PlayerSelectionMenu);
    }
}

/******************************************************************************************
*                                    RPG MENU HANDLERS                                   *
******************************************************************************************/

void:ShowPlayerMenu(client, target)
{
    decl String:item[128],String:starget[8];

    IntToString(target, starget, sizeof(starget));

    new Handle:hPlayerMenu = CreateMenu(Handle_PlayerMenu);

    SetMenuTitle(hPlayerMenu, "%T", "admin_choose_option_title", client);
    SetMenuExitButton(hPlayerMenu, true);
    SetMenuExitBackButton(hPlayerMenu, true);
    
    // Now add item to it
    Format(item, sizeof(item), "%T", "admin_xp_item", client);
    AddMenuItem(hPlayerMenu, starget, item);
    Format(item, sizeof(item), "%T", "admin_level_item", client);
    AddMenuItem(hPlayerMenu, starget, item);
    Format(item, sizeof(item), "%T", "admin_credits_item", client);
    AddMenuItem(hPlayerMenu, starget, item);
    Format(item, sizeof(item), "%T", "admin_purchase_item", client);
    AddMenuItem(hPlayerMenu, starget, item);
    Format(item, sizeof(item), "%T", "admin_sell_item", client);
    AddMenuItem(hPlayerMenu, starget, item);
    Format(item, sizeof(item), "%T", "admin_reset_item", client);
    AddMenuItem(hPlayerMenu, starget, item);

    // Send the menu to our admin
    DisplayMenu(hPlayerMenu, client, 30);
}

public Handle_PlayerMenu(Handle:PlayerMenu, MenuAction:action, client, param)
{
    decl String:info[5];
    GetMenuItem(PlayerMenu, param, info, sizeof(info));
    new target=StringToInt(info);

    switch(action)
    {
        case MenuAction_Select:
        {
            switch(param)
            {
                case 0:
                    ShowXPMenu(client, target);
                case 1:
                    ShowLevelMenu(client, target);
                case 2:
                    ShowCreditsMenu(client, target);
                case 3:
                    ShowForcePurchaseMenu(client, target, 0);
                case 4:
                    ShowForceSellMenu(client, target, 0);
                case 5:
                    ResetPlayer(target);
            }
        }
            
        case MenuAction_Cancel:
            // param gives us the reason why the menu was cancelled
            if(param == MenuCancel_ExitBack)
                ShowPlayerSelectionMenu(client);
        
        case MenuAction_End:
            CloseHandle(PlayerMenu);
    }
}

/******************************************************************************************
*                                      XP MENU HANDLERS                                  *
******************************************************************************************/

ShowXPMenu(client, target)
{
    decl String:starget[8];

    IntToString(target, starget, sizeof(starget));

    new Handle:hXPMenu = CreateMenu(Handle_XPMenu);

    SetMenuTitle(hXPMenu, "%T", "admin_xp_title", client);
    SetMenuExitButton(hXPMenu, true);
    
    // Now add item to it
    AddMenuItem(hXPMenu, starget, "-100 XP");
    AddMenuItem(hXPMenu, starget, "-50 XP");
    AddMenuItem(hXPMenu, starget, "-5 XP");
    AddMenuItem(hXPMenu, starget, "+5 XP");
    AddMenuItem(hXPMenu, starget, "+50 XP");
    AddMenuItem(hXPMenu, starget, "+100 XP");

    // Send the menu to our admin
    DisplayMenu(hXPMenu, client, 30);
}

public Handle_XPMenu(Handle:XPMenu, MenuAction:action, client, param)
{
    decl String:info[5];
    GetMenuItem(XPMenu, param, info, sizeof(info));
    new target=StringToInt(info);

    switch(action)
    {
        case MenuAction_Select:
        {
            switch(param)
            {
                case 0:
                    Exec_XP(AdminCmdEvent_XP_min100, target);
                case 1:
                    Exec_XP(AdminCmdEvent_XP_min50, target);
                case 2:
                    Exec_XP(AdminCmdEvent_XP_min5, target);
                case 3:
                    Exec_XP(AdminCmdEvent_XP_plus5, target);
                case 4:
                    Exec_XP(AdminCmdEvent_XP_plus50, target);
                case 5:
                    Exec_XP(AdminCmdEvent_XP_plus100, target);
            }
            
            // Now display the menu again
            ShowXPMenu(client, target);
        }
            
        case MenuAction_Cancel:
            // param gives us the reason why the menu was cancelled
            if(param == MenuCancel_ExitBack)
                ShowPlayerMenu(client, target);

        case MenuAction_End:
            CloseHandle(XPMenu);
    }
}

/******************************************************************************************
*                                   LEVEL MENU HANDLERS                                  *
******************************************************************************************/

ShowLevelMenu(client, target)
{
    decl String:starget[8];

    IntToString(target, starget, sizeof(starget));

    new Handle:hLevelMenu = CreateMenu(Handle_LevelMenu);

    SetMenuTitle(hLevelMenu, "%T", "admin_level_title", client);
    SetMenuExitButton(hLevelMenu, true);

    // Now add items to it
    AddMenuItem(hLevelMenu, starget, "-50 Levels");
    AddMenuItem(hLevelMenu, starget, "-10 Levels");
    AddMenuItem(hLevelMenu, starget, "-5 Level");
    AddMenuItem(hLevelMenu, starget, "+5 Level");
    AddMenuItem(hLevelMenu, starget, "+10 Levels");
    AddMenuItem(hLevelMenu, starget, "+50 Levels");

    // Send the menu to our admin
    DisplayMenu(hLevelMenu, client, 30);
}

public Handle_LevelMenu(Handle:LevelMenu, MenuAction:action, client, param)
{
    decl String:info[5];
    GetMenuItem(LevelMenu, param, info, sizeof(info));
    new target=StringToInt(info);

    switch(action)
    {
        case MenuAction_Select:
        {
            switch(param)
            {
                case 0:
                    Exec_Level(AdminCmdEvent_Level_min50, target);
                case 1:
                    Exec_Level(AdminCmdEvent_Level_min10, target);
                case 2:
                    Exec_Level(AdminCmdEvent_Level_min5, target);
                case 3:
                    Exec_Level(AdminCmdEvent_Level_plus5, target);
                case 4:
                    Exec_Level(AdminCmdEvent_Level_plus10, target);
                case 5:
                    Exec_Level(AdminCmdEvent_Level_plus50, target);
            }
            
            // Now display the menu again
            ShowLevelMenu(client, target);
        }
            
        case MenuAction_Cancel:
            // param gives us the reason why the menu was cancelled
            if(param == MenuCancel_ExitBack)
                ShowPlayerMenu(client, target);
                
        case MenuAction_End:
            CloseHandle(LevelMenu);
    }
}

/******************************************************************************************
*                                 CREDITS MENU HANDLERS                                  *
******************************************************************************************/

ShowCreditsMenu(client, target)
{
    decl String:starget[8];

    IntToString(target, starget, sizeof(starget));

    new Handle:hCreditsMenu = CreateMenu(Handle_CreditsMenu);

    SetMenuTitle(hCreditsMenu, "%T", "admin_credits_title", client);
    SetMenuExitButton(hCreditsMenu, true);

    // Now add players to it
    AddMenuItem(hCreditsMenu, starget, "-100 Credits");
    AddMenuItem(hCreditsMenu, starget, "-50 Credits");
    AddMenuItem(hCreditsMenu, starget, "-5 Credits");
    AddMenuItem(hCreditsMenu, starget, "+5 Credits");
    AddMenuItem(hCreditsMenu, starget, "+50 Credits");
    AddMenuItem(hCreditsMenu, starget, "+100 Credits");

    // Send the menu to our admin
    DisplayMenu(hCreditsMenu, client, 30);
}

public Handle_CreditsMenu(Handle:CreditsMenu, MenuAction:action, client, param)
{
    decl String:info[5];
    GetMenuItem(CreditsMenu, param, info, sizeof(info));
    new target=StringToInt(info);

    switch(action)
    {
        case MenuAction_Select:
        {
            switch(param)
            {
                case 0:
                    Exec_Credits(AdminCmdEvent_Credits_min100, target);
                case 1:
                    Exec_Credits(AdminCmdEvent_Credits_min50, target);
                case 2:
                    Exec_Credits(AdminCmdEvent_Credits_min5, target);
                case 3:
                    Exec_Credits(AdminCmdEvent_Credits_plus5, target);
                case 4:
                    Exec_Credits(AdminCmdEvent_Credits_plus50, target);
                case 5:
                    Exec_Credits(AdminCmdEvent_Credits_plus100, target);
            }

            // Now display the menu again
            ShowCreditsMenu(client, target);
        }

        case MenuAction_Cancel:
            // param gives us the reason why the menu was cancelled
            if(param == MenuCancel_ExitBack)
                ShowPlayerMenu(client, target);
                
        case MenuAction_End:
            CloseHandle(CreditsMenu);
    }
}

/******************************************************************************************
*                           FORCE PURCHASE MENU HANDLERS                                 *
******************************************************************************************/

public ShowForcePurchaseMenu(client, target, firstitem)
{
    decl String:starget[8];
    decl String:upgradeshortname[MM_DATA_SHORTNAME], String:upgradename[MM_DATA_FULLNAME], String:info[32];
    new Module:upgrade;
    
    IntToString(target, starget, sizeof(starget));

    new Handle:hForcePurchaseMenu = CreateMenu(Handle_ForcePurchaseMenu);

    SetMenuTitle(hForcePurchaseMenu, "%T", "admin_purchase_title", client);
    SetMenuExitButton(hForcePurchaseMenu, true);

    for(new moduleindex=1;moduleindex<MODULE_COUNT;moduleindex++)
    {
        upgrade = Module:moduleindex;
        ModuleMgr_ReadString(upgrade, ModuleData_ShortName, upgradeshortname, sizeof(upgradeshortname));
        ModuleMgr_ReadString(upgrade, ModuleData_FullName, upgradename, sizeof(upgradename));

        StrCat(info, sizeof(info), starget);
        StrCat(info, sizeof(info), " ");
        StrCat(info, sizeof(info), upgradeshortname);

        if(!IsUpgradeMaxed(target,upgrade))
            AddMenuItem(hForcePurchaseMenu,info,upgradename);
        else
            AddMenuItem(hForcePurchaseMenu,info,upgradename,ITEMDRAW_DISABLED);
    }

    // Send the menu to our admin
    DisplayMenuAtItem(hForcePurchaseMenu, client, firstitem, 30);
}

public Handle_ForcePurchaseMenu(Handle:ForcePurchaseMenu, MenuAction:action, client, param)
{
    decl String:info[64];
    decl String:infoparts[2][64];
    GetMenuItem(ForcePurchaseMenu, param, info, sizeof(info));
    ExplodeString(info, " ", infoparts, 2, 64);
    new target=StringToInt(infoparts[0]);

    switch(action)
    {
        case MenuAction_Select:
        {
            Exec_ForcePurchase(infoparts[1], target);

            // Now display the menu again
            ShowForcePurchaseMenu(client, target, GetMenuSelectionPosition());
        }

        case MenuAction_Cancel:
            // param gives us the reason why the menu was cancelled
            if(param == MenuCancel_ExitBack)
                ShowPlayerMenu(client, target);
                
        case MenuAction_End:
            CloseHandle(ForcePurchaseMenu);
    }
}

/******************************************************************************************
*                               FORCE SELL MENU HANDLERS                                 *
******************************************************************************************/

ShowForceSellMenu(client, target, firstitem)
{
    decl String:starget[8];
    decl String:upgradeshortname[MM_DATA_SHORTNAME], String:upgradename[MM_DATA_FULLNAME], String:info[32];
    new Module:upgrade;
    
    IntToString(target, starget, sizeof(starget));

    new Handle:hForceSellMenu = CreateMenu(Handle_ForceSellMenu);

    SetMenuTitle(hForceSellMenu, "%T", "admin_sell_title", client);
    SetMenuExitButton(hForceSellMenu, true);

    for(new moduleindex=1;moduleindex<MODULE_COUNT;moduleindex++)
    {
        upgrade = Module:moduleindex;
        ModuleMgr_ReadString(upgrade, ModuleData_ShortName, upgradeshortname, sizeof(upgradeshortname));
        ModuleMgr_ReadString(upgrade, ModuleData_FullName, upgradename, sizeof(upgradename));

        StrCat(info, sizeof(info), starget);
        StrCat(info, sizeof(info), " ");
        StrCat(info, sizeof(info), upgradeshortname);
            
        if(GetPlayerUpgradeLevel(target,upgrade)>0)
            AddMenuItem(hForceSellMenu,info,upgradename);
        else
            AddMenuItem(hForceSellMenu,info,upgradename,ITEMDRAW_DISABLED);
    }

    // Send the menu to our admin
    DisplayMenuAtItem(hForceSellMenu, client, firstitem, 30);
}

public Handle_ForceSellMenu(Handle:ForceSellMenu, MenuAction:action, client, param)
{
    decl String:info[64];
    decl String:infoparts[2][64];
    GetMenuItem(ForceSellMenu, param, info, sizeof(info));
    ExplodeString(info, " ", infoparts, 2, 64);
    new target=StringToInt(infoparts[0]);

    switch(action)
    {
        case MenuAction_Select:
        {
            Exec_ForceSell(infoparts[1], target);

            // Now display the menu again
            ShowForceSellMenu(client, target, GetMenuSelectionPosition());
        }

        case MenuAction_Cancel:
            // param gives us the reason why the menu was cancelled
            if(param == MenuCancel_ExitBack)
                ShowPlayerMenu(client, target);

        case MenuAction_End:
            CloseHandle(ForceSellMenu);
    }
}

/******************************************************************************************
*                               EXECUTE ADMIN COMMANDS                                   *
******************************************************************************************/

Exec_XP(AdminCmdEvent:event, target)
{
    decl String:userID[64];
    
    IntToString(GetClientUserId(target), userID, sizeof(userID));

    switch(event)
    {
        case AdminCmdEvent_XP_min100:
        {
            new String:command[64] = "thc_rpg_xp take @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 100");
            ServerCommand(command);
        }
        case AdminCmdEvent_XP_min50:
        {
            new String:command[64] = "thc_rpg_xp take @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 50");
            ServerCommand(command);
        }
        case AdminCmdEvent_XP_min5:
        {
            new String:command[64] = "thc_rpg_xp take @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 5");
            ServerCommand(command);
        }
        case AdminCmdEvent_XP_plus5:
        {
            new String:command[64] = "thc_rpg_xp add @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 5");
            ServerCommand(command);
        }
        case AdminCmdEvent_XP_plus50:
        {
            new String:command[64] = "thc_rpg_xp add @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 50");
            ServerCommand(command);
        }
        case AdminCmdEvent_XP_plus100:
        {
            new String:command[64] = "thc_rpg_xp add @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 100");
            ServerCommand(command);
        }
    }		
}

Exec_Level(AdminCmdEvent:event, target)
{
    decl String:userID[64];
    
    IntToString(GetClientUserId(target), userID, sizeof(userID));

    switch(event)
    {
        case AdminCmdEvent_Level_min50:
        {
            new String:command[64] = "thc_rpg_level take @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 50");
            ServerCommand(command);
        }
        case AdminCmdEvent_Level_min10:
        {
            new String:command[64] = "thc_rpg_level take @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 10");
            ServerCommand(command);
        }
        case AdminCmdEvent_Level_min5:
        {
            new String:command[64] = "thc_rpg_level take @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 5");
            ServerCommand(command);
        }
        case AdminCmdEvent_Level_plus5:
        {
            new String:command[64] = "thc_rpg_level add @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 5");
            ServerCommand(command);
        }
        case AdminCmdEvent_Level_plus10:
        {
            new String:command[64] = "thc_rpg_level add @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 10");
            ServerCommand(command);
        }
        case AdminCmdEvent_Level_plus50:
        {
            new String:command[64] = "thc_rpg_level add @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 50");
            ServerCommand(command);
        }
    }		
}

Exec_Credits(AdminCmdEvent:event, target)
{
    decl String:userID[64];
    
    IntToString(GetClientUserId(target), userID, sizeof(userID));

    switch(event)
    {
        case AdminCmdEvent_Credits_min100:
        {
            new String:command[64] = "thc_rpg_credits take @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 100");
            ServerCommand(command);
        }
        case AdminCmdEvent_Credits_min50:
        {
            new String:command[64] = "thc_rpg_credits take @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 50");
            ServerCommand(command);
        }
        case AdminCmdEvent_Credits_min5:
        {
            new String:command[64] = "thc_rpg_credits take @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 5");
            ServerCommand(command);
        }
        case AdminCmdEvent_Credits_plus5:
        {
            new String:command[64] = "thc_rpg_credits add @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 5");
            ServerCommand(command);
        }
        case AdminCmdEvent_Credits_plus50:
        {
            new String:command[64] = "thc_rpg_credits add @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 50");
            ServerCommand(command);
        }
        case AdminCmdEvent_Credits_plus100:
        {
            new String:command[64] = "thc_rpg_credits add @";
            StrCat(command, sizeof(command), userID);
            StrCat(command, sizeof(command), " 100");
            ServerCommand(command);
        }
    }		
}

Exec_ForcePurchase(String:upgradename[], target)
{
    decl String:userID[64];
    
    IntToString(GetClientUserId(target), userID, sizeof(userID));

    new String:command[64] = "thc_rpg_forcepurchase @";
    StrCat(command, sizeof(command), userID);
    StrCat(command, sizeof(command), " ");
    StrCat(command, sizeof(command), upgradename);
    ServerCommand(command);
}

Exec_ForceSell(String:upgradename[], target)
{
    decl String:userID[64];
    
    IntToString(GetClientUserId(target), userID, sizeof(userID));

    new String:command[64] = "thc_rpg_forcesell @";
    StrCat(command, sizeof(command), userID);
    StrCat(command, sizeof(command), " ");
    StrCat(command, sizeof(command), upgradename);
    ServerCommand(command);
}
