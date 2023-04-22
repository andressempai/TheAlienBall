#include <CryEntitySystem/IEntityComponent.h>

namespace Cry::DefaultComponents {
	class CInputComponent;
	class CCharacterControllerComponent;
	class CCameraComponent;
}

class CPlayerController final : public IEntityComponent {
	static constexpr auto rotation_pitch_min = -75.0f;
	static constexpr auto rotation_pitch_max = 80.0f;

public:
	static void ReflectType(Schematyc::CTypeDesc<CPlayerController>& desc);
	
private:
	void Initialize() override;
	Cry::Entity::EventFlags GetEventMask() const override;
	void ProcessEvent(const SEntityEvent& event) override;

	float rotation_pitch_min_{ rotation_pitch_min };
	float rotation_pitch_max_{ rotation_pitch_max };

	Cry::DefaultComponents::CInputComponent* input_component_{};
	Cry::DefaultComponents::CCharacterControllerComponent* character_controller_component_{};
	Cry::DefaultComponents::CCameraComponent* camera_component_{};

	Vec2 mouse_location_delta_{ ZERO };
};