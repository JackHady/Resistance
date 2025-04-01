class HydratePlayerAction : ScriptedUserAction
{
    [Attribute(defvalue: "20.0", desc: "Hydration added per use. Can be negative for dehydration.")]
    protected float m_iWaterValue;

    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
    {
        Print("Initiating hydration action...");

        // Ensure the user performing the action is the owner
        if (EntityUtils.GetPlayer() != pUserEntity){
			//Print("not my player");
			return;
		}

        // Apply hydration effect to the player
        ApplyHydrationEffect(pUserEntity);
    }

    protected void ApplyHydrationEffect(IEntity characterOwner)
    {
        if (characterOwner)
        {
            SCR_CharacterControllerComponent metabolismComponent = SCR_CharacterControllerComponent.Cast(characterOwner.FindComponent(SCR_CharacterControllerComponent));
            if (metabolismComponent)
            {
                Print("SCR_CharacterControllerComponent found.");

                // Apply hydration changes (can be negative, e.g., for thirst increase)
                metabolismComponent.IncreaseHydration(m_iWaterValue);

                float currentHydration = metabolismComponent.GetHydration();
                Print("Increased hydration by " + m_iWaterValue);
                Print("Current Hydration: " + currentHydration);
            }
            else
            {
                Print("SCR_CharacterControllerComponent not found on character.");
            }
        }
        else
        {
            Print("Character not found.");
        }
    }
}
