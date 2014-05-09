#pragma once

#include <string>
#include "EliteRNG.h"
#include "ElitePlanetSeed.h"

std::string generateGoatSoupString (EliteRNG::seed_t seed, const std::string& planetName, const char* digrams);

std::string generateInhabitantDescription (const PlanetSeed_t& seed);