class JH_ConsumableComponentClass : SCR_GadgetComponentClass
{
}

class JH_ConsumableComponent : SCR_GadgetComponent
{
    void ConsumeItem()
    {
        IEntity item = GetHeldItem();
        if (!item) return;

        if (SCR_ConsumableEffectBase consumable = SCR_ConsumableEffectBase.Cast(item.FindComponent(SCR_ConsumableEffectBase)))
        {
            consumable.ApplyEffect(GetOwner());

            if (InventoryStorageManagerComponent inventory = InventoryStorageManagerComponent.Cast(GetOwner().FindComponent(InventoryStorageManagerComponent)))
            {
                inventory.RemoveItem(item);
            }
        }
    }

    bool HasConsumableItem()
    {
        return GetHeldItem() && GetHeldItem().FindComponent(SCR_ConsumableEffectBase);
    }

    IEntity GetHeldItem()
    {
        CharacterControllerComponent controller = CharacterControllerComponent.Cast(GetOwner().FindComponent(CharacterControllerComponent));
        return controller ? controller.GetHeldItem() : null;
    }
}