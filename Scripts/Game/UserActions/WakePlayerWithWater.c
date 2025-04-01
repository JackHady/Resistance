class WakePlayerAction : ScriptedUserAction
{
    [Attribute(defvalue: "0.25", desc: "Hydration added per use. Can be negative for dehydration.")]
    protected float m_iWaterValue;

    [Attribute(defvalue: "0.15", desc: "Energy added per use. Can be negative for hunger.")]
    protected float m_iEnergyValue;

    // Determines whether the action can be performed
    override bool CanBeShownScript(IEntity user)
    {
        // Validate the target player entity (the unconscious player)
        IEntity targetEntity = GetOwner(); // The player being woken
        if (!targetEntity)
        {
            return false; // Action won't be shown
        }

        // Check unconscious state via DamageManagerComponent
        SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(targetEntity.FindComponent(SCR_CharacterDamageManagerComponent));
        if (!damageManager || !damageManager.IsUnconscious())
        {
            return false; // Action won't be shown
        }

        // Validate user entity (the player giving the water)
        ChimeraCharacter userCharacter = ChimeraCharacter.Cast(user);
        if (!userCharacter)
        {
            return false; // Action won't be shown
        }

        // Check if the user's inventory contains the required canteen prefab
        if (!HasRequiredCanteen(userCharacter))
        {
            return false; // Action won't be shown
        }

        return true; // All conditions met, action can be shown
    }

    // Executes the action
    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
    {
        // Ensure the user performing the action is valid
        ChimeraCharacter userCharacter = ChimeraCharacter.Cast(pUserEntity);
        if (!userCharacter)
        {
            return;
        }

        // Remove the required item from the user's inventory
        if (!RemoveRequiredCanteen(userCharacter))
        {
            return;
        }

        // Execute the logic on the server
        RplComponent rplComponent = RplComponent.Cast(pOwnerEntity.FindComponent(RplComponent));
        if (rplComponent && rplComponent.IsProxy())
        {
            RpcDoApplyMetabolismEffect(pOwnerEntity);
            return;
        }

        ApplyMetabolismEffect(pOwnerEntity); // Apply on the server
    }

    // Applies the metabolism effect on the server
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    protected void RpcDoApplyMetabolismEffect(IEntity pOwnerEntity)
    {
        ApplyMetabolismEffect(pOwnerEntity);
    }

    protected void ApplyMetabolismEffect(IEntity pOwnerEntity)
    {
        // Retrieve MetabolismComponent to apply hydration and energy effects
        SCR_CharacterControllerComponent metabolismComponent = SCR_CharacterControllerComponent.Cast(pOwnerEntity.FindComponent(SCR_CharacterControllerComponent));
        if (metabolismComponent)
        {
            metabolismComponent.IncreaseHydration(m_iWaterValue);
            metabolismComponent.IncreaseEnergy(m_iEnergyValue);
        }

        // Update consciousness state
        SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_CharacterDamageManagerComponent));
        if (damageManager)
        {
            damageManager.UpdateConsciousness(); // Revive the player
        }
    }

    // Checks if the user's inventory contains the required canteen prefab
    protected bool HasRequiredCanteen(ChimeraCharacter userCharacter)
    {
        InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(userCharacter.FindComponent(InventoryStorageManagerComponent));
        if (!inventory)
        {
            return false;
        }

        // Iterate through inventory items and look for the specific prefabs
        array<IEntity> inventoryItems = {};
        inventory.GetItems(inventoryItems);
        foreach (IEntity item : inventoryItems)
        {
            ResourceName resourceName = item.GetPrefabData().GetPrefabName();
            if (resourceName == "{54318E98BC91C39E}Prefabs/Items/Equipment/Canteens/Canteen.et" || resourceName == "{52BF2274BD8AFF01}Prefabs/Items/Equipment/Canteens/CanteenUSSR.et")
            {
                return true; // Found a required canteen
            }
        }

        return false;
    }

    // Removes the required canteen prefab from the user's inventory
    protected bool RemoveRequiredCanteen(ChimeraCharacter userCharacter)
    {
        InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(userCharacter.FindComponent(InventoryStorageManagerComponent));
        if (!inventory)
        {
            return false;
        }

        // Iterate through inventory items and remove the specific prefab
        array<IEntity> inventoryItems = {};
        inventory.GetItems(inventoryItems);
        foreach (IEntity item : inventoryItems)
        {
            ResourceName resourceName = item.GetPrefabData().GetPrefabName();
            if (resourceName == "{54318E98BC91C39E}Prefabs/Items/Equipment/Canteens/Canteen.et" || resourceName == "{52BF2274BD8AFF01}Prefabs/Items/Equipment/Canteens/CanteenUSSR.et")
            {
                inventory.TryDeleteItem(item); // Remove the item from the inventory
                return true;
            }
        }

        return false;
    }
}