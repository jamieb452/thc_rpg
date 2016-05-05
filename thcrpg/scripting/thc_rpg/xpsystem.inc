/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          xpsystem.inc
 *  Type:          Core
 *  Description:   contains functions specific to the XP system (xp, level, credits)
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

new g_iHurtXP[MAXPLAYERS+1];

// generate a log entry for XP gain/losse
stock LogXPEvent(client, newxp)
{
    decl String:temp[16];
    decl String:properties[16];
    
    IntToString(newxp,temp,sizeof(temp));
    
    properties = " ";
    StrCat(properties,sizeof(properties),temp);
    
    LogPlayerEvent(client, "triggered", "XP", false, properties);
}

SetHurtXP(client,amount)
{
    if(IsValidPlayer(client))
    {
        if(amount>0)
            g_iHurtXP[client] += amount;
        else
            g_iHurtXP[client] = 0;

        LogMgr_Print(g_moduleCore, LogType_Debug, "SetHurtXP", "HurtXP of client %d is %d XP", client, g_iHurtXP[client]);

        return 1;
    }
    
    return -1;
}

GetHurtXP(client)
{
    if(IsValidPlayer(client))
    {
        return g_iHurtXP[client];
    }
    
    return 0;
}

SetXP(client,amount,display)
{
    if(IsValidPlayer(client)&&amount!=0)
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
            new reqxp=GetReqXP(client);
            LogMgr_Print(g_moduleCore, LogType_Debug, "SetXP", "ReqXP of client %d is %d XP", client, reqxp);
            new xp=GetXP(client);
            new newxp=xp+amount;
            if(newxp>g_CoreConfigCache[CoreConfig_exp_max])
            {
                newxp=g_CoreConfigCache[CoreConfig_exp_max];
                amount=newxp-xp;
            }
            new newlevel=0;
            if(xp<g_CoreConfigCache[CoreConfig_exp_max])
            {
                while(newxp>reqxp)
                {
                    newlevel+=1;
                    newxp-=reqxp;
                }
                while(newxp<0)
                {
                    newlevel-=1;
                    newxp=reqxp-g_CoreConfigCache[CoreConfig_exp_inc]-newxp;
                }
                SetLevel(client,newlevel,display);
                if(newxp<0)
                   newxp=0;
                new vecPosi=GetPlayerVectorPosition(client);
                if(vecPosi!=-1)
                    SetArrayCell(GetArrayCell(g_hVecPlayers,vecPosi),VECTOR_PLAYERS_XP,newxp);
                else
                    return -1;
                LogMgr_Print(g_moduleCore, LogType_Debug, "SetXP", "Client %d is set to %d XP, alternation is %d", client, newxp, amount);
                //LogXPEvent(client, newxp);
                if(display)
                {
                    new newreqxp=GetReqXP(client);
                    if(amount>0)
                        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Give XP", amount, newxp, newreqxp);
                    else if(amount<0)
                        TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Take XP", amount, newxp, newreqxp);
                }
            }
            else
                LogMgr_Print(g_moduleCore, LogType_Debug, "SetXP", "Client %d reached max XP, max XP is %d", client, g_CoreConfigCache[CoreConfig_exp_max]);
            return 1;
        }
        else
            return 0;
    }
    else
        return -1;
}

SetStaticXP(client,amount)
{
    if(IsValidPlayer(client))
    {
        if(amount<0)
            amount=0;
        new vecPosi=GetPlayerVectorPosition(client);
        if(vecPosi!=-1)
        {
            SetArrayCell(GetArrayCell(g_hVecPlayers,vecPosi),VECTOR_PLAYERS_XP,amount);
            return 1;
        }
        else
            return -1;
    }
    else
        return -1;
}

GetXP(client)
{
    if(IsValidPlayer(client))
    {
        new vecPosi=GetPlayerVectorPosition(client);
        if(vecPosi!=-1)
        {
            return GetArrayCell(GetArrayCell(g_hVecPlayers,vecPosi),VECTOR_PLAYERS_XP);
        }
        else
            return -1;
    }
    else
        return -1;
}

SetReqXP(client,amount)
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
            new newreqxp=GetReqXP(client)+amount;
            if(newreqxp<g_CoreConfigCache[CoreConfig_exp_start])
                newreqxp=g_CoreConfigCache[CoreConfig_exp_start];
            if(newreqxp>g_CoreConfigCache[CoreConfig_exp_max])
                newreqxp=g_CoreConfigCache[CoreConfig_exp_max];
            new vecPosi=GetPlayerVectorPosition(client);
            if(vecPosi!=-1)
            {
                SetArrayCell(GetArrayCell(g_hVecPlayers,vecPosi),VECTOR_PLAYERS_REQXP,newreqxp);
                LogMgr_Print(g_moduleCore, LogType_Debug, "SetReqXP", "Client %d is set to %d ReqXP, alternation is %d", client, newreqxp, amount);
                return 1;
            }
            else
                return -1;
        }
        else
            return 0;
    }
    else
        return -1;
}

SetStaticReqXP(client,amount)
{
    if(IsValidPlayer(client))
    {
        if(amount<g_CoreConfigCache[CoreConfig_exp_start])
            amount=g_CoreConfigCache[CoreConfig_exp_start];
        if(amount>g_CoreConfigCache[CoreConfig_exp_max])
            amount=g_CoreConfigCache[CoreConfig_exp_max];
        new vecPosi=GetPlayerVectorPosition(client);
        if(vecPosi!=-1)
        {
            SetArrayCell(GetArrayCell(g_hVecPlayers,vecPosi),VECTOR_PLAYERS_REQXP,amount);
            return 1;
        }
        else
            return -1;
    }
    else
        return -1;
}

GetReqXP(client)
{
    if(IsValidPlayer(client))
    {
        new vecPosi=GetPlayerVectorPosition(client);
        if(vecPosi!=-1)
            return GetArrayCell(GetArrayCell(g_hVecPlayers,vecPosi),VECTOR_PLAYERS_REQXP);
        else
            return -1;
    }
    else
        return -1;
}

SetLevel(client,amount,display)
{
    if(IsValidPlayer(client)&&amount!=0)
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
            new newlevel=GetLevel(client)+amount;
            if(newlevel<1)
                newlevel=1;
            new vecPosi=GetPlayerVectorPosition(client);
            if(vecPosi!=-1)
                SetArrayCell(GetArrayCell(g_hVecPlayers,vecPosi),VECTOR_PLAYERS_LEVEL,newlevel);
            else
                return -1;
            LogMgr_Print(g_moduleCore, LogType_Debug, "SetLevel", "Client %d is set to level %d, alternation is %d", client, newlevel, amount);
            if(amount!=0)
            {
                SetCredits(client,g_CoreConfigCache[CoreConfig_credits_inc]*amount,display);
                SetReqXP(client,g_CoreConfigCache[CoreConfig_exp_inc]*amount);
                //SavePlayerData(client); // Save player's data
                decl String:name[64];
                GetClientName(client,name,sizeof(name));
                if(display)
                {
                    for(new x=1;x<=MaxClients;x++)
                        if(IsClientInGame(x)&&x!=client)
                            TransMgr_PrintText(x, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Announce levelup", name, newlevel);
                    TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Tell player levelup", newlevel);
                    PlayLevelupSound(client);
                }
                // Check if bot went over maxlevel
                if(isbot&&amount>0&&amount>=g_CoreConfigCache[CoreConfig_bot_maxlevel])
                {
                    ResetPlayer(client);
                    for(new x=1;x<=MaxClients;x++)
                        if(IsClientInGame(x))
                        {
                            LogMgr_Print(g_moduleCore, LogType_Debug, "SetLevel", "Bot %s reached maxlevel, stats got reset", name);
                            TransMgr_PrintText(x, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Bot reset stats", name, g_CoreConfigCache[CoreConfig_bot_maxlevel]);
                        }
                }
            }
            if(isbot)
                BotBuyRandomUpgrade(client);
            return 1;
        }
        else
            return 0;
    }
    else
        return -1;
}

SetStaticLevel(client,amount)
{
    if(IsValidPlayer(client))
    {
        if(amount<0)
            amount=0;
        new vecPosi=GetPlayerVectorPosition(client);
        if(vecPosi!=-1)
        {
            SetArrayCell(GetArrayCell(g_hVecPlayers,vecPosi),VECTOR_PLAYERS_LEVEL,amount);
            return 1;
        }
        else
            return -1;
    }
    else
        return -1;
}

GetLevel(client)
{
    if(IsValidPlayer(client))
    {
        new vecPosi=GetPlayerVectorPosition(client);
        if(vecPosi!=-1)
            return GetArrayCell(GetArrayCell(g_hVecPlayers,vecPosi),VECTOR_PLAYERS_LEVEL);
        else
            return -1;
    }
    else
        return -1;
}

SetCredits(client,amount,display)
{
    if(IsValidPlayer(client)&&amount!=0)
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
            new newcredits=GetCredits(client)+amount;
            if(newcredits<0)
                newcredits=0;
            new vecPosi=GetPlayerVectorPosition(client);
            if(vecPosi!=-1)
            {
                SetArrayCell(GetArrayCell(g_hVecPlayers,vecPosi),VECTOR_PLAYERS_CREDITS,newcredits);
                LogMgr_Print(g_moduleCore, LogType_Debug, "SetCredits", "Credits of client %d are set to %d, alternation is %d", client, newcredits, amount);
                if(display)
                    TransMgr_PrintText(client, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Notify credits", newcredits);
                return 1;
            }
            else
                return -1;
        }
        else
            return 0;
    }
    else
        return -1;
}

SetStaticCredits(client,amount)
{
    if(IsValidPlayer(client))
    {
        if(amount<0)
            amount=0;
        new vecPosi=GetPlayerVectorPosition(client);
        if(vecPosi!=-1)
        {
            SetArrayCell(GetArrayCell(g_hVecPlayers,vecPosi),VECTOR_PLAYERS_CREDITS,amount);
            return 1;
        }
        else
            return -1;
    }
    else
        return -1;
}

GetCredits(client)
{
    if(IsValidPlayer(client))
    {
        new vecPosi=GetPlayerVectorPosition(client);
        if(vecPosi!=-1)
            return GetArrayCell(GetArrayCell(g_hVecPlayers,vecPosi),VECTOR_PLAYERS_CREDITS);
        else
            return -1;
    }
    else
        return -1;
}

TeamWinXP(team)
{
    if(g_CoreConfigCache[CoreConfig_freeforall]==0)
    {
        for(new x=1;x<=MaxClients;x++)
            if(IsValidPlayer(x))
                if(IsPlayerAlive(x)&&GetClientTeam(x)==team)
                    if(g_CoreConfigCache[CoreConfig_exp_mode]==0)
                        SetXP(x,RoundToNearest(TeamRatio(team)*GetClientFrags(x)*g_CoreConfigCache[CoreConfig_exp_teamwin]),true);
                    else
                        SetXP(x,RoundToNearest(g_CoreConfigCache[CoreConfig_exp_teamwin]),true);
                    
        return 1;
    }
    else
        return -1;
}
