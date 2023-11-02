#pragma once
#include "_iglib_base.h"

namespace ig
{
	namespace wavefront
	{
		// may lack some standards
		struct Obj
		{
			Obj(const std::string &src);



			struct Vertex
			{
				struct
				{
					float x, y, z, w;
				} position;
				
				struct
				{
					float u, v, p;
				} texcoord;

				struct
				{
					float x, y, z;
				} normal;
			};

			struct Face
			{
				struct VertexIndex
				{
					uint32_t pos, texcoord, normal;
				} indcies[ 3 ];
			};

			struct Line
			{
				std::vector<uint32_t> indcies;
			};


			std::vector<Vertex> vertcies;
			std::vector<Face> faces;
			std::vector<Line> lines;
		};
	}
}
