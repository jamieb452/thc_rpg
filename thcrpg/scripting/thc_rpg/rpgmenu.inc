/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          rpgmenu.inc
 *  Type:          Core
 *  Description:   contains the RPGMenu for players
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

RPGMenu(client)
{
    decl String:item[MM_DATA_DESCRIPTION];

    new Handle:menu_main=CreateMenu(RPGMenuHandle);

    SetMenuTitle(menu_main, "%T", "main_title", client);

    Format(item, sizeof(item), "%T", "upgrade_item", client);
    AddMenuItem(menu_main,item,item,(MODULE_COUNT>0)?ITEMDRAW_DEFAULT:ITEMDRAW_DISABLED);

    if(Auth_ClientHasFlags(client, g_CoreConfigCache[CoreConfig_vip_flags], ",", Auth_Either))
    {
        Format(item, sizeof(item), "TEST VIP MENU");
        AddMenuItem(menu_main,item,item,(MODULE_COUNT>0)?ITEMDRAW_DEFAULT:ITEMDRAW_DISABLED);
    }
    
    Format(item, sizeof(item), "%T", "sell_item", client);
    AddMenuItem(menu_main,item,item,(MODULE_COUNT>0)?ITEMDRAW_DEFAULT:ITEMDRAW_DISABLED);

    Format(item, sizeof(item), "%T", "stats_item", client);
    AddMenuItem(menu_main,item,item);

    Format(item, sizeof(item), "%T", "info_item", client);
    AddMenuItem(menu_main,item,item,(MODULE_COUNT>0)?ITEMDRAW_DEFAULT:ITEMDRAW_DISABLED);

    DisplayMenu(menu_main,client,MENU_TIME_FOREVER);
}

public RPGMenuHandle(Handle:menu_main,MenuAction:action,client,slot)
{
    if(action==MenuAction_Select)
    {
        switch(slot)
        {
            case 0:
                UpgradeMenu(client,0);
            case 1:
                SellMenu(client,0);
            case 2:
                StatsMenu(client);
            case 3:
                InfoMenu(client);
        }
    }
    if(action==MenuAction_End)
        CloseHandle(menu_main);
}

UpgradeMenu(client,firstitem)
{
    decl String:upgradeid[4],String:item[MM_DATA_DESCRIPTION];
    decl String:upgradename[MM_DATA_FULLNAME];
    new TeamLock, level;
    new Module:upgrade;
    
    new Handle:menu_upgrades=CreateMenu(UpgradeHandle);
    SetMenuExitBackButton(menu_upgrades,true);

    SetMenuTitle(menu_upgrades, "%T", "upgrades_title", client, GetCredits(client));

    for(new moduleindex=1;moduleindex<MODULE_COUNT;moduleindex++)
    {
        // Read moduleindex as a Module type.
        upgrade = Module:moduleindex;
        IntToString(moduleindex,upgradeid,sizeof(upgradeid));

        GetUpgradeTranslatedFullName(client,upgrade,upgradename,sizeof(upgradename));

        TeamLock=GetUpgradeConfigTeamLock(upgrade);
        level = GetPlayerUpgradeLevel(client,upgrade);

        if(ModuleMgr_IsDisabled(upgrade))
        {
            Format(item,sizeof(item), "%T", "upgrade_disabled", client, upgradename);
            // Dont show the menu entry!!
            // AddMenuItem(menu_upgrades,upgradeid,item,ITEMDRAW_DISABLED);
        }
        else if(GetClientTeam(client)==TeamLock || IsUpgradeInterlocked(client, upgrade))
        {
            Format(item,sizeof(item), "%T", "upgrade_locked", client, upgradename);
            AddMenuItem(menu_upgrades,upgradeid,item,ITEMDRAW_DISABLED);
        }
        else if(IsUpgradeMaxed(client,upgrade))
        {
            Format(item,sizeof(item), "%T", "upgrade_maxed", client, upgradename);
            AddMenuItem(menu_upgrades,upgradeid,item,ITEMDRAW_DISABLED);
        }
        else
        {
            Format(item,sizeof(item), "%T", "upgrade_purchase",client,upgradename,level+1,GetUpgradeConfigMaxLevel(upgrade),GetUpgradeConfigCost(upgrade)+(level*GetUpgradeConfigiCost(upgrade)));
            AddMenuItem(menu_upgrades,upgradeid,item);
        }
    }
    if(MODULE_COUNT>0)
        DisplayMenuAtItem(menu_upgrades,client,firstitem,MENU_TIME_FOREVER);
    else
        RPGMenu(client);
}

public UpgradeHandle(Handle:menu_upgrades,MenuAction:action,client,slot)
{
    if(action==MenuAction_Select)
    {
        decl String:upgradeid[4];
        GetMenuItem(menu_upgrades,slot,upgradeid,sizeof(upgradeid));
        new Module:upgrade = Module:StringToInt(upgradeid);
        PurchaseUpgrade(client,upgrade);
        UpgradeMenu(client,GetMenuSelectionPosition());
    }
    if(action==MenuAction_Cancel)
        if(slot==MenuCancel_ExitBack)
            RPGMenu(client);
    if(action==MenuAction_End)
        CloseHandle(menu_upgrades);
}

SellMenu(client,firstitem)
{
    decl String:upgradeid[4],String:item[MM_DATA_DESCRIPTION];
    decl String:upgradename[MM_DATA_FULLNAME];
    new TeamLock, level;
    new Module:upgrade;
    
    new Handle:menu_sell=CreateMenu(SellHandle);
    SetMenuExitBackButton(menu_sell,true);

    SetMenuTitle(menu_sell, "%T", "sell_title", client, GetCredits(client));

    for(new moduleindex=1;moduleindex<MODULE_COUNT;moduleindex++)
    {
        // Read moduleindex as a Module type.
        upgrade = Module:moduleindex;
        IntToString(moduleindex,upgradeid,sizeof(upgradeid));

        GetUpgradeTranslatedFullName(client,upgrade,upgradename,sizeof(upgradename));

        TeamLock=GetUpgradeConfigTeamLock(upgrade);
        level = GetPlayerUpgradeLevel(client,upgrade);

        if(ModuleMgr_IsDisabled(upgrade))
        {
            Format(item,sizeof(item), "%T", "upgrade_disabled", client, upgradename);
            // Dont show the menu entry!!
            // AddMenuItem(menu_sell,upgradeid,item,ITEMDRAW_DISABLED);
        }
        else if(GetClientTeam(client)==TeamLock)
        {
            Format(item,sizeof(item), "%T", "upgrade_locked", client, upgradename);
            AddMenuItem(menu_sell,upgradeid,item,ITEMDRAW_DISABLED);
        }
        else if(level>0)
        {
            new cost=GetUpgradeConfigCost(upgrade)+((level-1)*GetUpgradeConfigiCost(upgrade));
            new refund=RoundToNearest(cost*g_CoreConfigCache[CoreConfig_save_percent]);
            Format(item,sizeof(item), "%T", "upgrade_sell",client,upgradename,GetPlayerUpgradeLevel(client,upgrade),GetUpgradeConfigMaxLevel(upgrade),refund);
            AddMenuItem(menu_sell,upgradeid,item);
        }
    }
    if(GetPlayerUpgradeCount(client)>0)
        DisplayMenuAtItem(menu_sell,client,firstitem,MENU_TIME_FOREVER);
    else
        RPGMenu(client);
}

public SellHandle(Handle:menu_sell,MenuAction:action,client,slot)
{
    if(action==MenuAction_Select)
    {
        decl String:upgradeid[4];
        GetMenuItem(menu_sell,slot,upgradeid,sizeof(upgradeid));
        new Module:upgrade = Module:StringToInt(upgradeid);
        SellUpgrade(client,upgrade);
        SellMenu(client,GetMenuSelectionPosition());
    }
    if(action==MenuAction_Cancel)
        if(slot==MenuCancel_ExitBack)
            RPGMenu(client);
    if(action==MenuAction_End)
        CloseHandle(menu_sell);
}

StatsMenu(client)
{
    decl String:item[MM_DATA_DESCRIPTION];

    new Handle:menu_stats=CreateMenu(StatsHandle);
    SetMenuExitBackButton(menu_stats,true);

    SetMenuTitle(menu_stats, "%T", "stats_title", client);

    Format(item, sizeof(item), "%T", "stats_reset_item", client);
    AddMenuItem(menu_stats,item,item);

    Format(item, sizeof(item), "%T", "stats_level_item", client, GetLevel(client));
    AddMenuItem(menu_stats,item,item,ITEMDRAW_DISABLED);

    Format(item, sizeof(item), "%T", "stats_xp_item", client, GetXP(client), GetReqXP(client));
    AddMenuItem(menu_stats,item,item,ITEMDRAW_DISABLED);

    Format(item, sizeof(item), "%T", "stats_credits_item", client, GetCredits(client));
    AddMenuItem(menu_stats,item,item,ITEMDRAW_DISABLED);

    DisplayMenu(menu_stats,client,MENU_TIME_FOREVER);
}

public StatsHandle(Handle:menu_stats,MenuAction:action,client,slot)
{
    if(action==MenuAction_Select)
        if(slot==0)
            ResetStatsMenu(client);
    if(action==MenuAction_Cancel)
        if(slot==MenuCancel_ExitBack)
            RPGMenu(client);
    if(action==MenuAction_End)
        CloseHandle(menu_stats);
}

ResetStatsMenu(client)
{
    decl String:item[MM_DATA_DESCRIPTION];

    new Handle:menu_resetstats=CreateMenu(ResetStatsHandle);

    SetMenuTitle(menu_resetstats, "%T", "stats_reset_warning", client);

    Format(item, sizeof(item), "%T", "stats_reset_yes", client);
    AddMenuItem(menu_resetstats,item,item);

    Format(item, sizeof(item), "%T", "stats_reset_no", client);
    AddMenuItem(menu_resetstats,item,item);

    DisplayMenu(menu_resetstats,client,MENU_TIME_FOREVER);
}

public ResetStatsHandle(Handle:menu_resetstats,MenuAction:action,client,slot)
{
    if(action==MenuAction_Select)
    {
        if(slot==0)
            ResetPlayer(client);
        StatsMenu(client);
    }
    if(action==MenuAction_End)
        CloseHandle(menu_resetstats);
}

InfoMenu(client)
{
    decl String:upgradeid[4],String:item[MM_DATA_FULLNAME+MM_DATA_DESCRIPTION];
    decl String:upgradename[MM_DATA_FULLNAME],String:upgradedescription[MM_DATA_DESCRIPTION];
    new Module:upgrade;
    
    new Handle:menu_info=CreateMenu(InfoHandle);
    SetMenuExitBackButton(menu_info,true);

    SetMenuTitle(menu_info, "%T", "info_title", client);

    for(new moduleindex=1;moduleindex<MODULE_COUNT;moduleindex++)
    {
        // Read moduleindex as a Module type.
        upgrade = Module:moduleindex;
        IntToString(moduleindex,upgradeid,sizeof(upgradeid));

        GetUpgradeTranslatedFullName(client,upgrade,upgradename,sizeof(upgradename));
        GetUpgradeTranslatedDescription(client,upgrade,upgradedescription,sizeof(upgradedescription));

        new TeamLock=GetUpgradeConfigTeamLock(upgrade);

        if(!ModuleMgr_IsDisabled(upgrade)&&GetClientTeam(client)!=TeamLock)
        {
            Format(item,sizeof(item),"%s",upgradename,upgradedescription);
            AddMenuItem(menu_info,upgradeid,item);
        }
    }
    if(MODULE_COUNT>0)
        DisplayMenu(menu_info,client,MENU_TIME_FOREVER);
    else
        CloseHandle(menu_info);
}

public InfoHandle(Handle:menu_info,MenuAction:action,client,slot)
{
	if(action==MenuAction_Select)
	{
		new String:item[MM_DATA_DESCRIPTION];
		
		new String:upgradeid[4];
		GetMenuItem(menu_info, slot, upgradeid, 4);
		new Module:upgrade = Module:StringToInt(upgradeid);
		GetUpgradeTranslatedDescription(client,upgrade,item,MM_DATA_DESCRIPTION);
		TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, true, item);
		InfoMenu(client);
	}
	else if(action==MenuAction_Cancel)
        if(slot==MenuCancel_ExitBack)
            RPGMenu(client);
	else if(action==MenuAction_End)
        CloseHandle(menu_info);
}
