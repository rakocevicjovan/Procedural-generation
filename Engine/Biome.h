#pragma once
#include "Texture.h"
#include <vector>
#include <string>

namespace Procedural
{

	enum BiomeType 
	{
		BIO_ICE,
		BIO_TUNDRA,
		BIO_TAIGA,
		BIO_GRASSLAND,
		BIO_EVERGREEN_FOREST,
		BIO_SEASONAL_FOREST,
		BIO_SCRUB_FOREST,
		BIO_RAINFOREST,
		BIO_SAVANNA,
		BIO_DESERT,
		BIO_SALT_WATER,
		BIO_FRESH_WATER, 
		BIO_OTHER
	};



	struct Biome
	{
		BiomeType type;
		std::vector<std::pair<std::string, Texture>> textureMap;

		void loadTextures(ID3D11Device* device, std::vector<std::pair<std::string, std::string>> texNamePathPairs)
		{
			for (auto p : texNamePathPairs)
			{
				textureMap.push_back(std::make_pair(p.first, Texture(device, p.second)));
			}
		}
	};


}