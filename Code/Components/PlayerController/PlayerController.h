#include <CryEntitySystem/IEntityComponent.h>

namespace Cry::DefaultComponents {
	class CInputComponent;
	class CCharacterControllerComponent;
}

class CPlayerController final : public IEntityComponent {
public:
	static void ReflectType(Schematyc::CTypeDesc<CPlayerController>& desc);

	CPlayerController();

private:
	void Initialize() override;

	Cry::DefaultComponents::CInputComponent* input_component_{};
	Cry::DefaultComponents::CCharacterControllerComponent* character_controller_component_{};
};