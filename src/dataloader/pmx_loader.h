#pragma once

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <cassert>
#include <iostream>

#include <windows.h>

#define Byte unsigned char

// lib
#include <glm/glm.hpp>


namespace lve::dataloader {

	struct PMX_Vertex {
		glm::vec3 Coordinate;
        glm::vec3 Normal;
        glm::vec2 UvCoordinate;
		float EdgeScale;

		std::vector<glm::vec4> ExtraUvCoordinate;
		int boneId0;
		int boneId1;
		int boneId2;
		int boneId3;
		glm::vec4 Weights;
		std::string ToString() { 
			std::string ans;
            for (int i = 0; i < 3; i++) 
				ans += std::to_string(Coordinate[0]);
                return ans;
        }
	};

	struct PMX_Texture {
		std::string TexturePath;
		std::string ToString() {
			return TexturePath;
		}
	};

	enum PMX_DrawFlag {
		DrawDoubleFace = 1,
		DrawGroundShadow = 2,
		CastSelfShadow = 4,
		DrawSelfShadow = 8,
		DrawEdge = 16,
	};

	class PMX_Material {
	public:
		std::string Name;
		std::string NameEN;
		glm::vec4 DiffuseColor;
		glm::vec4 SpecularColor;
		glm::vec3 AmbientColor;
		PMX_DrawFlag DrawFlags;
		glm::vec4 EdgeColor;
		float EdgeScale;
		int TextureIndex;
		int SecondaryTextureIndex;
		Byte SecondaryTextureType;
		bool UseToon;
		int ToonIndex;
		std::string Meta;
		int TriangeIndexStartNum;
		int TriangeIndexNum;
		std::string ToString() { return Name; }
	};


	class PMX_only_obj {
	public:
		bool Ready;
		std::string Name;
		std::string NameEN;
		std::string Description;
		std::string DescriptionEN;

		std::vector<PMX_Vertex> Vertices;
		uint32_t* TriangleIndexs1;
		std::vector<uint32_t> TriangleIndexs;
		std::vector<PMX_Texture> Textures;
		std::vector<PMX_Material> Materials;

		std::vector<int> Triangle;
		
		PMX_only_obj(std::string modelPath);
		void Reload(std::ifstream& fin); 
	

	};

	enum Encoding {
		UTF8 = 0,
		Unicode = 1,
	};

	enum PMX_BoneWeightDeformType {
		BDEF1 = 0,
		BDEF2 = 1,
		BDEF4 = 2,
		SDEF = 3,
		QDEF = 4
	};

	

}