/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          sql.inc
 *  Type:          Core
 *  Description:   contains SQL specific functions
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

new reconnectcounter = 0;

ConnectSQL()
{
    if (g_hSQL != INVALID_HANDLE)
        CloseHandle(g_hSQL);
    g_hSQL = INVALID_HANDLE;

    if (SQL_CheckConfig("thc_rpg"))
        SQL_TConnect(ConnectSQLCallback, "thc_rpg");
    else
        LogMgr_Print(g_moduleCore, LogType_Fatal_Plugin, "ConnectSQL", "PLUGIN STOPPED - Reason: no config entry found for 'thc_rpg' in databases.cfg - PLUGIN STOPPED");
}

public ConnectSQLCallback(Handle:owner, Handle:hndl, const String:error[], any:data)
{
    if (reconnectcounter >= 5)
    {
        LogMgr_Print(g_moduleCore, LogType_Fatal_Plugin, "ConnectSQL", "PLUGIN STOPPED - Reason: reconnect counter reached max - PLUGIN STOPPED");
        return -1;
    }

    if (hndl == INVALID_HANDLE)
    {
        LogMgr_Print(g_moduleCore, LogType_Error, "ConnectSQL", "Connection to SQL database has failed, Reason: %s", error);
        reconnectcounter++;
        ConnectSQL();
        return -1;
    }

    LogMgr_Print(g_moduleCore, LogType_Debug, "ConnectSQL", "Successfully connected to database");

    decl String:driver[16];
    SQL_GetDriverIdent(owner,driver,sizeof(driver));
    if (StrEqual(driver,"mysql",false))
        SQL_FastQuery(hndl,"SET NAMES  'utf8'");

    // save database handle
    g_hSQL = CloneHandle(hndl);

    if (reconnectcounter == 0)
        ManageSQLTable(1);		// create SQL table if it is not existing

    reconnectcounter = 1;
    return 1;
}

ManageSQLTable(mode)
{
    if (mode == 1)
        SQL_TQuery(g_hSQL,CreateSQLTableCallback,"CREATE TABLE IF NOT EXISTS thc_rpg (ID VARCHAR(64),name VARCHAR(128),xp INTEGER(8),reqxp INTEGER(8),level INTEGER(8),credits INTEGER(8),dbver VARCHAR(16))", DBPrio_High);
    else if (mode == 2)
        SQL_TQuery(g_hSQL,DropSQLTableCallback,"DROP TABLE thc_rpg", DBPrio_High);
}

public CreateSQLTableCallback(Handle:owner, Handle:hndl, const String:error[], any:data)
{
    if (owner == INVALID_HANDLE)
    {
        LogMgr_Print(g_moduleCore, LogType_Error, "CreateSQLTable", "Connection to SQL database has failed, Reason: %s", error);
        reconnectcounter++;
        ConnectSQL();
        return -1;
    }

    if (hndl == INVALID_HANDLE)
        LogMgr_Print(g_moduleCore, LogType_Fatal_Plugin, "CreateSQLTable", "Could not create the necessary table in database, Reason: %s", error);
    else
    {
        LogMgr_Print(g_moduleCore, LogType_Debug, "CreateSQLTable", "Successfully created necessary table in database");

        for(new moduleindex=1;moduleindex<MODULE_COUNT;moduleindex++)
        {
            decl String:shortname[16], String:sLevel[16];//, String:sCost[16];
            // Read moduleindex as a Module type.
            new Module:upgrade = Module:moduleindex;

            ModuleMgr_ReadString(upgrade, ModuleData_ShortName, shortname, sizeof(shortname));
            Format(sLevel,sizeof(sLevel),"%s_l",shortname);
            CreateColumn(owner,sLevel,"INTEGER(8)");
        }
    }

    return 1;
}

public DropSQLTableCallback(Handle:owner, Handle:hndl, const String:error[], any:data)
{
    if (owner == INVALID_HANDLE)
    {
        LogMgr_Print(g_moduleCore, LogType_Error, "DropSQLTable", "Connection to SQL database has failed, Reason: %s", error);
        reconnectcounter++;
        ConnectSQL();
        return -1;
    }

    if (hndl == INVALID_HANDLE)
        LogMgr_Print(g_moduleCore, LogType_Error, "DropSQLTable", "Could not drop tabel from database, Reason: %s", error);
    else
        LogMgr_Print(g_moduleCore, LogType_Debug, "DropSQLTable", "Successfully dropped table from database");

    return 1;
}

InsertPlayer(client)
{
    if(IsValidPlayer(client))
    {
        decl String:uniqueid[64];
        new vecposi=GetPlayerVectorPosition(client);
        if(vecposi!=-1)
            GetArrayString(GetArrayCell(g_hVecPlayers,vecposi), VECTOR_PLAYERS_ID, uniqueid, sizeof(uniqueid));
        else
            return -1;

        decl String:buffer[8192],String:clientname[128];
        GetClientName(client,clientname,sizeof(clientname));
        ReplaceString(clientname, sizeof(clientname), "'", "", false);

        decl String:shortname[16];
        Format(buffer,sizeof(buffer),"INSERT INTO thc_rpg (ID,name,xp,reqxp,level,credits,dbver");
        for(new moduleindex=1;moduleindex<MODULE_COUNT;moduleindex++)
        {
            // Read moduleindex as a Module type.
            new Module:upgrade = Module:moduleindex;

            ModuleMgr_ReadString(upgrade, ModuleData_ShortName, shortname, sizeof(shortname));
            Format(buffer,sizeof(buffer),"%s,%s_l",buffer,shortname,shortname);
        }
        Format(buffer,sizeof(buffer),"%s) VALUES ('%s','%s','0','%d','1','%d','%s'",buffer,uniqueid,clientname,g_CoreConfigCache[CoreConfig_exp_start],g_CoreConfigCache[CoreConfig_credits_start],PROJECT_VERSION);
        SetStaticXP(client,0);
        SetStaticReqXP(client,g_CoreConfigCache[CoreConfig_exp_start]);
        SetStaticLevel(client,1);
        SetStaticCredits(client,g_CoreConfigCache[CoreConfig_credits_start]);
        for(new moduleindex=1;moduleindex<MODULE_COUNT;moduleindex++)
        {
            // Read moduleindex as a Module type.
            new Module:upgrade = Module:moduleindex;

            ModuleMgr_ReadString(upgrade, ModuleData_ShortName, shortname, sizeof(shortname));

            Format(buffer,sizeof(buffer),"%s,'0'",buffer);
            SetPlayerUpgradeLevel(client,upgrade,0);
        }
        Format(buffer,sizeof(buffer),"%s)",buffer);
        if(strlen(buffer)>=sizeof(buffer))
            LogMgr_Print(g_moduleCore, LogType_Error, "InsertPlayer", "Player %s not added to database, Reason: string size limit exceeded", clientname);
        else
            SQL_TQuery(g_hSQL, InsertPlayerCallback, buffer, client, DBPrio_Normal);

        return 1;
    }
    else
        return -1;
}

public InsertPlayerCallback(Handle:owner, Handle:hndl, const String:error[], any:client)
{
    if (owner == INVALID_HANDLE)
    {
        LogMgr_Print(g_moduleCore, LogType_Error, "InsertPlayer", "Connection to SQL database has failed, Reason: %s", error);
        reconnectcounter++;
        ConnectSQL();
        return -1;
    }

    if(IsValidPlayer(client))
    {
        decl String:clientname[128];
        GetClientName(client,clientname,sizeof(clientname));
        ReplaceString(clientname, sizeof(clientname), "'", "", false);

        if (hndl == INVALID_HANDLE)
            LogMgr_Print(g_moduleCore, LogType_Error, "InsertPlayer", "Player %s not added to database, Reason: %s", clientname, error);
        else
            LogMgr_Print(g_moduleCore, LogType_Debug, "InsertPlayer", "Player %s added to database", clientname);

        return -1;
    }
    else
        return -1;
}

RemovePlayer(client)
{
    if(IsValidPlayer(client))
    {
        new vecPosi=GetPlayerVectorPosition(client);
        if(vecPosi!=-1)
        {
            decl String:uniqueid[64],String:buffer[128];

            GetArrayString(GetArrayCell(g_hVecPlayers,vecPosi), VECTOR_PLAYERS_ID, uniqueid, sizeof(uniqueid));
            Format(buffer,sizeof(buffer),"DELETE FROM thc_rpg WHERE ID='%s'",uniqueid);
            SQL_TQuery(g_hSQL, RemovePlayerCallback, buffer, client, DBPrio_High);
            return 1;
        }
        else
            return -1;
    }
    else
        return -1;
}

public RemovePlayerCallback(Handle:owner, Handle:hndl, const String:error[], any:client)
{
    if (owner == INVALID_HANDLE)
    {
        LogMgr_Print(g_moduleCore, LogType_Error, "RemovePlayer", "Connection to SQL database has failed, Reason: %s", error);
        reconnectcounter++;
        ConnectSQL();
        return -1;
    }

    if(IsValidPlayer(client))
    {
        decl String:clientname[128];
        GetClientName(client,clientname,sizeof(clientname));
        ReplaceString(clientname, sizeof(clientname), "'", "", false);

        if (hndl == INVALID_HANDLE)
            LogMgr_Print(g_moduleCore, LogType_Error, "RemovePlayer", "Player %s not removed from database, Reason: %s", clientname, error);
        else
            LogMgr_Print(g_moduleCore, LogType_Debug, "RemovePlayer", "Player %s removed from database", clientname);

        return 1;
    }
    else
        return -1;
}

LoadPlayerData(client)
{
    if(IsValidPlayer(client))
    {
        decl String:uniqueid[64];
        new vecPosi=GetPlayerVectorPosition(client);
        if(vecPosi!=-1)
            GetArrayString(GetArrayCell(g_hVecPlayers,vecPosi), VECTOR_PLAYERS_ID, uniqueid, sizeof(uniqueid));
        else
            return -1;
        decl String:buffer[128];
        Format(buffer,sizeof(buffer),"SELECT * FROM thc_rpg WHERE ID = '%s'",uniqueid);
        SQL_TQuery(g_hSQL, LoadPlayerDataCallback, buffer, client, DBPrio_Normal);
        return 1;
    }
    else
        return -1;
}

public LoadPlayerDataCallback(Handle:owner, Handle:hndl, const String:error[], any:client)
{
    if (owner == INVALID_HANDLE)
    {
        LogMgr_Print(g_moduleCore, LogType_Error, "LoadPlayerData", "Connection to SQL database has failed, Reason: %s", error);
        reconnectcounter++;
        ConnectSQL();
        return -1;
    }

    if(IsValidPlayer(client))
    {
        decl String:clientname[128];
        GetClientName(client,clientname,sizeof(clientname));
        ReplaceString(clientname, sizeof(clientname), "'", "", false);

        if (hndl == INVALID_HANDLE)
        {
            LogMgr_Print(g_moduleCore, LogType_Error, "LoadPlayerData", "Cant load data of Player %s from database, Reason: %s", clientname, error);
        }
        else
        {
            SQL_Rewind(hndl);
            new bool:fetch=SQL_FetchRow(hndl);
            if(!fetch)
            {
                LogMgr_Print(g_moduleCore, LogType_Debug, "LoadPlayerData", "Player %s does not exists in database, needs to be inserted", clientname);
                InsertPlayer(client);
                return 0;
            }
            else
            {
                SetStaticXP(client,GetSQLDataInt(hndl,"xp"));
                SetStaticReqXP(client,GetSQLDataInt(hndl,"reqxp"));
                SetStaticLevel(client,GetSQLDataInt(hndl,"level"));
                SetStaticCredits(client,GetSQLDataInt(hndl,"credits"));
                new Module:upgrade;
                decl String:shortname[16];
                for(new moduleindex=1;moduleindex<MODULE_COUNT;moduleindex++)
                {
                    // Read moduleindex as a Module type.
                    upgrade = Module:moduleindex;

                    ModuleMgr_ReadString(upgrade, ModuleData_ShortName, shortname, sizeof(shortname));

                    decl String:buffer[128];
                    Format(buffer,sizeof(buffer),"%s_l",shortname);
                    new level=GetSQLDataInt(hndl,buffer);
                    if(level<0)
                        level = 0;
                    SetPlayerUpgradeLevel(client,upgrade,level);
                }
                LogMgr_Print(g_moduleCore, LogType_Debug, "LoadPlayerData", "Player %s successfully loaded from database", clientname);
            }
        }

        return 1;
    }
    else
        return -1;
}

SavePlayerData(client)
{
    if(IsValidPlayer(client))
    {
        decl String:uniqueid[64];
        new vecPosi=GetPlayerVectorPosition(client);
        if(vecPosi!=-1)
            GetArrayString(GetArrayCell(g_hVecPlayers,vecPosi), VECTOR_PLAYERS_ID, uniqueid, sizeof(uniqueid));
        else
            return -1;

        decl String:clientname[128];
        GetClientName(client,clientname,sizeof(clientname));
        ReplaceString(clientname, sizeof(clientname), "'", "", false);

        decl String:buffer[8192];
        Format(buffer,sizeof(buffer),"name = '%s', xp = '%d', reqxp = '%d', level = '%d', credits = '%d'",clientname,GetXP(client),GetReqXP(client),GetLevel(client),GetCredits(client));
        for(new moduleindex=1;moduleindex<MODULE_COUNT;moduleindex++)
        {
            decl String:shortname[16];
            // Read moduleindex as a Module type.
            new Module:upgrade = Module:moduleindex;

            ModuleMgr_ReadString(upgrade, ModuleData_ShortName, shortname, sizeof(shortname));
            Format(buffer,sizeof(buffer),"%s, %s_l = '%d'",buffer,shortname,GetPlayerUpgradeLevel(client,upgrade));
        }
        new Handle:datapack = CreateDataPack();
        WritePackString(datapack, clientname);

        decl String:query[8192];
        Format(query,sizeof(query),"UPDATE thc_rpg SET %s WHERE ID = '%s'",buffer,uniqueid);
        SQL_TQuery(g_hSQL, SavePlayerDataCallback, query, datapack, DBPrio_Normal);
        return vecPosi;
    }
    else
        return -1;
}

public SavePlayerDataCallback(Handle:owner, Handle:hndl, const String:error[], any:datapack)
{
    if (owner == INVALID_HANDLE)
    {
        LogMgr_Print(g_moduleCore, LogType_Error, "SavePlayerData", "Connection to SQL database has failed, Reason: %s", error);
        reconnectcounter++;
        ConnectSQL();
        return -1;
    }

    decl String:clientname[128];
    ResetPack(datapack);
    ReadPackString(datapack, clientname, sizeof(clientname));
    CloseHandle(datapack);

    if (hndl == INVALID_HANDLE)
        LogMgr_Print(g_moduleCore, LogType_Error, "SavePlayerData", "Player %s not saved to database, Reason: %s", clientname, error);
    else
        LogMgr_Print(g_moduleCore, LogType_Debug, "SavePlayerData", "Player %s saved to database", clientname);

    return 1;
}

GetPlayerRank(client,String:player[])
{
    if(IsValidPlayer(client))
    {
        new mode=0;
        new neededclient=-1;
        decl String:query[128], String:playername[64], String:clientname[64];

        if(StrEqual(player,"",false))
        {
            GetClientName(client,clientname,sizeof(clientname));
            strcopy(playername, sizeof(playername), clientname);
            neededclient = client;
            mode = 0;
        }
        else
        {
            strcopy(playername, sizeof(playername), player);
            for(new x=1;x<=MaxClients;x++)
            {
                if(IsClientInGame(x))
                {
                    GetClientName(x,clientname,sizeof(clientname));
                    if(StrContains(clientname,playername,false)!=-1)
                        neededclient = x;
                }
            }
            mode = 1;
        }

        if(neededclient<=0)
            return -1;

        ReplaceString(playername, sizeof(playername), "'", "", false);

        new Handle:datapack = CreateDataPack();
        WritePackCell(datapack, mode);
        WritePackCell(datapack, neededclient);
        WritePackString(datapack, playername);

        Format(query,sizeof(query),"SELECT COUNT(*) FROM thc_rpg WHERE level > %d OR (level = %d AND xp > %d) OR (level = %d AND xp = %d AND credits > %d)",GetLevel(neededclient),GetLevel(neededclient),GetXP(neededclient),GetLevel(neededclient),GetXP(neededclient),GetCredits(neededclient));
        SQL_TQuery(g_hSQL, GetPlayerRankCallback, query, datapack, DBPrio_Low);

        return 1;
    }
    else
        return -1;
}

public GetPlayerRankCallback(Handle:owner, Handle:hndl, const String:error[], any:datapack)
{
    if (owner == INVALID_HANDLE)
    {
        LogMgr_Print(g_moduleCore, LogType_Error, "GetPlayerRank", "Connection to SQL database has failed, Reason: %s", error);
        reconnectcounter++;
        ConnectSQL();
        return -1;
    }

    new mode, neededclient;
    decl String:playername[64];
    ResetPack(datapack);
    mode = ReadPackCell(datapack);
    neededclient = ReadPackCell(datapack);
    ReadPackString(datapack, playername, sizeof(playername));
    CloseHandle(datapack);

    if (hndl == INVALID_HANDLE)
        LogMgr_Print(g_moduleCore, LogType_Error, "GetPlayerRank", "Rank of player %s is not calculateable, Reason: %s", playername, error);
    else
    {
        SQL_Rewind(hndl);
        new bool:fetch=SQL_FetchRow(hndl);
        if(!fetch)
        {
            LogMgr_Print(g_moduleCore, LogType_Error, "GetPlayerRank", "Rank of player %s is not calculateable, Reason: Not data fetched", playername);
            return 0;
        }
        else
        {
            new rank=SQL_FetchInt(hndl,0)+1;
            if(mode==0)
                TransMgr_PrintText(neededclient, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Your Player rank", rank,GetLevel(neededclient),GetXP(neededclient),GetCredits(neededclient));
            else
                TransMgr_PrintTextAll(true, false, MsgFormat_Plugin, MsgType_Chat, INVALID_MODULE, false, "Other Players rank", playername,rank,GetLevel(neededclient),GetXP(neededclient),GetCredits(neededclient));
            LogMgr_Print(g_moduleCore, LogType_Debug, "GetPlayerRank", "Player %s is ranked at %d", playername, rank);
        }
    }

    return 1;
}

GetPlayerTop10(client)
{
    if(IsValidPlayer(client))
    {
        decl String:query[128];
        Format(query,sizeof(query),"SELECT * FROM thc_rpg ORDER BY level DESC, xp DESC, credits DESC LIMIT 10");
        SQL_TQuery(g_hSQL, GetPlayerTop10Callback, query, client, DBPrio_Low);

        return 1;
    }
    else
        return -1;
}

public GetPlayerTop10Callback(Handle:owner, Handle:hndl, const String:error[], any:client)
{
    if (owner == INVALID_HANDLE)
    {
        LogMgr_Print(g_moduleCore, LogType_Error, "GetPlayerTop10", "Connection to SQL database has failed, Reason: %s", error);
        reconnectcounter++;
        ConnectSQL();
        return -1;
    }

    if (hndl == INVALID_HANDLE)
        LogMgr_Print(g_moduleCore, LogType_Error, "GetPlayerTop10", "Top10 of players is not calculateable, Reason: %s", error);
    else
    {
        SQL_Rewind(hndl);
        new playercount=SQL_GetRowCount(hndl);
        if(playercount==0)
        {
            LogMgr_Print(g_moduleCore, LogType_Error, "GetPlayerTop10", "Top10 of players is not calculateable, Reason: Not data fetched");
            return 0;
        }
        else
        {
            decl String:Top10[10][2][128];
            new column1,column2;
            SQL_FieldNameToNum(hndl,"name",column1);
            SQL_FieldNameToNum(hndl,"level",column2);
            for(new i=0;i<playercount;i++)
            {
                if(SQL_FetchRow(hndl))
                {
                    SQL_FetchString(hndl, column1, Top10[i][0], 128);
                    new level = SQL_FetchInt(hndl, column2);
                    IntToString(level, Top10[i][1], 128);
                }
            }
            Top10Menu(client,playercount,Top10);
            LogMgr_Print(g_moduleCore, LogType_Debug, "GetPlayerTop10", "Playercount is %d", playercount);
        }
    }

    return 1;
}

//
// following functions are unthreaded
//

GetSQLDataInt(Handle:query,const String:columnname[])
{
    new column;
    SQL_FieldNameToNum(query,columnname,column);
    return SQL_FetchInt(query,column);
}

CreateColumn(Handle:sql,const String:columnname[],const String:settings[])
{
    decl String:query[256];

    Format(query,sizeof(query),"SELECT %s FROM thc_rpg LIMIT 1",columnname);
    if(!SQL_FastQuery(g_hSQL,query))
    {
        Format(query,sizeof(query),"ALTER TABLE thc_rpg ADD COLUMN %s %s",columnname,settings);
        if(SQL_FastQuery(sql,query))
            LogMgr_Print(g_moduleCore, LogType_Debug, "CreateColumn", "Successfully created necessary column '%s' in database", columnname);
        else
        {
            decl String:errorbuffer[1024];
            new bool:error=SQL_GetError(g_hSQL, errorbuffer, sizeof(errorbuffer));
            if(error)
                LogMgr_Print(g_moduleCore, LogType_Fatal_Plugin, "CreateColumn", "Could not create column %s in database, Reason: %s", columnname, errorbuffer);
        }
    }
}

stock CheckDBUpdate()
{
    new bool:error;
    decl String:errorbuffer[1024],String:query[64];
    new Handle:hResult=SQL_Query(g_hSQL,"SELECT dbver FROM thc_rpg");
    if(hResult)
    {
        SQL_Rewind(hResult);
        new bool:fetch=SQL_FetchRow(hResult);
        if(!fetch)
        {
            CloseHandle(hResult);
            LogMgr_Print(g_moduleCore, LogType_Error, "CheckDBUpdate", "Database query error, no data fetched");
            return 0;
        }
        else
        {
            new column;
            decl String:result[16];
            SQL_FieldNameToNum(hResult,"dbver",column);
            SQL_FetchString(hResult,column,result,sizeof(result));
            if(!SQL_IsFieldNull(hResult, column))
                if(StrEqual(result, PROJECT_VERSION, false))
                {
                    LogMgr_Print(g_moduleCore, LogType_Debug, "CheckDBUpdate", "Database is up-to-date, no changes needed");
                    return 0;
                }
                else
                {
                    // replace points and explode string by _
                    // dbver entry look like this 0.8.3_rc_4 and get changed to 0_8_3_rc_4 for exploding
                    new String:parts[8][8];
                    ReplaceString(result, sizeof(result), ".", "_", false);
                    ExplodeString(result, "_", parts, sizeof(parts), sizeof(parts[]));
                    // make changes
                    // for v0.8.2 to v0.8.3 - db layout has changed (*_c column and dbver)
                    if(StringToInt(parts[0])==0&&StringToInt(parts[1])==8&&StringToInt(parts[2])==2)
                    {
                        decl String:driver[16];
                        SQL_GetDriverIdent(g_hSQL,driver,sizeof(driver));
                        if (StrEqual(driver,"mysql",false))
                        {
                            decl String:shortname[16];
    
                            new Module:upgrade;
                            for(new moduleindex=1;moduleindex<MODULE_COUNT;moduleindex++)
                            {
                                // Read moduleindex as a Module type.
                                upgrade = Module:moduleindex;

                                ModuleMgr_ReadString(upgrade, ModuleData_ShortName, shortname, sizeof(shortname));

                                Format(query,sizeof(query),"ALTER TABLE thc_rpg DROP COLUMN %s_c",shortname);

                                SQL_FastQuery(g_hSQL,query);
                            }
                        }
                        Format(query,sizeof(query),"ALTER TABLE thc_rpg MODIFY dbver VARCHAR(16)");
                        SQL_FastQuery(g_hSQL,query);
                    }
                    // for v0.8.3 to v0.8.4 - nothing have changed
                    if(StringToInt(parts[0])==0&&StringToInt(parts[1])==8&&StringToInt(parts[2])==3)
                    {
                        Format(query,sizeof(query),"nothing :-P");
                    }
                    // end
                    // update version of database
                    Format(query,sizeof(query),"UPDATE thc_rpg SET dbver='%s'", PROJECT_VERSION);
                    SQL_FastQuery(g_hSQL,query);
                    LogMgr_Print(g_moduleCore, LogType_Debug, "CheckDBUpdate", "Database is updated to version '%s'", PROJECT_VERSION);
                    return 1;
                }
            else
                LogMgr_Print(g_moduleCore, LogType_Error, "CheckDBUpdate", "Database query error, field is NULL");
        }
    }
    else
    {
        error=SQL_GetError(g_hSQL, errorbuffer, sizeof(errorbuffer));
        if(error)
            LogMgr_Print(g_moduleCore, LogType_Error, "CheckDBUpdate", "Database query error, Reason: %s", errorbuffer);
        // column does not exist, create it!
        CreateColumn(g_hSQL,"dbver","VARCHAR(16)");
        // set dbver to actual project version
        Format(query,sizeof(query),"UPDATE thc_rpg SET dbver='%s'",PROJECT_VERSION);
        SQL_Query(g_hSQL,query);
        // check for errors
        error=SQL_GetError(g_hSQL, errorbuffer, sizeof(errorbuffer));
        if(error)
        {
            LogMgr_Print(g_moduleCore, LogType_Error, "CheckDBUpdate", "Database query error, Reason: %s", errorbuffer);
            return -1;
        }
        else
        {
            LogMgr_Print(g_moduleCore, LogType_Debug, "CheckDBUpdate", "Successfully created necessary column 'dbver' in database");
            return 1;
        }
    }

    return 1;
}
