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
			&CPlayerController::min_walk_speed_
		,	'misp'
		,	"minwalkspeed"
		,	"Min Walk Speed"
		,	"Min Speed when the Player is Walking"
		,	min_walk_speed
	);

	desc.AddMember(
			&CPlayerController::max_walk_speed_
		,	'masp'
		,	"maxwalkspeed"
		,	"Max Walk Speed"
		,	"Max Speed when the Player is Walking"
		,	max_walk_speed
	);

	desc.AddMember(
			&CPlayerController::max_jump_speed_
		,	'majp'
		,	"maxjumpspeed"
		,	"Max Jump Speed"
		,	"Max Speed when the Player Jump"
		,	max_jump_speed
	);

	desc.AddMember(
			&CPlayerController::capsule_stand_height_
		,	'cshe'
		,	"capsulestandheight"
		,	"Capsule Stand Height"
		,	"Height of the Capsule when the Player is Stand"
		,	capsule_stand_height
	);

	desc.AddMember(
			&CPlayerController::capsule_crouch_height_
		,	'cche'
		,	"capsulecrouchheight"
		,	"Capsule Crouch Height"
		,	"Height of the Capsule when the Player is Crouch"
		,	capsule_crouch_height
	);

	desc.AddMember(
			&CPlayerController::capsule_height_offset_
		,	'chof'
		,	"capsuleheightoffset"
		,	"Capsule Height Offset"
		,	"Offset of the Capule Height from Ground"
		,	capsule_height_offset
	);

	desc.AddMember(
			&CPlayerController::camera_stand_height_
		,	'cash'
		,	"camerastandheight"
		,	"Camera Stand Height"
		,	"Camera Height when the Player is Stand"
		,	camera_stand_height
	);

	desc.AddMember(
			&CPlayerController::camera_crouch_height_
		,	'cach'
		,	"cameracrouchheight"
		,	"Camera Crouch Height"
		,	"Camera Height when the Player is Crouch"
		,	camera_crouch_height
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
				states_flags_.set(move_forward);
		});

	input_component_->RegisterAction("player", "moveback", [this](int activation_mode, float value)
		{
			if (activation_mode == eAAM_OnHold)
				states_flags_.set(move_back);
		});

	input_component_->RegisterAction("player", "moveright", [this](int activation_mode, float value)
		{
			if (activation_mode == eAAM_OnHold)
				states_flags_.set(move_right);
		});

	input_component_->RegisterAction("player", "moveleft", [this](int activation_mode, float value)
		{
			if (activation_mode == eAAM_OnHold)
				states_flags_.set(move_left);
		});

	input_component_->RegisterAction("player", "boost", [this](int activation_mode, float value)
		{
			if (activation_mode == eAAM_OnHold)
				states_flags_.set(boost);
		});

	input_component_->RegisterAction("player", "jump", [this](int activation_mode, float value)
		{
			if (activation_mode == eAAM_OnPress)
				states_flags_.set(jump);
		});

	input_component_->RegisterAction("player", "crouch", [this](int activation_mode, float value)
		{
			if (activation_mode == eAAM_OnHold)
				states_flags_.set(crouch);
		});

	input_component_->BindAction("player", "rotate_yaw", eAID_KeyboardMouse, eKI_MouseX);
	input_component_->BindAction("player", "rotate_pitch", eAID_KeyboardMouse, eKI_MouseY);
	input_component_->BindAction("player", "moveforward", eAID_KeyboardMouse, eKI_W);
	input_component_->BindAction("player", "moveback", eAID_KeyboardMouse, eKI_S);
	input_component_->BindAction("player", "moveright", eAID_KeyboardMouse, eKI_D);
	input_component_->BindAction("player", "moveleft", eAID_KeyboardMouse, eKI_A);
	input_component_->BindAction("player", "boost", eAID_KeyboardMouse, eKI_LShift);
	input_component_->BindAction("player", "jump", eAID_KeyboardMouse, eKI_Space);
	input_component_->BindAction("player", "crouch", eAID_KeyboardMouse, eKI_LCtrl);

	character_controller_component_ = m_pEntity->GetOrCreateComponent<CCharacterControllerComponent>();
	character_controller_component_->GetPhysicsParameters().m_height = capsule_stand_height;

	const auto character_radius = character_controller_component_->GetPhysicsParameters().m_radius * 0.50f;
	const auto character_height = character_controller_component_->GetPhysicsParameters().m_height * 0.25f;
	const auto character_offset = character_radius + character_height + capsule_height_offset;
	const auto character_transform = Matrix34::CreateTranslationMat({ 0.0f, 0.0f, character_offset });

	character_controller_component_->SetTransformMatrix(character_transform);
	character_controller_component_->Physicalize();

	camera_component_ = m_pEntity->GetOrCreateComponent<CCameraComponent>();
	camera_component_->SetTransformMatrix(Matrix34::CreateTranslationMat(Vec3{ 0.0f, 0.0f, camera_stand_height_ }));
}

Cry::Entity::EventFlags CPlayerController::GetEventMask() const
{
	return Cry::Entity::EEvent::Update;
}

void CPlayerController::ProcessEvent(const SEntityEvent& event)
{
	if (gEnv->IsEditing())
		return;
	
	const auto frame_time = event.fParam[0];

	auto velocity = Vec3{ ZERO };
	
	if (states_flags_.test(move_forward))
		velocity.y += 1.0f;
	if (states_flags_.test(move_back))
		velocity.y -= 1.0f;
	if (states_flags_.test(move_right))
		velocity.x += 1.0f;
	if (states_flags_.test(move_left))
		velocity.x -= 1.0f;

	velocity.Normalize();

	const auto desired_acceleration = states_flags_.test(boost) ? max_walk_speed_ : min_walk_speed_;

	walk_acceleration_ = Lerp(walk_acceleration_, desired_acceleration, 7.55f * frame_time);

	const auto camera_rotation = Quat{ camera_component_->GetTransformMatrix() };
	const auto yaw_rotation = Quat::CreateRotationZ(camera_rotation.GetRotZ());
	character_controller_component_->SetVelocity(yaw_rotation * velocity * walk_acceleration_);

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

	const auto current_stance = states_flags_.test(crouch) ? EPlayer_Stance::Crouching : EPlayer_Stance::Standing;

	if (desired_stance_ != current_stance) {
		desired_stance_ = current_stance;

		auto dimensions = pe_player_dimensions{};
		m_pEntity->GetPhysics()->GetParams(&dimensions);

		auto character_height = 0.0f;

		switch(desired_stance_)
		{
		case EPlayer_Stance::Standing:
		{
			character_height = capsule_stand_height;
		}
		break;

		case EPlayer_Stance::Crouching:
		{
			character_height = capsule_crouch_height;
		}
		break;
		}

		dimensions.sizeCollider.z = character_height * 0.25f;
		dimensions.heightCollider = dimensions.sizeCollider.x + dimensions.sizeCollider.z + capsule_height_offset_;
		m_pEntity->GetPhysicalEntity()->SetParams(&dimensions);
	}

	const auto eyes_height = states_flags_.test(crouch) ? camera_crouch_height_ : camera_stand_height_;

	auto camera_transform = camera_component_->GetTransformMatrix();
	auto camera_position = camera_transform.GetColumn3();
	camera_position.z = Lerp(camera_position.z, eyes_height, 10.0f * frame_time);

	camera_transform.SetTranslation(camera_position);
	camera_component_->SetTransformMatrix(camera_transform);

	if (states_flags_.test(jump) && character_controller_component_->IsOnGround())
		character_controller_component_->AddVelocity({ 0.0f, 0.0f, max_jump_speed_ });

	mouse_location_delta_.zero();
	states_flags_.reset();
}