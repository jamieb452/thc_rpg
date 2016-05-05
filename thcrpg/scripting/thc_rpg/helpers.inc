/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          helpers.inc
 *  Type:          Core
 *  Description:   contains helper functions for the whole project
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

#include <sdktools>

InitOffsets()
{
    g_iColorOffset=FindSendPropOffs("CAI_BaseNPC","m_clrRender");
    if(g_iColorOffset==-1)
        LogMgr_Print(g_moduleCore, LogType_Fatal_Plugin, "InitOffsets", "PLUGIN STOPPED - Reason: could not find the Color offset - PLUGIN STOPPED");

    g_iGetVelocityOffset_0=FindSendPropOffs("CBasePlayer","m_vecVelocity[0]");
    if(g_iGetVelocityOffset_0==-1)
        LogMgr_Print(g_moduleCore, LogType_Fatal_Plugin, "InitOffsets", "PLUGIN STOPPED - Reason: could not find the GetVelocity_0 offset - PLUGIN STOPPED");

    g_iGetVelocityOffset_1=FindSendPropOffs("CBasePlayer","m_vecVelocity[1]");
    if(g_iGetVelocityOffset_1==-1)
        LogMgr_Print(g_moduleCore, LogType_Fatal_Plugin, "InitOffsets", "PLUGIN STOPPED - Reason: could not find the GetVelocity_1 offset - PLUGIN STOPPED");

    g_iBaseVelocityOffset=FindSendPropOffs("CBasePlayer","m_vecBaseVelocity");
    if(g_iBaseVelocityOffset==-1)
        LogMgr_Print(g_moduleCore, LogType_Fatal_Plugin, "InitOffsets", "PLUGIN STOPPED - Reason: could not find the BaseVelocity offset - PLUGIN STOPPED");

    g_iActiveWeaponOffset=FindSendPropOffs("CAI_BaseNPC", "m_hActiveWeapon");
    if(g_iActiveWeaponOffset==-1)
        LogMgr_Print(g_moduleCore, LogType_Fatal_Plugin, "InitOffsets", "PLUGIN STOPPED - Reason: could not find the ActiveWeapon offset - PLUGIN STOPPED");

    g_iWeaponClipOffset=FindSendPropOffs("CBaseCombatWeapon", "m_iClip1");
    if(g_iWeaponClipOffset==-1)
        LogMgr_Print(g_moduleCore, LogType_Fatal_Plugin, "InitOffsets", "PLUGIN STOPPED - Reason: could not find the WeaponClip offset - PLUGIN STOPPED");

    g_iArmorOffset=FindSendPropOffs("CCSPlayer","m_ArmorValue");
    if(g_iArmorOffset==-1)
        LogMgr_Print(g_moduleCore, LogType_Fatal_Plugin, "InitOffsets", "PLUGIN STOPPED - Reason: could not find the Armor offset - PLUGIN STOPPED");

    g_iAmmoOffset=FindSendPropOffs("CCSPlayer","m_iAmmo");
    if(g_iAmmoOffset==-1)
        LogMgr_Print(g_moduleCore, LogType_Fatal_Plugin, "InitOffsets", "PLUGIN STOPPED - Reason: could not find the Ammo offset - PLUGIN STOPPED");
}

FindMatchingPlayers(const String:matchstr[],clients[])
{
    new count=0;
    if(StrEqual(matchstr,"@all",false))
    {
        for(new x=1;x<=MaxClients;x++)
        {
            if(IsClientInGame(x))
            {
                clients[count]=x;
                count++;
            }
        }
    }
    else if(StrEqual(matchstr,"@t",false))
    {
        for(new x=1;x<=MaxClients;x++)
        {
            if(IsClientInGame(x)&&GetClientTeam(x)==CSS_TEAM_T)
            {
                clients[count]=x;
                count++;
            }
        }
    }
    else if(StrEqual(matchstr,"@ct",false))
    {
        for(new x=1;x<=MaxClients;x++)
        {
            if(IsClientInGame(x)&&GetClientTeam(x)==CSS_TEAM_CT)
            {
                clients[count]=x;
                count++;
            }
        }
    }
    else if(matchstr[0]=='@')
    {
        new userid=StringToInt(matchstr[1]);
        if(userid)
        {
            new index=GetClientOfUserId(userid);
            if(index)
            {
                if(IsClientInGame(index))
                {
                    clients[count]=index;
                    count++;
                }
            }
        }
    }
    else
    {
        decl String:name[64];
        for(new x=1;x<=MaxClients;x++)
        {
            if(IsClientInGame(x))
            {
                GetClientName(x,name,sizeof(name));
                if(StrContains(name,matchstr,false)!=-1)
                {
                    clients[count]=x;
                    count++;
                }
            }
        }
    }
    return count;
}

Float:TeamRatio(team)
{
    new Float:ttotal = 0.0;
    new Float:cttotal = 0.0;
    new Float:ratio;

    for(new x=1;x<=MaxClients;x++)
    {
        if(IsClientInGame(x))
            if(IsPlayerAlive(x))
                if(GetClientTeam(x)==CSS_TEAM_T)
                    ttotal++;
                else if(GetClientTeam(x)==CSS_TEAM_CT)
                    cttotal++;
    }

    if (ttotal<=0.0)
        ttotal = 1.0;
    if (cttotal<=0.0)
        cttotal = 1.0;

    if(team==CSS_TEAM_T)
        ratio = cttotal/ttotal;
    else
        ratio = ttotal/cttotal;

    if(ratio<=0.0)
        ratio = 1.0;

    return ratio;
}

Float:PlayerLevelRatio(victim,attacker)
{
    new victimlevel = GetLevel(victim);
    new attackerlevel = GetLevel(attacker);
    new Float:ratio;
    
    if(attackerlevel>1)
        ratio = float(victimlevel)/float(attackerlevel);
    else
        ratio = float(victimlevel);

    if(ratio<=0.0)
        ratio = 1.0;

    return ratio;
}

CountHumans()
{
    new total=0;
    
    for(new x=1;x<=MaxClients;x++)
        if(IsClientInGame(x)&&!IsFakeClient(x))
            if(GetClientTeam(x)==CSS_TEAM_T||GetClientTeam(x)==CSS_TEAM_CT)
                total++;
        
    return total;
}

UniqueID(client, String:authid[], maxlen)
{
    if(IsValidPlayer(client))
    {
        new exists=GetClientAuthString(client,authid,maxlen);
        if(exists)
        {
            if(StrEqual(authid,"BOT",true))
            {
                decl String:name[64];
                GetClientName(client,name,sizeof(name));
                Format(authid,maxlen,"BOT_%s",name);
            }
            if(StrEqual(authid,"STEAM_ID_LAN",true))
                GetClientName(client,authid,maxlen);
            return 1;
        }
        else
            return -1;
    }
    else
        return -1;
}

SetSpawnHealth(client,amount)
{
    if(IsValidPlayer(client))
    {
        if(amount<=0||amount>g_CoreConfigCache[CoreConfig_maxspawnhp])
            amount=g_CoreConfigCache[CoreConfig_maxspawnhp];
        g_iClientSpawnHealth[client]=amount;
        return 1;
    }
    else
        return -1;
}

GetSpawnHealth(client)
{
    if(IsValidPlayer(client))
    {
        new returnhealth=g_iClientSpawnHealth[client];
        if(returnhealth<=0)
            returnhealth=g_CoreConfigCache[CoreConfig_maxspawnhp];
        return returnhealth;
    }
    else
        return 100;
}

SetSpawnSpeed(client,Float:amount)
{
    if(IsValidPlayer(client))
    {
        if(amount<=0.0)
            amount=1.0;
        g_fClientSpawnSpeed[client]=amount;
        return 1;
    }
    else
        return -1;
}

Float:GetSpawnSpeed(client)
{
    if(IsValidPlayer(client))
    {
        new Float:returnspeed=g_fClientSpawnSpeed[client];
        if(returnspeed<=0.0)
            returnspeed=1.0;
        return returnspeed;
    }
    else
        return 1.0;
}

SetModifiedHealth(client,amount)
{
    if(IsValidPlayer(client))
    {
        if(amount<=0)
            amount=100;
        g_iClientModifiedHealth[client]=amount;
        return 1;
    }
    else
        return -1;
}

GetModifiedHealth(client)
{
    if(IsValidPlayer(client))
    {
        new returnhealth=g_iClientModifiedHealth[client];
        if(returnhealth<=0)
            returnhealth=100;
        return returnhealth;
    }
    else
        return 100;
}

SetModifiedSpeed(client,Float:amount)
{
    if(IsValidPlayer(client))
    {
        if(amount<=0.0)
            amount=1.0;
        g_fClientModifiedSpeed[client]=amount;
        return 1;
    }
    else
        return -1;
}

Float:GetModifiedSpeed(client)
{
    if(IsValidPlayer(client))
    {
        new Float:returnspeed=g_fClientModifiedSpeed[client];
        if(returnspeed<=0.0)
            returnspeed=1.0;
        return returnspeed;
    }
    else
        return 1.0;
}

Float:CreateVecFromPoints(Float:vec1[3],Float:vec2[3],Float:output[3])
{
    output[0]=vec2[0]-vec1[0];
    output[1]=vec2[1]-vec1[1];
    output[2]=vec2[2]-vec1[2];
}

Normalize(Float:vec[3],Float:output[3])
{
    new Float:scalar=SquareRoot(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
    output[0]=vec[0]/scalar;
    output[1]=vec[1]/scalar;
    output[2]=vec[2]/scalar;
}

Float:GetDistanceBetween(Float:startvec[3],Float:endvec[3])
{
    return SquareRoot((startvec[0]-endvec[0])*(startvec[0]-endvec[0])+(startvec[1]-endvec[1])*(startvec[1]-endvec[1])+(startvec[2]-endvec[2])*(startvec[2]-endvec[2]));
}

public Action:Timer_KillTimer(Handle:timer,Handle:hTimer)
{
    if(hTimer!=INVALID_HANDLE)
        KillTimer(hTimer, true);
        
    return Plugin_Stop;
}

SetVelocityVector(client,const Float:velocity[3])
{
    if(IsValidPlayer(client))
    {
        SetEntDataVector(client,g_iBaseVelocityOffset,velocity,true);
        return 1;
    }
    else
        return -1;
}

GetHorizontalVelocity(client,Float:output[3])
{
    if(IsValidPlayer(client))
    {
        output[0]=GetEntDataFloat(client,g_iGetVelocityOffset_0);
        output[1]=GetEntDataFloat(client,g_iGetVelocityOffset_1);
        output[2]=0.0;
        return 1;
    }
    else
    {
        output[0]=0.0;
        output[1]=0.0;
        output[2]=0.0;
        return -1;
    }
}

Freeze(client,bool:freeze,totalfreeze,effects,Float:amount=0.1)
{
    if(IsValidPlayer(client))
        if(IsClientInGame(client))
        {
            new Float:velocity=GetSpawnSpeed(client);
            if(freeze)
            {
                if(totalfreeze==0)
                    SetEntPropFloat(client, Prop_Data, "m_flLaggedMovementValue", velocity*amount);
                else
                    SetEntPropFloat(client, Prop_Data, "m_flLaggedMovementValue", 0.0);                
                if(effects)
                    SetColor(client,0,0,255,-1); // Set the player's color
            }
            else
            {
                SetEntPropFloat(client, Prop_Data, "m_flLaggedMovementValue", velocity);
                SetColor(client,255,255,255,-1); // Set the player's color
            }
        }
}

stock ExtinguishClient(client)
{
	new ent = GetEntPropEnt(client, Prop_Data, "m_hEffectEntity");
	if (IsValidEdict(ent))
	{
		decl String:classname[64];
		GetEdictClassname(ent, classname, sizeof(classname));
		if (StrEqual(classname, "entityflame", false))
			SetEntPropFloat(ent, Prop_Data, "m_flLifetime", 0.0);
	}
}

SetColor(entity,red,green,blue,alpha)
{
    if(red!=-1)
        SetEntData(entity,g_iColorOffset,red,1,true);
    if(green!=-1)
        SetEntData(entity,g_iColorOffset+1,green,1,true);
    if(blue!=-1)
        SetEntData(entity,g_iColorOffset+2,blue,1,true);
    if(alpha!=-1)
        SetEntData(entity,g_iColorOffset+3,alpha,1,true);

    SetEntityRenderMode(entity, RENDER_TRANSTEXTURE);
}

PlayLevelupSound(client)
{
    if(IsValidPlayer(client))
    {
        ClientCommand(client,"play buttons/blip2.wav");
        return 1;
    }
    else
        return -1;
}

Top10Menu(client,playercount,String:Top10[][][])
{
    if(IsValidPlayer(client))
    {
        decl String:item[128];

        new Handle:menu_main=CreateMenu(Top10MenuHandle);

        SetMenuPagination(menu_main, MENU_NO_PAGINATION);

        SetMenuTitle(menu_main, "%T", "top10_title", client);

        for(new i=0;i<playercount;i++)
        {
            Format(item, sizeof(item), "%T", "top10_item", client, Top10[i][0], Top10[i][1]);
            AddMenuItem(menu_main,item,item,ITEMDRAW_DISABLED);
        }

        DisplayMenu(menu_main,client,5);
        return 1;
    }
    else
        return -1;
}

public Top10MenuHandle(Handle:menu_main,MenuAction:action,client,slot)
{
    if(action==MenuAction_Select)
    {
    }
    if(action==MenuAction_End)
        CloseHandle(menu_main);
}

ResetPlayer(client)
{
    if(IsValidPlayer(client))
    {
        // remove player from db
        RemovePlayer(client);
        // remove player from array
        new vecposi=GetPlayerVectorPosition(client);
        RemoveFromArray(g_hVecPlayers, vecposi);
        // reset global upgrade variables
        ResetUpgradeVariables(client);
        SetHurtXP(client,0);
        if(!StrEqual(g_CoreConfigCache[CoreConfig_sound_reset], "off", false))
            ClientCommand(client,"play %s", g_CoreConfigCache[CoreConfig_sound_reset]);
        // insert player in array
        InitPlayer(client);
        // insert player in db
        LoadPlayerData(client);
        return 1;
    }
    else
        return -1;
}

ResetUpgradeVariables(client)
{
    SetModifiedHealth(client, GetSpawnHealth(client));
    SetModifiedSpeed(client, GetSpawnSpeed(client));
}

/**
 * Returns a list of modules that interlock the given module.
 *
 * @param module        The module to find interlocks for.
 * @param interlocks    A list of modules that interlock the given module.
 *
 * @return              The number of modules that interlock the given module.
 */
stock FindInterlockModules(Module:module, Module:interlocks[MM_DATA_INTERLOCKS])
{
    new dindex = 0;
    new Module:_interlocks[MM_DATA_INTERLOCKS];

    // Loop through all the modules.
    new count = MODULE_COUNT;
    for(new moduleindex = 0; moduleindex < count; moduleindex++)
    {
        ModuleMgr_ReadArray(Module:moduleindex, ModuleData_Interlocks, _interlocks, sizeof(_interlocks));
        for(new _dindex = 0; _dindex < sizeof(_interlocks); _dindex++)
        {
            if(_interlocks[_dindex] == INVALID_MODULE)
                break;

            if(_interlocks[_dindex] == module)
            {
                interlocks[dindex++] = Module:moduleindex;
                break;
            }
        }
    }

    return dindex;
}

bool:IsUpgradeInterlocked(client, Module:module)
{
    if(IsValidPlayer(client))
    {
        new Module:interlocks[MM_DATA_INTERLOCKS];
        new count = FindInterlockModules(module, interlocks);

        for(new dindex = 0; dindex < count; dindex++)
            // check for interlock on module
            if(GetPlayerUpgradeLevel(client,interlocks[dindex])>0)
                return true;
    }

    return false;
}

bool:AreValidPlayers(victim, attacker)
{
	return (IsValidPlayer(victim) && IsValidPlayer(attacker) && victim!=attacker);
}

bool:AreValidTeams(victim, attacker, freeforall, teamlock)
{
    return ( (GetClientTeam(victim)!=GetClientTeam(attacker) || freeforall) && GetClientTeam(attacker)!=teamlock );
}

bool:GetChance(level, maxlevel)
{
    new random=GetRandomInt(0, maxlevel+1);
    if(random==level||random==level*2||random==maxlevel)
        return true;

    return false;
}
