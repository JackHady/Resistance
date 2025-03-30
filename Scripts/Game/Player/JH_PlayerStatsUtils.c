enum JH_EPlayerStatType
{
    MONEY,
    THIRST,
    HUNGER
}

class JH_PlayerStatsUtils
{
    private static ref map<int, ref map<JH_EPlayerStatType, float>> playerStats = new map<int, ref map<JH_EPlayerStatType, float>>;

    //------------------------------------------------------------------------------------------------
    // Ensure player stats exist
    private static void EnsurePlayerExists(int playerId)
    {
        if (!playerStats.Contains(playerId))
        {
            playerStats[playerId] = new map<JH_EPlayerStatType, float>;
            playerStats[playerId][JH_EPlayerStatType.MONEY] = 0;
            playerStats[playerId][JH_EPlayerStatType.THIRST] = 100; // Default full thirst
            playerStats[playerId][JH_EPlayerStatType.HUNGER] = 100; // Default full hunger
        }
    }

    //------------------------------------------------------------------------------------------------
    // Get Player Stat
    static float GetPlayerStat(int playerId, JH_EPlayerStatType statType)
    {
        EnsurePlayerExists(playerId);
        return playerStats[playerId][statType];
    }

    //------------------------------------------------------------------------------------------------
    // Set Player Stat
    static void SetPlayerStat(int playerId, JH_EPlayerStatType statType, float value)
    {
        EnsurePlayerExists(playerId);
        playerStats[playerId][statType] = Math.Max(value, 0); // Prevent negative values
    }

    //------------------------------------------------------------------------------------------------
    // Add to Player Stat
    static void AddPlayerStat(int playerId, JH_EPlayerStatType statType, float amount)
    {
        EnsurePlayerExists(playerId);
        playerStats[playerId][statType] = Math.Max(playerStats[playerId][statType] + amount, 0);
    }

    //------------------------------------------------------------------------------------------------
    // Remove from Player Stat
    static void RemovePlayerStat(int playerId, JH_EPlayerStatType statType, float amount)
    {
        EnsurePlayerExists(playerId);
        playerStats[playerId][statType] = Math.Max(playerStats[playerId][statType] - amount, 0);
    }
}
