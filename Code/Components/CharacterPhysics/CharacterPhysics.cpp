#include "StdAfx.h"
#include "CharacterPhysics.h"

#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CryCore/StaticInstanceList.h>

#include <CryPhysics/physinterface.h>

void register_character_physics(Schematyc::IEnvRegistrar& registrar)
{
	auto scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	auto component_scope = scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CCharacterPhysics));
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&register_character_physics);

void CCharacterPhysics::ReflectType(Schematyc::CTypeDesc<CCharacterPhysics>& desc) noexcept
{
	desc.SetGUID("{8810911C-459F-4083-8C3A-843B062F3049}"_cry_guid);
	desc.SetEditorCategory("Game Components");
	desc.SetName(Schematyc::CTypeName{ "Character Physics" });
	desc.SetLabel("Character Physics");
	desc.SetDescription("Character's Physics Representation");
	desc.SetComponentFlags(EEntityComponentFlags::Singleton);
	desc.AddMember(
			&CCharacterPhysics::character_dimension_
		,	'cdim'
		,	"characterdimensions"
		,	"Character Dimensions"
		,	"Character Dimensions"
		,	SCharacterDimensions{ SDefaults::radius, SDefaults::height, SDefaults::ground_contact_epsilon }
	);

	desc.AddMember(
			&CCharacterPhysics::character_dynamics_
		,	'cdyn'
		,	"characterdynamics"
		,	"Character Dynamics"
		,	"Character Dynamics"
		,	SCharacterDynamics{
					SDefaults::inertia
				,	SDefaults::inertia_acceleration
				,	SDefaults::air_control
				,	SDefaults::air_resistance
				,	SDefaults::mass
			}
	);
}

void CCharacterPhysics::Initialize()
{
	physicalize();
}

Cry::Entity::EventFlags CCharacterPhysics::GetEventMask() const
{
	return Cry::Entity::EEvent::EditorPropertyChanged;
}

void CCharacterPhysics::ProcessEvent(const SEntityEvent& event)
{
	physicalize();
}

void CCharacterPhysics::physicalize() noexcept
{
	const auto character_radius = character_dimension_.radius_ / 2.0f;
	const auto character_height = character_dimension_.height_ / 4.0f;

	auto player_dimensions = pe_player_dimensions{};
	player_dimensions.heightPivot = 0.0f;
	player_dimensions.sizeCollider = { character_radius, 0.0f, character_height };
	player_dimensions.heightCollider = character_radius + character_height + character_dimension_.height_offset_;
	player_dimensions.bUseCapsule = true;
	player_dimensions.groundContactEps = character_dimension_.ground_contact_epsilon_;

	auto player_dynamics = pe_player_dynamics{};
	player_dynamics.kInertia = character_dynamics_.inertia_;
	player_dynamics.kInertiaAccel = character_dynamics_.inertia_acceleration_;
	player_dynamics.kAirControl = character_dynamics_.air_control_;
	player_dynamics.kAirResistance = character_dynamics_.air_resistance_;
	player_dynamics.mass = character_dynamics_.mass_;

	auto physicalize_params = SEntityPhysicalizeParams{};
	physicalize_params.type = pe_type::PE_LIVING;
	physicalize_params.nSlot = GetOrMakeEntitySlotId();
	physicalize_params.pPlayerDimensions = &player_dimensions;
	physicalize_params.pPlayerDynamics = &player_dynamics;

	m_pEntity->Physicalize(physicalize_params);
}