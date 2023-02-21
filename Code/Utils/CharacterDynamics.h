#pragma once
#include <CrySchematyc/MathTypes.h>

struct SCharacterDynamics {
	Schematyc::PositiveFloat inertia_{};
	Schematyc::PositiveFloat inertia_acceleration_{};
	Schematyc::PositiveFloat air_control_{};
	Schematyc::PositiveFloat air_resistance_{};
	Schematyc::PositiveFloat mass_{};
};

inline bool operator==(const SCharacterDynamics& lhs, const SCharacterDynamics& rhs) noexcept
{
	return std::memcmp(&lhs, &rhs, sizeof(SCharacterDynamics) == 0);
}

inline bool operator!=(const SCharacterDynamics& lhs, const SCharacterDynamics& rhs) noexcept
{
	return !(operator==(lhs, rhs));
}

static void ReflectType(Schematyc::CTypeDesc<SCharacterDynamics>& desc) noexcept
{
	desc.SetGUID("{9523185F-D9B6-4D23-9F89-E70EBA439DDC}"_cry_guid);
	desc.AddMember(
			&SCharacterDynamics::inertia_
		,	'iner'
		,	"inertia"
		,	"Inertia"
		,	"Character Physics Inertia"
		,	Schematyc::PositiveFloat{}
	);

	desc.AddMember(
			&SCharacterDynamics::inertia_acceleration_
		,	'iacc'
		,	"inertiaacceleration"
		,	"Inertia Acceleration"
		,	"Character Physics Inertia Acceleration"
		,	Schematyc::PositiveFloat{}
	);

	desc.AddMember(
			&SCharacterDynamics::air_control_
		,	'acon'
		,	"aircontrol"
		,	"Air Control"
		,	"Character Physics Air Control"
		,	Schematyc::PositiveFloat{}
	);

	desc.AddMember(
			&SCharacterDynamics::air_resistance_
		,	'ares'
		,	"airresistance"
		,	"Air Resistance"
		,	"Character Physics Air Resistance"
		,	Schematyc::PositiveFloat{}
	);

	desc.AddMember(
			&SCharacterDynamics::mass_
		,	'mass'
		,	"mass"
		,	"Mass"
		,	"Character Physics Mass"
		,	Schematyc::PositiveFloat{}
	);
}