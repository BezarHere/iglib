#pragma once
#include "_iglib_base.h"
#include "_iglib_vertex.h"
#include "_iglib_texture.h"
#include "_iglib_transform.h"

namespace ig
{
	namespace ssm
	{

		struct ScalerSceneModelMetaData
		{
			std::string title;
			std::string desc;
			std::string author;
		};

		struct Shape2D
		{
			Transform2D transform;
			std::vector<Vertex2D> vertcies;
			std::shared_ptr<Texture> texture;
			PrimitiveType primitve;
		};

		struct ScalerSceneModel2D
		{
			ScalerSceneModel2D(const std::string &src);
			ScalerSceneModel2D(std::ifstream &file);

			ScalerSceneModelMetaData meta;
			std::vector<Shape2D> shapes;
		};
	}
}
