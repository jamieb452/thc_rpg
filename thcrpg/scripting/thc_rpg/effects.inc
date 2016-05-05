/*
 * ============================================================================
 *
 *  [THC RPG] Total HardCore RPG
 *
 *  File:          effects.inc
 *  Type:          Core
 *  Description:   contains all effect natives
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

new precache_laser;

PrecacheModels()
{
    precache_laser=PrecacheModel("materials/sprites/laserbeam.vmt");
}

/*FadeEffect(client,type,time,duration,const color[4])
{
    if(IsValidPlayer(client))
    {
        new Handle:hBf=StartMessageOne("Fade",client);
        if(hBf!=INVALID_HANDLE)
        {
        decl color[4];
        color[0] = red;
        color[1] = green;
        color[2] = blue;
        color[3] = alpha;
        PbSetInt(hFade, "duration", speed);
        PbSetInt(hFade, "hold_time", hold);
        PbSetInt(hFade, "flags", type);
        PbSetColor(hFade, "clr", color);
            EndMessage();
            return 1;
        }
        else
            return -1;
    }
    else
        return -1;
}*/

BeamEffect(const String:matchstr[],Float:startvec[3],Float:endvec[3],Float:life,Float:width,Float:endwidth,const color[4],Float:amplitude,speed)
{
    new clients[MaxClients];
    new count=FindMatchingPlayers(matchstr,clients);
    if(count)
    {
        TE_SetupBeamPoints(startvec,endvec,precache_laser,0,0,66,life,width,endwidth,0,amplitude,color,speed);
        TE_Send(clients,count);
        return 1;
    }
    else
        return -1;
}

BeamRingEffect(const String:matchstr[],Float:centervec[3],Float:startradius,Float:endradius,Float:life,Float:width,const color[4],Float:amplitude,speed)
{
    new clients[MaxClients];
    new count=FindMatchingPlayers(matchstr,clients);
    if(count)
    {
        TE_SetupBeamRingPoint(centervec,startradius,endradius,precache_laser,0,0,66,life,width,amplitude,color,speed,0);
        TE_Send(clients,count);
        return 1;
    }
    else
        return -1;
}

BeamFollowEffect(const String:matchstr[],index,Float:life,Float:width,Float:endwidth,const color[4])
{
    new clients[MaxClients];
    new count=FindMatchingPlayers(matchstr,clients);
    if(count)
    {
        TE_SetupBeamFollow(index,precache_laser,0,life,width,endwidth,1,color);
        TE_Send(clients,count);
        return 1;
    }
    else
        return -1;
}

/*FourBeamEffect(client,const String:matchstr[],Float:life,Float:height,Float:width,Float:endwidth,Float:beamwidth,color[4],Float:amplitude,speed)
{
    if(IsValidPlayer(client))
    {
        new Float:indexloc[3],Float:point1[3],Float:point2[3],Float:point3[3],Float:point4[3],Float:point5[3],Float:point6[3],Float:point7[3],Float:point8[3];
        GetClientAbsOrigin(client,indexloc);
        indexloc[2]-=30;
        point1[0]=indexloc[0]+width;
        point1[1]=indexloc[1];
        point1[2]=indexloc[2];
        point2[0]=indexloc[0]-width;
        point2[1]=indexloc[1];
        point2[2]=indexloc[2];
        point3[0]=indexloc[0];
        point3[1]=indexloc[1]+width;
        point3[2]=indexloc[2];
        point4[0]=indexloc[0];
        point4[1]=indexloc[1]-width;
        point4[2]=indexloc[2];
        point5[0]=indexloc[0]+endwidth;
        point5[1]=indexloc[1];
        point5[2]=indexloc[2]+height;
        point6[0]=indexloc[0]-endwidth;
        point6[1]=indexloc[1];
        point6[2]=indexloc[2]+height;
        point7[0]=indexloc[0];
        point7[1]=indexloc[1]+endwidth;
        point7[2]=indexloc[2]+height;
        point8[0]=indexloc[0];
        point8[1]=indexloc[1]-endwidth;
        point8[2]=indexloc[2]+height;
        BeamEffect(matchstr,point1,point5,life,beamwidth,beamwidth,color,amplitude,speed);
        BeamEffect(matchstr,point2,point6,life,beamwidth,beamwidth,color,amplitude,speed);
        BeamEffect(matchstr,point3,point7,life,beamwidth,beamwidth,color,amplitude,speed);
        BeamEffect(matchstr,point4,point8,life,beamwidth,beamwidth,color,amplitude,speed);
        return 1;
    }
    else
        return -1;
}
*/
