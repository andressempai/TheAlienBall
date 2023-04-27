#include <CryEntitySystem/IEntityComponent.h>

#include <bitset>

namespace Cry::DefaultComponents {
	class CInputComponent;
	class CCharacterControllerComponent;
	class CCameraComponent;
}

class CPlayerController final : public IEntityComponent {
	static constexpr auto min_walk_speed = 2.35f;
	static constexpr auto max_walk_speed = 4.35f;
	static constexpr auto max_jump_speed = 5.35f;
	static constexpr auto capsule_stand_height = 1.77f;
	static constexpr auto capsule_crouch_height = 0.83f;
	static constexpr auto capsule_height_offset = 0.02f;
	static constexpr auto camera_stand_height = 1.57f;
	static constexpr auto camera_crouch_height = 0.63f;
	static constexpr auto rotation_pitch_min = -75.0f;
	static constexpr auto rotation_pitch_max = 80.0f;

	enum {
			move_forward = 0
		,	move_back
		,	move_right
		,	move_left
		,	boost
		,	jump
		,	crouch
		,	total_states
	};

	enum struct EPlayer_Stance {
			Standing
		,	Crouching
	};

public:
	static void ReflectType(Schematyc::CTypeDesc<CPlayerController>& desc);
	
private:
	void Initialize() override;
	Cry::Entity::EventFlags GetEventMask() const override;
	void ProcessEvent(const SEntityEvent& event) override;

	float min_walk_speed_{ min_walk_speed };
	float max_walk_speed_{ max_walk_speed };
	float max_jump_speed_{ max_jump_speed };
	float capsule_stand_height_{ capsule_stand_height };
	float capsule_crouch_height_{ capsule_crouch_height };
	float capsule_height_offset_{ capsule_height_offset };
	float camera_stand_height_{ camera_stand_height };
	float camera_crouch_height_{ camera_crouch_height };
	float rotation_pitch_min_{ rotation_pitch_min };
	float rotation_pitch_max_{ rotation_pitch_max };

	Cry::DefaultComponents::CInputComponent* input_component_{};
	Cry::DefaultComponents::CCharacterControllerComponent* character_controller_component_{};
	Cry::DefaultComponents::CCameraComponent* camera_component_{};

	Vec2 mouse_location_delta_{ ZERO };
	std::bitset<total_states> states_flags_{};

	float walk_acceleration_{ min_walk_speed_ };
	EPlayer_Stance desired_stance_{ EPlayer_Stance::Standing };
};