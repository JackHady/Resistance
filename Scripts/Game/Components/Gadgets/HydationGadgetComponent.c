class SCR_HydrationGadgetComponentClass : SCR_GadgetComponentClass
{
}

class SCR_HydrationGadgetComponent : SCR_GadgetComponent
{

    [Attribute(defvalue: "0.0", desc: "Hydration added per use. Can be negative for dehydration.")]
    protected float m_iWaterValue;
	
	protected RplComponent m_RplComp;
	protected ChimeraCharacter m_User;
	protected AnimationEventID m_iDetonatorTriggeredEventID = -1;
	
	protected const string TRIGGER_ANIMATION_EVENT_NAME = "DetonatorTriggered";
	protected const string ANIMATION_BIND_COMMAND = "CMD_Item_Action";
	
	protected const float DURATION = 3.0; // Duration of the drinking animation in seconds
	protected GadgetAnimationComponent m_GadgetAnimationComponent;
	
	override void ToggleActive(bool state, SCR_EUseContext context)
	{
		if (m_eUseMask == SCR_EUseContext.NONE || (m_eUseMask & context) == 0)
			return;

		if (!m_User)
		{
			m_User = m_CharacterOwner;

			if (!m_User)
				return;
		}

		ChimeraCharacter character = m_CharacterOwner;
		if (character)
		{
			SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(character.GetCharacterController());
	
			if (!charController || charController.GetLifeState() != ECharacterLifeState.ALIVE)
				return;
	
			if (!charController.CanPlayItemGesture() && charController.GetInspectEntity() != GetOwner())
				return;
		}

		RplComponent rplComponent = RplComponent.Cast(m_User.FindComponent(RplComponent));
		if (!rplComponent || !rplComponent.IsOwner())
			return;					// NOT owner of the character in possession of this gadget

		// Client side
		rplComponent = RplComponent.Cast(GetOwner().FindComponent(RplComponent));
		if (rplComponent && rplComponent.IsProxy())
			OnToggleActive(state);	// activate client side to avoid server delay

		// Sync
		SCR_GadgetManagerComponent.GetGadgetManager(m_User).AskToggleGadget(this, state);
	}
	
		//------------------------------------------------------------------------------------------------
	override void OnToggleActive(bool state)
	{
		if (!m_User)
		{
			if (!m_CharacterOwner)
				return;

			m_User = m_CharacterOwner;
			if (!m_User)
				return;
		}

		if (!m_CharacterOwner)
		{
			return;
		}
		
		PlaySound(SCR_SoundEvent.SOUND_DETONATOR_DETONATE_CHARGES);
		ScheduleDeletion();
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_User.GetCharacterController());

		CharacterAnimationComponent pAnimationComponent = m_User.GetAnimationComponent();
		int itemActionId = pAnimationComponent.BindCommand(ANIMATION_BIND_COMMAND);

		ItemUseParameters animParams = new ItemUseParameters();
		animParams.SetEntity(GetOwner());
		animParams.SetAllowMovementDuringAction(true);
		animParams.SetKeepInHandAfterSuccess(true);
		animParams.SetCommandID(itemActionId);
		animParams.SetCommandIntArg(1);
		//Print("Playing Animation");
		
		charController.TryUseItemOverrideParams(animParams);
	}
	
		//------------------------------------------------------------------------------------------------
	protected void ApplyMetabolismEffect()
	{
    	IEntity owner = GetOwner();
    	IEntity characterOwner = owner;
    	if (!SCR_CharacterControllerComponent.Cast(owner.FindComponent(SCR_CharacterControllerComponent)))
	    {
        	characterOwner = owner.GetParent();
    	}
    
    	if (characterOwner)
    	{
        	SCR_CharacterControllerComponent metabolismComponent = SCR_CharacterControllerComponent.Cast(characterOwner.FindComponent(SCR_CharacterControllerComponent));
        	if (metabolismComponent)
        	{
            	metabolismComponent.IncreaseHydration(m_iWaterValue); // Increase hydration
        	}
    	}
	}

    float GetWaterValue()
    {
        return m_iWaterValue;
    }

	void Delete()
	{
    	//Print("Delete called. Preparing to clean up the gadget.");
		ApplyMetabolismEffect();
    	// Validate GadgetAnimationComponent before deletion
    	if (m_GadgetAnimationComponent && m_CharacterOwner)
    	{
        	ChimeraCharacter characterOwner = (m_CharacterOwner);
        	if (characterOwner)
        	{
            	m_GadgetAnimationComponent.RemoveSyncReference(characterOwner);
            	//Print("Removed sync reference from GadgetAnimationComponent.");
        	}
    	}
    	else
    	{
        	//Print("GadgetAnimationComponent or CharacterOwner is null. Skipping sync removal.");
    	}

    	// Proceed with deletion
    	if (m_CharacterOwner)
    	{
        	SCR_EntityHelper.DeleteEntityAndChildren(GetOwner());
        	//Print("Gadget entity deleted.");
        	m_bActivated = false;
    	}
	}

	protected void OnAnimationEnded(IEntity item, bool successful, ItemUseParameters animParams)
	{
		if (!m_User)
			return;

		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(m_User.GetCharacterController());
		m_User = null;

	}
	
	void ActivateAction(notnull IEntity pUserEntity)
	{
		m_User = ChimeraCharacter.Cast(pUserEntity);
		ActivateAction();
	}
	
	protected override void ActivateAction()
	{
		ToggleActive(false, SCR_EUseContext.FROM_ACTION);
	}
	
	protected void PlaySound(string soundName)
	{
    	SoundComponent soundComp = SoundComponent.Cast(GetOwner().FindComponent(SoundComponent));
    
    	if (soundComp)
    	{
        	// Schedule the sound to play after 1 second (1000 milliseconds)
        	GetGame().GetCallqueue().CallLater(soundComp.SoundEvent, 1000, false, soundName);
    	}
	}

	
	override EGadgetType GetType()
	{
		return EGadgetType.CONSUMABLE;
	}
	
	override bool CanBeRaised()
	{
		return true;
	}
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	protected void ScheduleDeletion()
	{
    	//Print("ScheduleDeletion called. Verifying GadgetAnimationComponent.");

    	if (!m_GadgetAnimationComponent)
    {
        //Print("Warning: GadgetAnimationComponent not found. Scheduling deletion anyway.");
    }

    	// Schedule deletion after the defined duration
    	GetGame().GetCallqueue().CallLater(Delete, DURATION * 1000, false);
    	//Print("Item scheduled for deletion after " + DURATION + " seconds.");
	}
}
