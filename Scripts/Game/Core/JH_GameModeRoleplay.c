[EntityEditorProps(category: "Resistance/Core", description: "Core gamemode")]
class JH_GameModeRoleplayClass: EL_GameModeRoleplayClass
{
}

class JH_GameModeRoleplay: EL_GameModeRoleplay
{
	//------------------------------------------------------------------------------------------------
	void ~JH_GameModeRoleplay()
	{
		JH_PlayerAccountManager.Reset();
	}
	
	override void OnPlayerConnected(int playerId)
    {
        super.OnPlayerConnected(playerId);
		
		GetGame().GetCallqueue().CallLater(LogPlayerConnection, 1000, false, playerId);
	}
	
	void LogPlayerConnection(int playerId)
	{
	    Print("[JH_GameModeRoleplay] Player Connected (Delayed Log): " + playerId);
	}
}
