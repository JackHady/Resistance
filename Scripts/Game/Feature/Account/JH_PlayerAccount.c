class JH_PlayerAccount : EPF_PersistentScriptedState
{
	/*protected*/ ref array<ref JH_PlayerCharacter> m_aCharacters = {};
	/*protected*/ int m_iActiveCharacterIdx;
	
	protected float m_fHunger = 100; // Default full hunger
	protected float m_fThirst = 100; // Default full thirst
	protected const float HUNGER_DECAY_RATE = 0.1; // Per tick
    protected const float THIRST_DECAY_RATE = 0.15; // Per tick
    protected const float TICK_INTERVAL = 1.0; // secondes
	
	protected ref ScriptInvoker m_OnStatsChanged = new ScriptInvoker(); // Event pour update UI
	

    void JH_PlayerAccount()
    {
		Print("[JH_PlayerAccount]");
        GetGame().GetCallqueue().CallLater(DecayStats, TICK_INTERVAL * 1000, true); // Tick loop
    }
	
	// Getters
	float GetHunger() { return m_fHunger; }
	float GetThirst() { return m_fThirst; }
	
	// Setters avec clamping (0-100) et sauvegarde automatique
    void SetHunger(float value) 
    {
        m_fHunger = Math.Clamp(value, 0, 100);
        Save();
        m_OnStatsChanged.Invoke();
    }

    void SetThirst(float value) 
    {
        m_fThirst = Math.Clamp(value, 0, 100);
        Save();
        m_OnStatsChanged.Invoke();
    }

    // Faim et soif baissent progressivement
    protected void DecayStats()
    {
        SetHunger(m_fHunger - HUNGER_DECAY_RATE);
        SetThirst(m_fThirst - THIRST_DECAY_RATE);
        Print("[JH_PlayerAccount] Hunger: " + m_fHunger + " | Thirst: " + m_fThirst);
    }
	
	

	//------------------------------------------------------------------------------------------------
	void AddCharacter(notnull JH_PlayerCharacter character, bool setAsActive = false)
	{
		int idx = m_aCharacters.Insert(character);
		if (setAsActive)
			m_iActiveCharacterIdx = idx;
	}

	//------------------------------------------------------------------------------------------------
	void RemoveCharacter(notnull JH_PlayerCharacter character)
	{
		m_aCharacters.RemoveItemOrdered(character);
	}

	//------------------------------------------------------------------------------------------------
	bool HasCharacters()
	{
		return !m_aCharacters.IsEmpty();
	}

	//------------------------------------------------------------------------------------------------
	array<JH_PlayerCharacter> GetCharacters()
	{
		array<JH_PlayerCharacter> results();
		results.Reserve(m_aCharacters.Count());
		foreach (JH_PlayerCharacter character : m_aCharacters)
		{
			results.Insert(character);
		}
		return results;
	}

	//------------------------------------------------------------------------------------------------
	JH_PlayerCharacter GetActiveCharacter()
	{
		if (!m_aCharacters.IsEmpty())
			return m_aCharacters.Get(m_iActiveCharacterIdx);

		return null;
	}

	//------------------------------------------------------------------------------------------------
	void SetActiveCharacter(notnull JH_PlayerCharacter character)
	{
		m_iActiveCharacterIdx = m_aCharacters.Find(character);
	}

	//------------------------------------------------------------------------------------------------
	static JH_PlayerAccount Create(string playerUid)
	{
		JH_PlayerAccount account();
		account.SetPersistentId(playerUid);
		return account;
	}
};

class JH_PlayerCharacter
{
	protected string m_sId;
	protected ResourceName m_rPrefab;

	//------------------------------------------------------------------------------------------------
	string GetId()
	{
		return m_sId;
	}

	//------------------------------------------------------------------------------------------------
	ResourceName GetPrefab()
	{
		return m_rPrefab;
	}

	//------------------------------------------------------------------------------------------------
	static JH_PlayerCharacter Create(ResourceName prefab)
	{
		JH_PlayerCharacter character();
		character.m_sId = EPF_PersistenceIdGenerator.Generate();
		character.m_rPrefab = prefab;
		return character;
	}
};
