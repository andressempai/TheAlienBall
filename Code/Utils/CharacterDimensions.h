#pragma once
#include <CrySchematyc/MathTypes.h>

struct SCharacterDimensions {
	Schematyc::PositiveFloat radius_{};
	Schematyc::PositiveFloat height_{};
	Schematyc::PositiveFloat height_offset_{};
	Schematyc::PositiveFloat ground_contact_epsilon_{};
};

inline bool operator==(const SCharacterDimensions& lhs, const SCharacterDimensions& rhs) noexcept
{
	return std::memcmp(&lhs, &rhs, sizeof(SCharacterDimensions) == 0);
}

inline bool operator!=(const SCharacterDimensions& lhs, const SCharacterDimensions& rhs) noexcept
{
	return !(operator==(lhs, rhs));
}

static void ReflectType(Schematyc::CTypeDesc<SCharacterDimensions>& desc) noexcept
{
	desc.SetGUID("{B40866A5-77E6-41B9-8C67-88A892ECB344}"_cry_guid);
	desc.AddMember(
			&SCharacterDimensions::radius_
		,	'rad'
		,	"radius"
		,	"Radiuss"
		,	"Character Physics Radius"
		,	Schematyc::PositiveFloat{}
	);

	desc.AddMember(
			&SCharacterDimensions::height_
		,	'hei'
		,	"height"
		,	"Height"
		,	"Character Physics Height"
		,	Schematyc::PositiveFloat{}
	);

	desc.AddMember(
			&SCharacterDimensions::height_offset_
		,	'hoff'
		,	"heightoffset"
		,	"Height Offset"
		,	"Character Physics Height Offset"
		,	Schematyc::PositiveFloat{}
	);

	desc.AddMember(
			&SCharacterDimensions::ground_contact_epsilon_
		,	'gcep'
		,	"groundcontactepsilon"
		,	"Ground Contact Epsilon"
		,	"Character Physics Ground Contact Epsilon"
		,	Schematyc::PositiveFloat{}
	);
}