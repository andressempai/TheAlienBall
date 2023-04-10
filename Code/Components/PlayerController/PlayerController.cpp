#include "StdAfx.h"
#include "PlayerController.h"

#include <CrySchematyc/Env/Elements/EnvComponent.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CryCore/StaticInstanceList.h>
#include <DefaultComponents/Input/InputComponent.h>
#include <DefaultComponents/Physics/CharacterControllerComponent.h>

void register_player_controller(Schematyc::IEnvRegistrar& registrar)
{
	auto scope = registrar.Scope(IEntity::GetEntityScopeGUID());
	scope.Register(SCHEMATYC_MAKE_ENV_COMPONENT(CPlayerController));
}

CRY_STATIC_AUTO_REGISTER_FUNCTION(&register_player_controller);

void CPlayerController::ReflectType(Schematyc::CTypeDesc<CPlayerController>& desc)
{
	desc.SetGUID("{761DD699-0799-4FB8-BCDC-024ECC72919A}"_cry_guid);
}

CPlayerController::CPlayerController()
{
	
}

void CPlayerController::Initialize()
{
	using namespace Cry::DefaultComponents;

	input_component_ = m_pEntity->GetOrCreateComponent<CInputComponent>();
	character_controller_component_ = m_pEntity->GetOrCreateComponent<CCharacterControllerComponent>();
	character_controller_component_->SetTransformMatrix(Matrix34::CreateTranslationMat(Vec3{ 0.0f, 0.0f, 0.5f }));
	character_controller_component_->Physicalize();
}