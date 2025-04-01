modded class SCR_CharacterControllerComponent
{
    private float m_fHydration = 1.0; // Player's hydration level
    private float m_fEnergy = 1.0; // Player's energy level

    private float m_fDehydrationDamage = 4.0; // Damage multiplier for dehydration
    private float m_fStarvationDamage = 2.0; // Damage multiplier for starvation

    private bool m_bIsThirsty = false; // Tracks if the player is thirsty
    private bool m_bIsHungry = false; // Tracks if the player is hungry
	
	protected const float HUNGER_DECAY_RATE = 0.05; // Per tick
    protected const float THIRST_DECAY_RATE = 0.07; // Per tick
    protected const float TICK_INTERVAL = 10; // secondes

    IEntity characterOwner;

    override void OnControlledByPlayer(IEntity owner, bool controlled)
    {
        super.OnControlledByPlayer(owner, controlled);
        characterOwner = owner;

        if (EntityUtils.GetPlayer() != owner) return;
		
		InputManager inputManager = GetGame().GetInputManager();
	    if (!inputManager) return;
	
	    // Vérifie si un consommable est tenu avant d'ajouter l'input
	    if (HasConsumableItem())
	    {
	        inputManager.AddActionListener("IA_HoldF", EActionTrigger.DOWN, ConsumeItem);
	    }

        GetGame().GetCallqueue().CallLater(UpdateMetabolism, TICK_INTERVAL * 1000, true, 1);
    }
	
	bool HasConsumableItem()
	{
	    IEntity item = GetHeldItem();
	    if (!item) return false;
	
	    return item.FindComponent(SCR_ConsumableEffectBase) != null;
	}

    void UpdateMetabolism(float timeSlice)
	{
    	if (!characterOwner) return;

    	HandleHydration(timeSlice);
    	HandleEnergy(timeSlice);

    	if (m_fHydration <= 0 && m_fEnergy <= 0)
		{
    	Rpc(RpcForceUnconscious);
		}
	}


    void HandleHydration(float timeSlice)
    {
        if (m_fHydration > 0)
        {
            m_fHydration = Math.Clamp(m_fHydration - THIRST_DECAY_RATE, 0.0, 1.0);
        }
        else
        {
            ApplyDamageFromDehydration(timeSlice);
        }

        m_bIsThirsty = m_fHydration < 0.25;

        if (m_bIsThirsty)
        {
            SCR_CharacterStaminaComponent StamComponent = SCR_CharacterStaminaComponent.Cast(characterOwner.FindComponent(SCR_CharacterStaminaComponent));
            if (StamComponent) StamComponent.AddStamina(-1);
        }
    }

    void HandleEnergy(float timeSlice)
    {
        if (m_fEnergy > 0)
        {
            m_fEnergy = Math.Clamp(m_fEnergy - HUNGER_DECAY_RATE, 0.0, 1.0);
        }
        else
        {
            ApplyDamageFromStarvation(timeSlice);
        }

        m_bIsHungry = m_fEnergy < 0.1;
    }

    void ApplyDamageFromDehydration(float timeSlice)
    {
        if (m_fHydration <= 0 && timeSlice > 0)
        {
            float damage = m_fDehydrationDamage * timeSlice;
            Rpc(RpcApplyDams, damage);
        }
    }

    void ApplyDamageFromStarvation(float timeSlice)
    {
        if (m_fEnergy <= 0 && timeSlice > 0)
        {
            float damage = m_fStarvationDamage * timeSlice;
            Rpc(RpcApplyDams, damage);
        }
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void RpcApplyDams(float damage)
    {
        ApplyDamages(damage);
    }

    void ApplyDamages(float damage)
    {
        DamageManagerComponent damageManager = DamageManagerComponent.Cast(characterOwner.FindComponent(DamageManagerComponent));
        if (damageManager)
        {
            HitZone defaultHitZone = damageManager.GetDefaultHitZone();
            if (defaultHitZone)
            {
                float newHealth = Math.Max(defaultHitZone.GetHealth() - damage, 0);
                defaultHitZone.SetHealth(newHealth);
            }
        }
    }

    bool CanSprint() { return m_fHydration >= 0.2; }

    void IncreaseHydration(float amount)
    {
        m_fHydration = Math.Clamp(m_fHydration + amount, 0.0, 1.0);
    }

    void IncreaseEnergy(float amount)
    {
        m_fEnergy = Math.Clamp(m_fEnergy + amount, 0.0, 1.0);
    }
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcForceUnconscious()
	{
    	if (!characterOwner) return;

    	SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(characterOwner.FindComponent(SCR_CharacterDamageManagerComponent));
    	if (damageManager)
    	{
        	damageManager.ForceUnconsciousness();
    	}
	}

    float GetHydration() { return m_fHydration; }
    float GetEnergy() { return m_fEnergy; }
    bool IsThirsty() { return m_bIsThirsty; }
    bool IsHungry() { return m_bIsHungry; }
}