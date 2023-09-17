#include <CryEntitySystem/IEntityComponent.h>

#include <bitset>

namespace Cry::DefaultComponents {
	class CInputComponent;
	class CCharacterControllerComponent;
	class CCameraComponent;
}

class CPlayerController final : public IEntityComponent {
	static constexpr auto min_walk_speed = 2.5f;
	static constexpr auto max_walk_speed = 4.5f;
	static constexpr auto jump_speed = 5.7f;
	static constexpr auto rotation_pitch_min = -75.0f;
	static constexpr auto rotation_pitch_max = 80.0f;

	enum {
			idle = 0
		,	move_forward
		,	move_back
		,	move_right
		,	move_left
		,	boost
		,	jump
		,	crouch
		,	interact
		,	total_states
	};

public:
	static void ReflectType(Schematyc::CTypeDesc<CPlayerController>& desc);
	
private:
	void Initialize() override;
	Cry::Entity::EventFlags GetEventMask() const override;
	void ProcessEvent(const SEntityEvent& event) override;

	float min_walk_speed_{ min_walk_speed };
	float max_walk_speed_{ max_walk_speed };
	float jump_speed_{ jump_speed };
	float rotation_pitch_min_{ rotation_pitch_min };
	float rotation_pitch_max_{ rotation_pitch_max };

	Cry::DefaultComponents::CInputComponent* input_component_{};
	Cry::DefaultComponents::CCharacterControllerComponent* character_controller_component_{};
	Cry::DefaultComponents::CCameraComponent* camera_component_{};

	Vec2 mouse_location_delta_{ ZERO };

	std::bitset<total_states> state_flags_{};
};