/*

			Functions:
				SetPlayerWaze(playerid, Float:x, Float:y, Float:z, color = 0x8A44E4FF)
				StopWazeGPS(playerid)
				IsValidWazeGPS(playerid)


*/


#if defined include_waze_gps
	#endinput
#endif

#define include_waze_gps

			// Definers
#define 	MAX_WAZE_DOTS 			(100)
#define 	WAZE_UPDATE_TIME 		(3100)

			// Includes
#include <GPS>
#include <YSI_Coding\y_hooks>
#include <YSI_Data\y_iterate>

			// Forrwards
forward UpdateWaze(playerid, Float:X, Float:Y, Float:Z);
forward OnPlayerWazeRouters(Path:pathid, playerid);

enum wazeGPS_Enum {
	wazeGPS_Timer,
	wazeGPS_Color,
	wazeGPS_Routes[MAX_WAZE_DOTS],

	Float:wazeGPS_Position[3],
	Float:wazeGPS_TickPosition[3]
};

static wazeGPS[MAX_PLAYERS][wazeGPS_Enum];

hook OnPlayerConnect(playerid) {

	for(new i = 0; i < MAX_WAZE_DOTS; i++) 
		wazeGPS[playerid][wazeGPS_Routes][i] = -1;

	return true;
}

hook OnPlayerDisconnect(playerid, reason) {
	StopWazeGPS(playerid);

	new dsada[wazeGPS_Enum];
	wazeGPS[playerid] = dsada;
	return true;
}

hook OnPlayerStateChange(playerid, newstate, oldstate) {

	switch(newstate)
	{
		case PLAYER_STATE_ONFOOT: return StopWazeGPS(playerid);
		case PLAYER_STATE_PASSENGER:
		{
			new vehicleid = GetPlayerVehicleID(playerid);
			foreach(new i: Player) {
				if(GetPlayerState(i) != PLAYER_STATE_DRIVER) continue;
				if(GetPlayerVehicleID(i) != vehicleid) continue;

				SetPlayerWaze(playerid, wazeGPS[playerid][wazeGPS_Position][0], wazeGPS[playerid][wazeGPS_Position][1], wazeGPS[playerid][wazeGPS_Position][2], wazeGPS[playerid][wazeGPS_Color]);
				break;
			}
		}
	}
	return true;
}



/*

					.oooooo..o ooooooooooooo   .oooooo.     .oooooo.   oooo    oooo  .oooooo..o 
					d8P'    `Y8 8'   888   `8  d8P'  `Y8b   d8P'  `Y8b  `888   .8P'  d8P'    `Y8 
					Y88bo.           888      888      888 888           888  d8'    Y88bo.      
					`"Y8888o.       888      888      888 888           88888[       `"Y8888o.  
						`"Y88b      888      888      888 888           888`88b.         `"Y88b 
					oo     .d8P      888      `88b    d88' `88b    ooo   888  `88b.  oo     .d8P 
					8""88888P'      o888o      `Y8bood8P'   `Y8bood8P'  o888o  o888o 8""88888P'  

*/

stock SetPlayerWaze(playerid, Float:x, Float:y, Float:z, color = 0x8A44E4FF) {
	
	if(!IsPlayerInAnyVehicle(playerid)) return true;
	DestroyWazeRoutesGPS(playerid);		// Destroy Routes if exists.

	wazeGPS[playerid][wazeGPS_Color] = color;
	wazeGPS[playerid][wazeGPS_Position][0] = x;
	wazeGPS[playerid][wazeGPS_Position][1] = y;
	wazeGPS[playerid][wazeGPS_Position][2] = z;

	if(wazeGPS[playerid][wazeGPS_Timer] == 0) 
		wazeGPS[playerid][wazeGPS_Timer] = SetTimerEx("UpdateWaze", WAZE_UPDATE_TIME, true, "ifff", playerid, x, y, z);

	UpdateWaze(playerid, x, y, z);		// Force First Update
	return true;
}

stock StopWazeGPS(playerid) {
	if(wazeGPS[playerid][wazeGPS_Timer] == 0) return false;
	KillTimer(wazeGPS[playerid][wazeGPS_Timer]);
	wazeGPS[playerid][wazeGPS_Timer] = 0;

	DestroyWazeRoutesGPS(playerid);
	return true;
}

stock IsValidWazeGPS(playerid) 
	return (wazeGPS[playerid][wazeGPS_Timer] <= 0 ? (false) : (true));

static stock DestroyWazeRoutesGPS(playerid) {
	for(new i; i < MAX_WAZE_DOTS; i++) {
		if(wazeGPS[playerid][wazeGPS_Routes][i] == -1) continue;
		GangZoneDestroy(wazeGPS[playerid][wazeGPS_Routes][i]);
		wazeGPS[playerid][wazeGPS_Routes][i] = -1;
	}
}

static stock CreateWazePointer(playerid, Float:X1, Float:Y1, Float:X2, Float:Y2, color) {

	new 
		Float:Dis = 12.5,
		Float:x,
		Float:y,
		Points = floatround(GDBP(X1, Y1, 0.0, X2, Y2, 0.0) / Dis);

	for(new i = 1; i <= Points; i++) {

		switch(i) {
			case 0: {x = X1; y = Y1;}
			default: {x = X1 + (((X2 - X1) / Points) * i); y = Y1 + (((Y2 - Y1) / Points) * i);}
		}

		new slot = 0;
		while(slot <= MAX_WAZE_DOTS) {
			if(slot == MAX_WAZE_DOTS) {
				slot = -1;
				break;
			}
			if(wazeGPS[playerid][wazeGPS_Routes][slot] == -1) break;
			slot++;
		}
		if(slot == -1) return;

		wazeGPS[playerid][wazeGPS_Routes][slot] = GangZoneCreate(x - (Dis / 2)-  5, y - (Dis / 2) - 5, x + (Dis / 2) + 5, y + (Dis / 2) + 5); 
		GangZoneShowForPlayer(playerid, wazeGPS[playerid][wazeGPS_Routes][slot], color);
	}
}

static stock GDBP(Float:X, Float:Y, Float:Z, Float:PointX, Float:PointY, Float:PointZ) {
	new Float:tempFloat = (floatsqroot(floatadd(floatadd(floatpower(floatsub(X, PointX), 2.0), floatpower(floatsub(Y, PointY), 2.0)), floatpower(floatsub(Z, PointZ), 2.0))));
	return _:tempFloat;
}



/*

					ooooooooo.   ooooo     ooo oooooooooo.  ooooo        ooooo   .oooooo.   
					`888   `Y88. `888'     `8' `888'   `Y8b `888'        `888'  d8P'  `Y8b  
					888   .d88'  888       8   888     888  888          888  888          
					888ooo88P'   888       8   888oooo888'  888          888  888          
					888          888       8   888    `88b  888          888  888          
					888          `88.    .8'   888    .88P  888       o  888  `88b    ooo  
					o888o           `YbodP'    o888bood8P'  o888ooooood8 o888o  `Y8bood8P'  

*/

public UpdateWaze(playerid, Float:X, Float:Y, Float:Z) {

	if(GetPlayerInterior(playerid) != 0) 
		return StopWazeGPS(playerid);
	
	if(IsPlayerInRangeOfPoint(playerid, 30.0, wazeGPS[playerid][wazeGPS_TickPosition][0], wazeGPS[playerid][wazeGPS_TickPosition][1], wazeGPS[playerid][wazeGPS_TickPosition][2])) return true;		// Prevent Lags

	new
		MapNode:start,
		MapNode:target;

	GetPlayerPos(playerid, wazeGPS[playerid][wazeGPS_TickPosition][0], wazeGPS[playerid][wazeGPS_TickPosition][1], wazeGPS[playerid][wazeGPS_TickPosition][2]);
    if(GetClosestMapNodeToPoint(wazeGPS[playerid][wazeGPS_TickPosition][0], wazeGPS[playerid][wazeGPS_TickPosition][1], wazeGPS[playerid][wazeGPS_TickPosition][2], start) != 0) return true;
    if(GetClosestMapNodeToPoint(X, Y, Z, target)) return true;
	if(FindPathThreaded(start, target, "OnPlayerWazeRouters", "i", playerid)) return true;
    return true;
}

public OnPlayerWazeRouters(Path:pathid, playerid) {
	if(wazeGPS[playerid][wazeGPS_Timer] == 0) return true;
    if(!IsValidPath(pathid)) return true;

	new 
		Float:length,
		size;
	
	GetPathSize(pathid, size);
	GetPathLength(pathid, length);

	if(size == 1) return StopWazeGPS(playerid);
	DestroyWazeRoutesGPS(playerid);

	new 
		MapNode:nodeid,
		Float:x,
		Float:y,
		Float:z,
		index;

	GetPlayerPos(playerid, x, y, z);
	GetClosestMapNodeToPoint(x, y, z, nodeid);
	GetMapNodePos(nodeid, x, y, z);

	new 
		_size = MAX_WAZE_DOTS,
		Float:X, 
		Float:Y, 
		Float:Z;
	
	if(MAX_WAZE_DOTS > size) _size = size;

	for(new i = 0; i < _size; i++) {
		GetPathNode(pathid, i, nodeid);
		GetPathNodeIndex(pathid, nodeid, index);
		GetMapNodePos(nodeid, X, Y, Z);

		if(i == index) 
			CreateWazePointer(playerid, x, y, X, Y, wazeGPS[playerid][wazeGPS_Color]);

		x = X + 0.5;
		y = Y + 0.5;
	}
    return true;
}
