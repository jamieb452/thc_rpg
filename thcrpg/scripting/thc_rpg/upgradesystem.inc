/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          upgradesystem.inc
 *  Type:          Core
 *  Description:   contains Upgrade specific functions
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

#define UPGRADE_OFFSET_LEVEL 4
#define UPGRADE_OFFSET_COST 5

PurchaseUpgrade(client,Module:upgrade)
{
    if(IsValidPlayer(client))
    {
        if(!IsUpgradeInterlocked(client, upgrade))
        {
            new isbot=IsFakeClient(client);
            if(g_CoreConfigCache[CoreConfig_bot_enable]||!g_CoreConfigCache[CoreConfig_bot_enable]&&!isbot)
            {
                if(g_CoreConfigCache[CoreConfig_bot_control]&&isbot)
                {
                    new count=CountHumans();
                    if(!count)
                        return -1;
                }
                decl String:name[MM_DATA_FULLNAME],String:translatedname[MM_DATA_FULLNAME];
                ModuleMgr_ReadString(upgrade, ModuleData_FullName, name, sizeof(name));
                GetUpgradeTranslatedFullName(client,upgrade,translatedname,sizeof(translatedname));
                new level=GetPlayerUpgradeLevel(client,upgrade);
                if(level<=GetUpgradeConfigMaxLevel(upgrade))
                {
                    new credits=GetCredits(client);
                    new cost=GetUpgradeConfigCost(upgrade)+(level*GetUpgradeConfigiCost(upgrade));
                    if(credits>=cost)
                    {
                        SetPlayerUpgradeLevel(client,upgrade,level+1);
                        SetCredits(client,-cost,false);
                        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Successfully purchase upgrade", translatedname, level+1);
                        LogMgr_Print(g_moduleCore, LogType_Debug, "PurchaseUpgrade", "Client %d purchased upgrade %s at level %d", client, name, level+1);
                        if(!StrEqual(g_CoreConfigCache[CoreConfig_sound_purchase], "off", false))
                            ClientCommand(client,"play %s", g_CoreConfigCache[CoreConfig_sound_purchase]);

                        // Call forward
                        new any:eventdata[2][1];
                        eventdata[0][0] = client;
                        eventdata[1][0] = level+1;
                        new any:modules[2];
                        modules[0] = upgrade;
                        modules[1] = INVALID_MODULE;
                        EventMgr_Forward(g_EvOnUpgradePurchase, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType3, modules);
                    }
                    else
                        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Not enough credits for upgrade", translatedname, level+1, cost);
                }
                else
                {
                    TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "At max level for upgrade");
                    LogMgr_Print(g_moduleCore, LogType_Debug, "PurchaseUpgrade", "Client %d is at maxlevel for upgrade %s", client, name);
                }

                if(!StrEqual(g_CoreConfigCache[CoreConfig_sound_error], "off", false))
                    ClientCommand(client,"play %s", g_CoreConfigCache[CoreConfig_sound_error]);
                return -1;
            }
            else
                return 0;
        }
        else
        {
            TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Upgrade is interlocked");
            LogMgr_Print(g_moduleCore, LogType_Debug, "PurchaseUpgrade", "Client %d tried to buy interlocked upgrade %s", client, name);
        }
    }
    else
        return -1;

    return 1;
}

SellUpgrade(client,Module:upgrade)
{
    if(IsValidPlayer(client))
    {
        new isbot=IsFakeClient(client);
        if(g_CoreConfigCache[CoreConfig_bot_enable]||!g_CoreConfigCache[CoreConfig_bot_enable]&&!isbot)
        {
            if(g_CoreConfigCache[CoreConfig_bot_control]&&isbot)
            {
                new count=CountHumans();
                if(!count)
                    return -1;
            }
            decl String:name[MM_DATA_FULLNAME],String:translatedname[MM_DATA_FULLNAME];
            ModuleMgr_ReadString(upgrade, ModuleData_FullName, name, sizeof(name));
            GetUpgradeTranslatedFullName(client,upgrade,translatedname,sizeof(translatedname));
            new level=GetPlayerUpgradeLevel(client,upgrade);
            if(level>0)
            {
                new cost=GetUpgradeConfigCost(upgrade)+((level-1)*GetUpgradeConfigiCost(upgrade));
                new refund=RoundToNearest(cost*g_CoreConfigCache[CoreConfig_save_percent]);
                SetCredits(client,refund,true);
                SetPlayerUpgradeLevel(client,upgrade,level-1);
                TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Sell upgrade", translatedname, level, refund);
                LogMgr_Print(g_moduleCore, LogType_Debug, "SellUpgrade", "Client %d sold upgrade %s level %d", client, name, level);
                if(!StrEqual(g_CoreConfigCache[CoreConfig_sound_sell], "off", false))
                    ClientCommand(client,"play %s", g_CoreConfigCache[CoreConfig_sound_sell]);

                // Call forward
                new any:eventdata[2][1];
                eventdata[0][0] = client;
                eventdata[1][0] = level-1;
                new any:modules[2];
                modules[0] = upgrade;
                modules[1] = INVALID_MODULE;
                EventMgr_Forward(g_EvOnUpgradeSell, eventdata, sizeof(eventdata), sizeof(eventdata[]), g_CommonDataType3, modules);                    
            }
            else
                return -1;
        }
        else
            return 0;
    }
    else
        return -1;

    return 1;
}

BotBuyRandomUpgrade(client)
{
    if(IsValidPlayer(client))
    {
        new isbot=IsFakeClient(client);
        if(g_CoreConfigCache[CoreConfig_bot_enable]&&isbot==1)
        {
            if(g_CoreConfigCache[CoreConfig_bot_control])
            {
                new count=CountHumans();
                if(!count)
                    return -1;
            }
            new Module:upgrade;
            for(new moduleindex=1;moduleindex<MODULE_COUNT;moduleindex++)
            {
                upgrade = Module:moduleindex;
                if(GetCredits(client)>=GetUpgradeConfigCost(upgrade)+(GetPlayerUpgradeLevel(client,upgrade)*GetUpgradeConfigiCost(upgrade))&&!IsUpgradeMaxed(client,upgrade))
                    PurchaseUpgrade(client,upgrade);
            }
            return 1;
        }
        else
            return 0;
    }
    else
        return -1;
}

SetPlayerUpgradeLevel(client,Module:upgrade,level)
{
    if(IsValidPlayer(client))
    {
        new maxed=GetUpgradeConfigMaxLevel(upgrade);
        if(level>maxed)
            level=maxed;
        new vecPosi=GetPlayerVectorPosition(client);
        if(vecPosi!=-1)
        {
            SetArrayCell(GetArrayCell(g_hVecPlayers,vecPosi),_:upgrade*2+UPGRADE_OFFSET_LEVEL,level);
            return 1;
        }
        else
            return -1;
    }
    else
        return -1;
}

GetPlayerUpgradeLevel(client,Module:upgrade)
{
    if(IsValidPlayer(client))
    {
        new vecPosi=GetPlayerVectorPosition(client);
        if(vecPosi!=-1)
        {
            return GetArrayCell(GetArrayCell(g_hVecPlayers,vecPosi),_:upgrade*2+UPGRADE_OFFSET_LEVEL);
        }
        else
            return -1;
    }
    else
        return -1;
}

GetPlayerUpgradeCount(client)
{
    if(IsValidPlayer(client))
    {
        new count=0;
        new Module:upgrade;
        for(new moduleindex=1;moduleindex<MODULE_COUNT;moduleindex++)
        {
            upgrade = Module:moduleindex;
            if(GetPlayerUpgradeLevel(client,upgrade)>0)
                count++;
        }
        return count;
    }
    else
        return -1;
}

GetUpgradeConfigShortName(Module:upgrade,String:shortname[],maxlen)
{
    ModuleMgr_ReadString(upgrade, ModuleData_ShortName, shortname, maxlen);
}

GetUpgradeTranslatedFullName(client,Module:upgrade,String:name[],maxlen)
{
    decl String:upgradeshortname[MM_DATA_SHORTNAME],String:translatestring[32];
    
    GetUpgradeConfigShortName(upgrade,upgradeshortname,sizeof(upgradeshortname));
    Format(translatestring, sizeof(translatestring), "%s_fullname", upgradeshortname);
    Format(name, maxlen, "%T", translatestring, client);
}

GetUpgradeTranslatedDescription(client,Module:upgrade,String:description[],maxlen)
{
    decl String:upgradeshortname[MM_DATA_SHORTNAME],String:translatestring[32];
    
    GetUpgradeConfigShortName(upgrade,upgradeshortname,sizeof(upgradeshortname));
    Format(translatestring, sizeof(translatestring), "%s_description", upgradeshortname);
    Format(description, maxlen, "%T", translatestring, client);
}

GetUpgradeConfigTeamLock(Module:upgrade)
{
    return ModuleMgr_ReadCell(upgrade, ModuleData_TeamLock);
}

GetUpgradeConfigMaxLevel(Module:upgrade)
{
    return ModuleMgr_ReadCell(upgrade, ModuleData_MaxLevel);
}

stock GetUpgradeConfigCost(Module:upgrade)
{
    return ModuleMgr_ReadCell(upgrade, ModuleData_Cost);
}

GetUpgradeConfigiCost(Module:upgrade)
{
    return ModuleMgr_ReadCell(upgrade, ModuleData_iCost);
}

bool:IsUpgradeMaxed(client,Module:upgrade)
{
    if(IsValidPlayer(client))
        return (GetPlayerUpgradeLevel(client,upgrade)>=GetUpgradeConfigMaxLevel(upgrade));
    else
        return true;
}
