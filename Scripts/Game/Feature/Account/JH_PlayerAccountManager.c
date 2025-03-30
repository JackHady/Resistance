class JH_PlayerAccountManager
{
	protected static ref JH_PlayerAccountManager s_pInstance;
	protected ref map<string, ref JH_PlayerAccount> m_mAccounts;

	//------------------------------------------------------------------------------------------------
	//! Async loading of a players account
	//! \param playerUid The players Bohemia UID
	//! \param create If true a new account will be created if none is found for the UID
	//! \param callback Async callback to handle the result
	void LoadAccountAsync(string playerUid, bool create, notnull EDF_DataCallbackSingle<JH_PlayerAccount> callback)
	{
		JH_PlayerAccount account = m_mAccounts.Get(playerUid);
		if (account)
		{
			callback.Invoke(account);
			return;
		}

		auto processorCallback = JH_PlayerAccountManagerProcessorCallback.Create(playerUid, create, callback);
		EPF_PersistentScriptedStateLoader<JH_PlayerAccount>.LoadAsync(playerUid, processorCallback);
	}

	//------------------------------------------------------------------------------------------------
	//! Save and pending changes on the account and release it from the manager. Used primarily on disconnect of player
	//! \param playerUid The players Bohemia UID
	void SaveAndReleaseAccount(notnull JH_PlayerAccount account)
	{
		account.PauseTracking();
		account.Save();
		m_mAccounts.Remove(account.GetPersistentId());
	}

	//------------------------------------------------------------------------------------------------
	//! Add the player accountt instance to the cache so it is returned on the next LoadAccountAsync call
	//! \param account Account instance to cache
	void AddToCache(notnull JH_PlayerAccount account)
	{
		m_mAccounts.Set(account.GetPersistentId(), account);
	}

	//------------------------------------------------------------------------------------------------
	JH_PlayerAccount GetFromCache(string playerUid)
	{
		return m_mAccounts.Get(playerUid);
	}

	//------------------------------------------------------------------------------------------------
	JH_PlayerAccount GetFromCache(int playerId)
	{
		return GetFromCache(EL_Utils.GetPlayerUID(playerId));
	}

	//------------------------------------------------------------------------------------------------
	JH_PlayerAccount GetFromCache(IEntity player)
	{
		return GetFromCache(EL_Utils.GetPlayerUID(player));
	}

	//------------------------------------------------------------------------------------------------
	static JH_PlayerAccountManager GetInstance()
	{
		if (!s_pInstance)
			s_pInstance = new JH_PlayerAccountManager();

		return s_pInstance;
	}

	//------------------------------------------------------------------------------------------------
	static void Reset()
	{
		s_pInstance = null;
	}

	//------------------------------------------------------------------------------------------------
	protected void JH_PlayerAccountManager()
	{
		m_mAccounts = new map<string, ref JH_PlayerAccount>()
	}
};

class JH_PlayerAccountManagerProcessorCallback : EDF_DataCallbackSingle<JH_PlayerAccount>
{
	string m_sPlayerUid
	bool m_bCreate;
	ref EDF_DataCallbackSingle<JH_PlayerAccount> m_pCallback;

	//------------------------------------------------------------------------------------------------
	override void OnComplete(JH_PlayerAccount data, Managed context)
	{
		JH_PlayerAccount result = data; //Keep explicit strong ref to it or else create on null will fail
		if (!result && m_bCreate)
			result = JH_PlayerAccount.Create(m_sPlayerUid);

		if (result)
			JH_PlayerAccountManager.GetInstance().AddToCache(result);

		if (m_pCallback)
			m_pCallback.Invoke(result);
	}

	//------------------------------------------------------------------------------------------------
	static JH_PlayerAccountManagerProcessorCallback Create(string playerUid, bool create, EDF_DataCallbackSingle<JH_PlayerAccount> callback)
	{
		JH_PlayerAccountManagerProcessorCallback instance();
		instance.m_sPlayerUid = playerUid;
		instance.m_bCreate = create;
		instance.m_pCallback = callback;
		return instance;
	}
};
