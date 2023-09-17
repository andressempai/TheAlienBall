#include "StdAfx.h"
#include "PlayerController.h"

#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CryCore/StaticInstanceList.h>
#include <DefaultComponents/Input/InputComponent.h>
#include <DefaultComponents/Physics/CharacterControllerComponent.h>
#include <DefaultComponents/Cameras/CameraComponent.h>
#include <CryPhysics/physinterface.h>

void register_player_controller(Schematyc::IEnvRegistrar& registrar)
{
	auto scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerController));
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&register_player_controller);

void CPlayerController::ReflectType(Schematyc::CTypeDesc<CPlayerController>& desc)
{
	desc.SetGUID("{761DD699-0799-4FB8-BCDC-024ECC72919A}"_cry_guid);
	desc.AddMember(
			&CPlayerController::radius_
		,	'radi'
		,	"radius"
		,	"Radius"
		,	"Player Collider Radius."
		,	radius
	);

	desc.AddMember(
			&CPlayerController::stand_height_
		,	'shei'
		,	"standheight"
		,	"Stand Height"
		,	"Player Collider Height when is Stand."
		,	stand_height
	);

	desc.AddMember(
			&CPlayerController::crouch_height_
		,	'chei'
		,	"crouchheight"
		,	"Crouch Height"
		,	"Player Collider Height when is Crouch."
		,	crouch_height
	);

	desc.AddMember(
			&CPlayerController::capsule_height_offset_
		,	'cphe'
		,	"capsuleheight"
		,	"Capsule Height"
		,	"Player Capsule Height."
		,	capsule_height_offset
	);

	desc.AddMember(
			&CPlayerController::eyes_height_stand_
		,	'ehis'
		,	"eyesheightstand"
		,	"Eyes Height Stand"
		,	"Player Eyes Height when is Stand."
		,	eyes_height_stand
	);

	desc.AddMember(
			&CPlayerController::eyes_height_crouch_
		,	'ehic'
		,	"eyesheightcrouch"
		,	"Eyes Height Crouch"
		,	"Player Eyes Height when is Crouch."
		,	eyes_height_crouch
	);

	desc.AddMember(
			&CPlayerController::min_walk_speed_
		,	'mwsp'
		,	"minwalkspeed"
		,	"Min Walk Speed"
		,	"Speed when player walking"
		,	min_walk_speed
	);

	desc.AddMember(
			&CPlayerController::max_walk_speed_
		,	'maws'
		,	"maxwalkspeed"
		,	"Max Walk Speed"
		,	"Speed when the player Sprint"
		,	max_walk_speed
	);

	desc.AddMember(
			&CPlayerController::jump_speed_
		,	'jspe'
		,	"jumpspeed"
		,	"Jump Speed"
		,	"Speed when the player Jump"
		,	jump_speed
	);

	desc.AddMember(
			&CPlayerController::rotation_pitch_min_
		,	'pmin'
		,	"pitchmin"
		,	"Rotation Pitch Min"
		,	nullptr
		,	rotation_pitch_min
	);

	desc.AddMember(
			&CPlayerController::rotation_pitch_max_
		,	'pmax'
		,	"pitchmax"
		,	"Rotation Pitch Max"
		,	nullptr
		,	rotation_pitch_max
	);
}

void CPlayerController::Initialize()
{
	using namespace Cry::DefaultComponents;

	input_component_ = m_pEntity->GetOrCreateComponent<CInputComponent>();
	input_component_->RegisterAction("player", "rotate_yaw", [this](int activation_mode, float value)
		{
			mouse_location_delta_.x -= value;
		});

	input_component_->RegisterAction("player", "rotate_pitch", [this](int activation_mode, float value)
		{
			mouse_location_delta_.y -= value;
		});

	input_component_->RegisterAction("player", "moveforward", [this](int activation_mode, float value)
		{
			if (activation_mode == eAAM_OnHold)
				state_flags_.set(move_forward);
		});

	input_component_->RegisterAction("player", "moveback", [this](int activation_mode, float value)
		{
			if (activation_mode == eAAM_OnHold)
				state_flags_.set(move_back);
		});

	input_component_->RegisterAction("player", "moveright", [this](int activation_mode, float value)
		{
			if (activation_mode == eAAM_OnHold)
				state_flags_.set(move_right);
		});

	input_component_->RegisterAction("player", "moveleft", [this](int activation_mode, float value)
		{
			if (activation_mode == eAAM_OnHold)
				state_flags_.set(move_left);
		});

	input_component_->RegisterAction("player", "boost", [this](int activation_mode, float value)
		{
			if (activation_mode == eAAM_OnHold)
				state_flags_.set(boost);
		});

	input_component_->RegisterAction("player", "moveup", [this](int activation_mode, float value)
		{
			if (activation_mode == eAAM_OnPress)
				state_flags_.set(jump);
		});

	input_component_->RegisterAction("player", "movedown", [this](int activation_mode, float value)
		{
			if (activation_mode == eAAM_OnPress)
				state_flags_.set(crouch);
		});

	input_component_->BindAction("player", "rotate_yaw", eAID_KeyboardMouse, eKI_MouseX);
	input_component_->BindAction("player", "rotate_pitch", eAID_KeyboardMouse, eKI_MouseY);
	input_component_->BindAction("player", "moveforward", eAID_KeyboardMouse, eKI_W);
	input_component_->BindAction("player", "moveback", eAID_KeyboardMouse, eKI_S);
	input_component_->BindAction("player", "moveright", eAID_KeyboardMouse, eKI_D);
	input_component_->BindAction("player", "moveleft", eAID_KeyboardMouse, eKI_A);
	input_component_->BindAction("player", "boost", eAID_KeyboardMouse, eKI_LShift);
	input_component_->BindAction("player", "moveup", eAID_KeyboardMouse, eKI_Space);
	input_component_->BindAction("player", "movedown", eAID_KeyboardMouse, eKI_LCtrl);

	character_controller_component_ = m_pEntity->GetOrCreateComponent<CCharacterControllerComponent>();
	character_controller_component_->SetTransformMatrix(Matrix34::CreateTranslationMat(Vec3{ 0.0f, 0.0f, 0.5f }));
	character_controller_component_->Physicalize();

	camera_component_ = m_pEntity->GetOrCreateComponent<CCameraComponent>();
	camera_component_->SetTransformMatrix(Matrix34::CreateTranslationMat(Vec3{ 0.0f, 0.0f, 0.735f }));
}

Cry::Entity::EventFlags CPlayerController::GetEventMask() const
{
	return Cry::Entity::EEvent::Update;
}

void CPlayerController::ProcessEvent(const SEntityEvent& event)
{
	if (gEnv->IsEditing())
		return;

	auto move_velocity = Vec3{ ZERO };

	if (state_flags_.test(move_forward))
		move_velocity.y += 1.0f;
	if (state_flags_.test(move_back))
		move_velocity.y -= 1.0f;
	if (state_flags_.test(move_right))
		move_velocity.x += 1.0f;
	if (state_flags_.test(move_left))
		move_velocity.x -= 1.0f;

	move_velocity.Normalize();

	if (state_flags_.test(boost))
		move_velocity = move_velocity.scale(max_walk_speed_);

	const auto camera_rotation = Quat{ camera_component_->GetTransformMatrix() };
	const auto movement_vector = move_velocity * min_walk_speed_;
	character_controller_component_->SetVelocity(Quat::CreateRotationZ(camera_rotation.GetRotZ()) * movement_vector);

	if (state_flags_.test(jump)) {
		if (character_controller_component_->IsOnGround())
			character_controller_component_->AddVelocity({ 0.0f, 0.0f, jump_speed_ });
	}

	if (!mouse_location_delta_.IsZero()) {
		auto camera_transform = camera_component_->GetTransformMatrix();

		static constexpr auto rotation_speed = 0.002f;
		const auto rotation_limit_pitch_min = DEG2RAD(rotation_pitch_min_);
		const auto rotation_limit_pitch_max = DEG2RAD(rotation_pitch_max_);

		auto camera_ypr = CCamera::CreateAnglesYPR(Matrix33{ camera_transform });
		camera_ypr.x += mouse_location_delta_.x * rotation_speed;

		camera_ypr.y = crymath::clamp(camera_ypr.y + mouse_location_delta_.y * rotation_speed
			,	rotation_limit_pitch_min
			,	rotation_limit_pitch_max
		);

		camera_ypr.z = 0.0f;

		camera_transform.SetRotation33(CCamera::CreateOrientationYPR(camera_ypr));

		camera_component_->SetTransformMatrix(camera_transform);
	}

	if (state_flags_.test(crouch)) {
		is_crouch_ = !is_crouch_;

		if (auto* const physcial_entity = m_pEntity->GetPhysicalEntity()) {
			const auto player_radius = radius_ / 2.0f;
			const auto player_height = [this]() -> f32
			{
				if (is_crouch_)
					return crouch_height_ * 0.25f;

				const auto rad = radius_ * 0.5f;
				const auto hei = stand_height_ * 0.5f;

				primitives::capsule capsule{};
				capsule.center = m_pEntity->GetWorldPos() + Vec3(0.0f, 0.0f, rad + hei + capsule_height_offset_);
				capsule.axis = Vec3(0.0f, 0.0f, 1.0f);
				capsule.r = rad;
				capsule.hh = hei;

				std::array ignored_entities{ m_pEntity->GetPhysicalEntity() };

				IPhysicalWorld::SPWIParams physic_intersection_params{};
				physic_intersection_params.itype = capsule.type;
				physic_intersection_params.pprim = &capsule;
				physic_intersection_params.pSkipEnts = ignored_entities.data();
				physic_intersection_params.nSkipEnts = ignored_entities.size();

				if (gEnv->pPhysicalWorld->PrimitiveWorldIntersection(physic_intersection_params) > 0.0f) {
					is_crouch_ = true;

					return crouch_height_ * 0.25f;
				}

				return stand_height_ * 0.25f;
			}();

			pe_player_dimensions player_dimensions{};
			physcial_entity->GetParams(&player_dimensions);

			player_dimensions.sizeCollider = Vec3(player_radius, 0.0f, player_height);
			player_dimensions.heightCollider = player_radius + player_height + capsule_height_offset_;

			physcial_entity->SetParams(&player_dimensions);
		}
	}

	state_flags_.reset();
	mouse_location_delta_.zero();
}