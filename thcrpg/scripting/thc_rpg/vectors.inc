/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          vectors.inc
 *  Type:          Core
 *  Description:   contains all the vector functions for player handling
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

InitPlayersVector()
{
    g_hVecPlayers=CreateArray(1,0);
    if(g_hVecPlayers==INVALID_HANDLE)
        LogMgr_Print(g_moduleCore, LogType_Fatal_Plugin, "InitPlayersVector", "PLUGIN STOPPED - Reason: could not create vector to hold player data - PLUGIN STOPPED");
}

InitPlayer(client)
{
    if(IsValidPlayer(client))
    {
        if (GetPlayerVectorPosition(client)==-1)
        {
            decl String:uniqueid[64];
            UniqueID(client, uniqueid, sizeof(uniqueid));

            new Handle:vecPlayer=CreateArray(ByteCountToCells(sizeof(uniqueid)),0);
            PushArrayCell(vecPlayer,client); 		// Insert player's index
            PushArrayString(vecPlayer,uniqueid); 	// Insert player's id
            PushArrayCell(vecPlayer,0); 			// Insert player's xp
            PushArrayCell(vecPlayer,0); 			// Insert player's reqxp
            PushArrayCell(vecPlayer,0); 			// Insert player's level
            PushArrayCell(vecPlayer,0); 			// Insert player's credits

            for(new x=1;x<MODULE_COUNT;x++)
            {
                PushArrayCell(vecPlayer,0); 		// Insert space for level of an upgrade
                PushArrayCell(vecPlayer,0); 		// Insert space for cost of an upgrade
            }
            PushArrayCell(g_hVecPlayers,vecPlayer); 	// Insert player's handle into main vector

            LogMgr_Print(g_moduleCore, LogType_Debug, "InitPlayer", "Inserted client %d in player data vector", client);
            return 1;
        }
        else
        {
            LogMgr_Print(g_moduleCore, LogType_Debug, "InitPlayer", "Client %d already exists in player data vector", client);
            return 0;
        }
    }
    else
        return -1;
}

ClearPlayersVector()
{
    new size=GetArraySize(g_hVecPlayers);
    for(new x=0;x<size;x++)
        RemoveFromArray(g_hVecPlayers, 0);
    ClearArray(g_hVecPlayers);
}

GetPlayerVectorPosition(client)
{
    if(IsValidPlayer(client))
    {
        new index;
        new size=GetArraySize(g_hVecPlayers);
        for(new x=0;x<size;x++)
        {
            index=GetArrayCell(GetArrayCell(g_hVecPlayers,x),VECTOR_PLAYERS_CLIENT);
            if(client==index)
                return x;
        }
        return -1;
    }
    else
        return -1;
}
