#include "pmx_loader.h"
#pragma once

namespace lve::dataloader {
	// not tested
	std::string ReadString(int size, char* in, Encoding encoding) {
		return "test\0";
		std::string in1 = in;
		if (encoding == Encoding::UTF8) {
			int nwLen = MultiByteToWideChar(CP_UTF8, 0, in1.c_str(), -1, NULL, 0);
			wchar_t* pwBuf = new wchar_t[nwLen + 1];
			memset(pwBuf, 0, nwLen * 2 + 2);
			MultiByteToWideChar(CP_UTF8, 0, in1.c_str(), in1.length(), pwBuf, nwLen);
			int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
			char* pBuf = new char[nLen + 1];
			memset(pBuf, 0, nLen + 1);
			WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
			std::string retStr = pBuf;
			delete[]pBuf;
			delete[]pwBuf;
			pBuf = NULL;
			pwBuf = NULL;
			return retStr;
		}
		else if (encoding == Encoding::Unicode) {
			//TODO
		}
	}

	float getfloat_from_chars(char* in, int idx) {
		char tmp[4];
		float ans;
		for (int i = 0; i < 4; i++)
			tmp[i] = in[i + idx];
		memcpy(&ans, tmp, sizeof(float));
		return ans;
	}

	glm::vec3 ReadVector3XInv(std::ifstream& fin) {
		char in[12];
		fin.read(in, 3 * sizeof(float));
		glm::vec3 ans;
		ans.x = -getfloat_from_chars(in, 0);
		ans.y = getfloat_from_chars(in, sizeof(float));
		ans.z = getfloat_from_chars(in, 2 * sizeof(float));
		return ans;
	}

	glm::vec2 ReadVector2(std::ifstream& fin) {
		char in[8];
		fin.read(in, 2 * sizeof(float));
		glm::vec2 ans;
		ans.x = getfloat_from_chars(in, 0);
		ans.y = getfloat_from_chars(in, sizeof(float));
		return ans;
	}

	glm::vec3 ReadVector3(std::ifstream& fin) {
		char in[12];
		fin.read(in, 3 * sizeof(float));
		glm::vec3 ans;
		ans.x = getfloat_from_chars(in, 0);
		ans.y = getfloat_from_chars(in, sizeof(float));
		ans.z = getfloat_from_chars(in, 2 * sizeof(float));
		return ans;
	}

	glm::vec4 ReadVector4(std::ifstream& fin) {
		char in[16];
		fin.read(in, 4 * sizeof(float));
		glm::vec4 ans;
		ans.r = getfloat_from_chars(in, 0);
		ans.g = getfloat_from_chars(in, sizeof(float));
		ans.b = getfloat_from_chars(in, 2 * sizeof(float));
		ans.a = getfloat_from_chars(in, 3 * sizeof(float));
		return ans;
	}

	int ReadIndex(std::ifstream& fin, int size) {
		if (size == 1) {
			char in[1];
			int8_t ans;
			fin.read(in, sizeof(int8_t));
			memcpy(&ans, in, sizeof(int8_t));
			return ans;
		}
		else if (size == 2) {
			char in[2];
			int16_t ans;
			fin.read(in, sizeof(int16_t));
			memcpy(&ans, in, sizeof(int16_t));
			return ans;
		}
		char in[4];
		int32_t ans;
		fin.read(in, sizeof(int32_t));
		memcpy(&ans, in, sizeof(int32_t));
		return ans;
	}

	float ReadFloat(std::ifstream& fin) {
		char in[4];
		float ans;
		fin.read(in, sizeof(float));
		memcpy(&ans, in, sizeof(float));
		return ans;
	}

	int ReadInt(std::ifstream& fin) {
		char in[4];
		int ans;
		fin.read(in, sizeof(int));
		memcpy(&ans, in, sizeof(int));
		return ans;
	}

	int ReadUIndex(std::ifstream& fin, int size) {
		if (size == 1) {
			char in[1];
			int8_t ans;
			fin.read(in, sizeof(int8_t));
			memcpy(&ans, in, sizeof(int8_t));
			return ans;
		}
		else if (size == 2) {
			char in[2];
			int16_t ans;
			fin.read(in, sizeof(int16_t));
			memcpy(&ans, in, sizeof(int16_t));
			return ans;
		}
		char in[4];
		int32_t ans;
		fin.read(in, sizeof(int32_t));
		memcpy(&ans, in, sizeof(int32_t));
		return ans;
	}

	Byte ReadByte(std::ifstream& fin) {
		char in[1];
		Byte ans;
		fin.read(in, sizeof(Byte));
		memcpy(&ans, in, sizeof(Byte));
		return ans;
	}

	int8_t Readint8_t(std::ifstream& fin) {
		char in[1];
		int8_t ans;
		fin.read(in, sizeof(int8_t));
		memcpy(&ans, in, sizeof(int8_t));
		return ans;
	}


	PMX_only_obj::PMX_only_obj(std::string modelPath) {
		std::ifstream fin;
		fin.open(modelPath, std::ios::in | std::ios::binary);
		if(!fin)throw std::runtime_error("File open failed.");
		Reload(fin);
		fin.close();
	}

	void PMX_only_obj::Reload(std::ifstream& fin) {
		Textures.reset();
		Materials.reset();

		int fileHeader=ReadInt(fin);
		if (fileHeader != 0x20584D50)throw std::runtime_error("File is not Pmx format.");//' XMP'
		float version = ReadFloat(fin);

		int8_t flagsSize = Readint8_t(fin);//useless

		char boolbuf[4];
		bool Utf8Encoding;
		fin.read(boolbuf, sizeof(bool));
		memcpy(&Utf8Encoding, boolbuf, sizeof(bool));
		bool isUtf8Encoding = (Utf8Encoding != 0);
		int extraUVNumber=Readint8_t(fin);
		int vertexIndexSize= Readint8_t(fin);
		int textureIndexSize = Readint8_t(fin);
		int materialIndexSize = Readint8_t(fin);
		int boneIndexSize = Readint8_t(fin);
		int morphIndexSize = Readint8_t(fin);
		int rigidBodyIndexSize = Readint8_t(fin);

		Encoding encoding = isUtf8Encoding ? Encoding::UTF8 : Encoding::Unicode;

		//load basic info
		int size;
		char strbuff[256];

		size = ReadInt(fin);
		fin.read((char*)strbuff, size);
		Name = ReadString(size, strbuff, encoding);

		size = ReadInt(fin);
		fin.read((char*)strbuff, size);
		NameEN = ReadString(size, strbuff, encoding);

		size = ReadInt(fin);
		fin.read((char*)strbuff, size);
		Description = ReadString(size, strbuff, encoding);

		size = ReadInt(fin);
		fin.read((char*)strbuff, size);
		DescriptionEN = ReadString(size, strbuff, encoding);

		//load vertices info
		int countOfVertex=ReadInt(fin);
		std::cout << "mmd model vertices num: " << countOfVertex << std::endl;
		for (int i = 0; i < countOfVertex; i++) {

			PMX_Vertex vertex{};

			//coord norm uv
			vertex.Coordinate = ReadVector3XInv(fin);
			//std::cout << "xyz: " << vertex->Coordinate.x<< vertex->Coordinate.y<< vertex->Coordinate.z << std::endl;
			vertex.Normal = ReadVector3XInv(fin);
			//std::cout << "normal: " << vertex->Normal.x<< vertex->Normal.y<<vertex->Normal.z << std::endl;
			vertex.UvCoordinate = ReadVector2(fin);
			//std::cout << "uv: " << vertex->UvCoordinate.x<< vertex->UvCoordinate.y << std::endl;
			//ex uv
			//std::cout << "ex uv num: " << extraUVNumber << std::endl;
			if (extraUVNumber > 0){
				for (int j = 0; j < extraUVNumber; j++)
				{
					vertex.ExtraUvCoordinate.push_back(ReadVector4(fin));
				}
			}
			//skinning
			int skinningType = Readint8_t(fin); 
			//std::cout << "skining type: " << skinningType << std::endl;
			if (skinningType == (int)PMX_BoneWeightDeformType::BDEF1)
			{
				vertex.boneId0 = ReadIndex(fin, boneIndexSize);
				vertex.boneId1 = -1;
				vertex.boneId2 = -1;
				vertex.boneId3 = -1;
				vertex.Weights.x = 1;
			}
			else if (skinningType == (int)PMX_BoneWeightDeformType::BDEF2)
			{
				vertex.boneId0 = ReadIndex(fin, boneIndexSize);
				vertex.boneId1 = ReadIndex(fin, boneIndexSize);
				vertex.boneId2 = -1;
				vertex.boneId3 = -1;
				vertex.Weights.x = ReadFloat(fin);
				vertex.Weights.y = 1.0f - vertex.Weights.x;
			}
			else if (skinningType == (int)PMX_BoneWeightDeformType::BDEF4)
			{
				vertex.boneId0 = ReadIndex(fin, boneIndexSize);
				vertex.boneId1 = ReadIndex(fin, boneIndexSize);
				vertex.boneId2 = ReadIndex(fin, boneIndexSize);
				vertex.boneId3 = ReadIndex(fin, boneIndexSize);
				vertex.Weights = ReadVector4(fin);
			}
			else if (skinningType == (int)PMX_BoneWeightDeformType::SDEF)
			{

				vertex.boneId0 = ReadIndex(fin, boneIndexSize);
				vertex.boneId1 = ReadIndex(fin, boneIndexSize);
				vertex.boneId2 = -1;
				vertex.boneId3 = -1;
				vertex.Weights.x = ReadFloat(fin);
				vertex.Weights.y = 1.0f - vertex.Weights.x;
				ReadVector3(fin);
				ReadVector3(fin);
				ReadVector3(fin);
			}
			else
			{
				//empty
			}
			vertex.EdgeScale = ReadFloat(fin);

			Vertices.push_back(vertex);
			//std::cout << "edge scale: " << vertex->EdgeScale << std::endl;
		}
		//--end of per vertex loading

		//--begin of triangle loading
		
		int countOfTriangleIndex = ReadInt(fin);
		std::cout << "mmd model triangles num: " << countOfTriangleIndex << std::endl;
		for (int i = 0; i < 3000; i++)
		{
			TriangleIndexs.push_back(ReadUIndex(fin, vertexIndexSize));
		}

		
		//--end of triangle loading

		//--begin of texture loading
		//--end of texture loading

		//--begin of material loading
		//--end of material loading

	}


}
