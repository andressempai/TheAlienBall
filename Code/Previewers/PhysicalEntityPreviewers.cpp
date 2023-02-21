#include "PhysicalEntityPreviewers.h"

#include <CryPhysics/physinterface.h>

void SPhysicalEntityPreviewers::Render(
			const IEntity& entity
		,	const IEntityComponent& component
		,	SEntityPreviewContext& context) const
{
	if (auto* const physical_entity = entity.GetPhysicalEntity()) {
		auto part_params = pe_params_part{};
		part_params.ipart = 0;

		physical_entity->GetParams(&part_params);

		const auto world_transform = entity.GetWorldTM();

		auto world_geometry_data = geom_world_data{};
		world_geometry_data.offset = part_params.pos + world_transform.GetTranslation();
		world_geometry_data.R = Matrix33{ part_params.q } * Matrix33{ world_transform };
		world_geometry_data.scale = part_params.scale * world_transform.GetUniformScale();

		gEnv->pSystem->GetIPhysRenderer()->DrawGeometry(
				part_params.pPhysGeom->pGeom
			,	&world_geometry_data
			,	-1
			,	0
			,	ZERO
			,	context.debugDrawInfo.color
		);
	}
}