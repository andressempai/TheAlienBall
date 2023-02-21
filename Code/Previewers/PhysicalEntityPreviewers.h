#pragma once
#include <CryEntitySystem/IEntityComponent.h>

struct SPhysicalEntityPreviewers final : IEntityComponentPreviewer {
	virtual void SerializeProperties(Serialization::IArchive& archive) override {}
	virtual void Render(
			const IEntity& entity
		,	const IEntityComponent& component
		,	SEntityPreviewContext& context) const override;
};