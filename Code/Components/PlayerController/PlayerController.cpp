#include "StdAfx.h"
#include "PlayerController.h"

#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CryCore/StaticInstanceList.h>
#include <DefaultComponents/Input/InputComponent.h>
#include <DefaultComponents/Physics/CharacterControllerComponent.h>
#include <DefaultComponents/Cameras/CameraComponent.h>

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

	input_component_->BindAction("player", "rotate_yaw", eAID_KeyboardMouse, eKI_MouseX);
	input_component_->BindAction("player", "rotate_pitch", eAID_KeyboardMouse, eKI_MouseY);

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

	mouse_location_delta_.zero();
}