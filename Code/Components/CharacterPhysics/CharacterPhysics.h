#pragma once

#include <CryEntitySystem/IEntityComponent.h>
#include <CrySchematyc/MathTypes.h>

#include "../../Utils/CharacterDimensions.h"
#include "../../Utils/CharacterDynamics.h"

#include "../../Previewers/PhysicalEntityPreviewers.h"

class CCharacterPhysics final : public IEntityComponent {
	struct SDefaults {
		static constexpr auto radius = 0.6f;
		static constexpr auto height = 1.753f;
		static constexpr auto height_offset = 0.05f;
		static constexpr auto ground_contact_epsilon = 0.004f;

		static constexpr auto inertia = 6.35f;
		static constexpr auto inertia_acceleration = 3.35f;
		static constexpr auto air_control = 0.65f;
		static constexpr auto air_resistance = 0.2f;
		static constexpr auto mass = 71.0f;
	};

public:
	static void ReflectType(Schematyc::CTypeDesc<CCharacterPhysics>& desc) noexcept;

private:
	virtual void Initialize() override;
	virtual Cry::Entity::EventFlags GetEventMask() const override;
	virtual void ProcessEvent(const SEntityEvent& event) override;
	virtual IEntityComponentPreviewer* GetPreviewer() { return &physical_entity_previewers_; }

	void physicalize() noexcept;

	SCharacterDimensions character_dimension_{
			SDefaults::radius
		,	SDefaults::height
		,	SDefaults::height_offset
		,	SDefaults::ground_contact_epsilon
	};

	SCharacterDynamics character_dynamics_{
			SDefaults::inertia
		,	SDefaults::inertia_acceleration
		,	SDefaults::air_control
		,	SDefaults::air_resistance
		,	SDefaults::mass
	};

	SPhysicalEntityPreviewers physical_entity_previewers_{};
};