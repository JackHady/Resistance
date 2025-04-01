modded class JH_PlayerMetabolismComponent : ScriptComponent
{
    protected float m_fHydration = 1.0;
    protected float m_fEnergy = 1.0;
    
    protected const float HUNGER_DECAY_RATE = 0.05;
    protected const float THIRST_DECAY_RATE = 0.07;
    protected const float TICK_INTERVAL = 10.0; // secondes
    
    override void OnPostInit(IEntity owner)
    {
        super.OnPostInit(owner);
        GetGame().GetCallqueue().CallLater(UpdateMetabolism, TICK_INTERVAL * 1000, true);
    }

    void UpdateMetabolism()
    {
        if (!GetOwner()) return;

        // Gérer faim & soif
        m_fHydration = Math.Clamp(m_fHydration - THIRST_DECAY_RATE, 0.0, 1.0);
        m_fEnergy = Math.Clamp(m_fEnergy - HUNGER_DECAY_RATE, 0.0, 1.0);
        
        // Appliquer dommages si besoin
        if (m_fHydration <= 0) ApplyDamage(4.0);
        if (m_fEnergy <= 0) ApplyDamage(2.0);
    }

    void ApplyDamage(float amount)
    {
        DamageManagerComponent dmc = DamageManagerComponent.Cast(GetOwner().FindComponent(DamageManagerComponent));
        if (!dmc) return;

        HitZone defaultHitZone = dmc.GetDefaultHitZone();
        if (defaultHitZone)
        {
            defaultHitZone.SetHealth(Math.Max(defaultHitZone.GetHealth() - amount, 0));
        }
    }

    float GetHydration() { return m_fHydration; }
    float GetEnergy() { return m_fEnergy; }
}