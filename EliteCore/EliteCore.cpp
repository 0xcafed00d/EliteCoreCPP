#include "EliteCore.h"

#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

#include "EliteRNG.h"
#include "ElitePlanetSeed.h"
#include "EliteGoatSoup.h"

// ====================================================================================================
//   Static Data
// ====================================================================================================

static char* AllDigrams = "abouseitiletstonlonuthno..lexegezacebisousesarmaindirea.eratenberalavetiedorquanteisrion";	/* Dots should be nullprint characters */
static char* PlanetNameDigrams = AllDigrams + 24;

static size_t NUM_GALAXIES      = 8;
static size_t NUM_GALAXIES_MASK = 0x07;
static size_t NUM_SYSTEMS_PER_GALAXY = 256;

static const char* govnames[] = 
{
    "Anarchy",
    "Feudal",
    "Multi-gov",
    "Dictatorship",
    "Communist",
    "Confederacy",
    "Democracy",
    "Corporate State"
};
    
static const char* econnames[] = 
{
    "Rich Industrial",
    "Average Industrial",
    "Poor Industrial",
    "Mainly Industrial",
    "Mainly Agricultural",
    "Rich Agricultural",
    "Average Agricultural",
    "Poor Agricultural"
};

struct tradegood
{                         
	unsigned int baseprice;
	unsigned int gradient;  
	unsigned int basequant;  
	unsigned int maskbyte;   
	EliteCore::MarketUnitType units;      
	const char* name;  
};

static tradegood commodities[] =
{
	{0x13, -0x02, 0x06, 0x01, EliteCore::UNIT_T,  "Food"},
	{0x14, -0x01, 0x0A, 0x03, EliteCore::UNIT_T,  "Textiles"},
	{0x41, -0x03, 0x02, 0x07, EliteCore::UNIT_T,  "Radioactives"},
	{0x28, -0x05, 0xE2, 0x1F, EliteCore::UNIT_T,  "Slaves"},
	{0x53, -0x05, 0xFB, 0x0F, EliteCore::UNIT_T,  "Liquor/Wines"},
	{0xC4, +0x08, 0x36, 0x03, EliteCore::UNIT_T,  "Luxuries"},
	{0xEB, +0x1D, 0x08, 0x78, EliteCore::UNIT_T,  "Narcotics"},
	{0x9A, +0x0E, 0x38, 0x03, EliteCore::UNIT_T,  "Computers"},
	{0x75, +0x06, 0x28, 0x07, EliteCore::UNIT_T,  "Machinery"},
	{0x4E, +0x01, 0x11, 0x1F, EliteCore::UNIT_T,  "Alloys"},
	{0x7C, +0x0d, 0x1D, 0x07, EliteCore::UNIT_T,  "Firearms"},
	{0xB0, -0x09, 0xDC, 0x3F, EliteCore::UNIT_T,  "Furs"},
	{0x20, -0x01, 0x35, 0x03, EliteCore::UNIT_T,  "Minerals"},
	{0x61, -0x01, 0x42, 0x07, EliteCore::UNIT_KG, "Gold"},
	{0xAB, -0x02, 0x37, 0x1F, EliteCore::UNIT_KG, "Platinum"},
	{0x2D, -0x01, 0xFA, 0x0F, EliteCore::UNIT_G,  "Gem-Stones"},
	{0x35, +0x0F, 0xC0, 0x07, EliteCore::UNIT_T,  "Alien Items"},
	{0, 0, 0, 0,  EliteCore::UNIT_T, NULL},
};




// ====================================================================================================
//   Private Implementation
// ====================================================================================================

struct EliteCore::Impl
{
    size_t m_galaxyNumber;
	std::vector<EliteCore::SystemInfo> m_galaxy;
	std::map<std::string, size_t> m_systemNameMap;

	EliteCore::Impl::Impl(size_t galaxyNumber);
	EliteCore::Impl::~Impl();

	EliteCore::SystemInfo MakeSystem (PlanetSeed_t& seed);

	float Distance (unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
};



EliteCore::Impl::Impl(size_t galaxyNumber) : m_galaxyNumber (galaxyNumber & NUM_GALAXIES_MASK)
{
    PlanetSeed_t initialSeed (0x5A4A, 0x0248, 0xB753);

	// advance to correct galaxy
	for (size_t n = 0; n < m_galaxyNumber; n++)
		initialSeed = nextGalaxy(initialSeed);

	for (size_t system = 0; system < NUM_SYSTEMS_PER_GALAXY; system++)
	{
		EliteCore::SystemInfo systemInfo = MakeSystem(initialSeed);
		m_galaxy.push_back(systemInfo);
		m_systemNameMap[systemInfo.name] = system;
	}
}

EliteCore::Impl::~Impl()
{
}

EliteCore::SystemInfo EliteCore::Impl::MakeSystem (PlanetSeed_t& s)
{
	EliteCore::SystemInfo thissys;
	
	unsigned int longnameflag = (s.w0) & 64;

	thissys.x = s.w1 >> 8;
	thissys.y = s.w0 >> 8;

	thissys.govtype = (s.w1 >> 3) & 7; /* bits 3,4 &5 of w1 */
	thissys.economy = (s.w0 >> 8) & 7; /* bits 8,9 &A of w0 */

	if (thissys.govtype <= 1)
		thissys.economy = thissys.economy | 2;

	thissys.techlev = ((s.w1 >> 8) & 3) + (thissys.economy ^ 7);
	thissys.techlev += thissys.govtype >> 1;

	if (((thissys.govtype)&1)==1)	
		thissys.techlev+=1;
	/* C simulation of 6502's LSR then ADC */

	thissys.population = 4*(thissys.techlev) + (thissys.economy);
	thissys.population +=  (thissys.govtype) + 1;

	thissys.productivity = (((thissys.economy)^7)+3)*((thissys.govtype)+4);
	thissys.productivity *= (thissys.population)*8;

	thissys.radius = 256*(((s.w2 >> 8) & 15) + 11) + thissys.x;  

    thissys.inhabitantdesc = generateInhabitantDescription(s);

	EliteRNG::seed_t goatSoupSeed;
	goatSoupSeed.a = s.w1 & 0xFF;;
	goatSoupSeed.b = s.w1 >> 8;
	goatSoupSeed.c = s.w2 & 0xFF;
	goatSoupSeed.d = s.w2 >> 8;

	unsigned int pair1 = 2 * ((s.w2 >> 8) & 31);  s = tweakseed(s);
	unsigned int pair2 = 2 * ((s.w2 >> 8) & 31);  s = tweakseed(s);
	unsigned int pair3 = 2 * ((s.w2 >> 8) & 31);  s = tweakseed(s);
	unsigned int pair4 = 2 * ((s.w2 >> 8) & 31);  s = tweakseed(s);
	/* Always four iterations of random number */

	thissys.name += PlanetNameDigrams[pair1];
	thissys.name += PlanetNameDigrams[pair1+1];
	thissys.name += PlanetNameDigrams[pair2];
	thissys.name += PlanetNameDigrams[pair2+1];
	thissys.name += PlanetNameDigrams[pair3];
	thissys.name += PlanetNameDigrams[pair3+1];

	if (longnameflag) /* bit 6 of ORIGINAL w0 flags a four-pair name */
	{
		thissys.name += PlanetNameDigrams[pair4];
		thissys.name += PlanetNameDigrams[pair4+1];
	}

	thissys.name.erase(std::remove(thissys.name.begin(), thissys.name.end(), '.'), thissys.name.end());
	thissys.name[0] = toupper(thissys.name[0]);

	thissys.description = generateGoatSoupString(goatSoupSeed, thissys.name, AllDigrams);

	return thissys;
}

float EliteCore::Impl::Distance( unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2 )
{
	return sqrtf (float((x1-x2) * (x1-x2) + (y1-y2) * (y1-y2) / 4)) * 4 / 10;  
}


// ====================================================================================================
//   Public interface
// ====================================================================================================

EliteCore::EliteCore (size_t galaxyNum) : m_impl (new Impl(galaxyNum))
{
}

EliteCore::~EliteCore()
{
	delete m_impl;
}

void EliteCore::GenerateGalaxy (size_t galaxyNum)
{
    delete m_impl;
    m_impl = new Impl(galaxyNum);
}

size_t EliteCore::GetGalaxyNumber() const
{
    return m_impl->m_galaxyNumber;
}

size_t EliteCore::GetNumSystems() const
{
    return m_impl->m_galaxy.size();
}

const EliteCore::SystemInfo& EliteCore::GetSystemInfo(size_t systemIndex)
{
    return m_impl->m_galaxy[systemIndex];
}

std::string EliteCore::GetEconomyName (unsigned int economy) const
{
    if (economy >= 8)
        return "<Invalid Economy>";

    return econnames[economy];
}

std::string EliteCore::GetGovernmentName (unsigned int govtype) const
{
    if (govtype >= 8)
        return "<Invalid Government>";

    return govnames[govtype];
}

/* 
Prices and availabilities are influenced by the planet's economy type
(0-7) and a random "fluctuation" byte that was kept within the saved
commander position to keep the market prices constant over gamesaves.
Availabilities must be saved with the game since the player alters them
by buying (and selling(?))

Almost all operations are one byte only and overflow "errors" are
extremely frequent and exploited.

Trade Item prices are held internally in a single byte=true value/4.
The decimal point in prices is introduced only when printing them.
Internally, all prices are integers.  
*/

void EliteCore::GenerateMarket( const SystemInfo& system, unsigned int fluctuation, std::vector<MarketItem>& marketInfo ) const
{
	marketInfo.clear();

	for (int n = 0; commodities[n].name != NULL; n++)
	{
		MarketItem item;
		item.itemID = n;
		item.name = commodities[n].name;
		item.unit = commodities[n].units;

		signed int q; 

		signed int product = (system.economy)*(commodities[n].gradient);
		signed int changing = fluctuation & (commodities[n].maskbyte);

		q =  ((commodities[n].basequant) + changing - product) & 0xff;

		if(q&0x80) 
			q=0;                       /* Clip to positive 8-bit */

		item.quantity = (unsigned int)(q & 0x3F); /* Mask to 6 bits */

		q =  (commodities[n].baseprice) + changing + product;
		q = q & 0xFF;
		item.price = (unsigned int) (q*4);

		marketInfo.push_back(item);
	}
}

size_t EliteCore::FindSystemByName (const std::string& name)
{
	std::map<std::string, size_t>::iterator i = m_impl->m_systemNameMap.find(name);
	if (i == m_impl->m_systemNameMap.end())
		return NOT_FOUND;

	return i->second;
}

float EliteCore::DistanceBetweenSystems (size_t system1, size_t system2)
{
	const EliteCore::SystemInfo& s1 = m_impl->m_galaxy[system1];
	const EliteCore::SystemInfo& s2 = m_impl->m_galaxy[system2];

	return m_impl->Distance(s1.x, s1.y, s2.x, s2.y); 
}

void EliteCore::FindSystemsWithin (size_t systemFrom, float radius_LY, std::vector<size_t>& result )
{
	result.clear();

	for (size_t n = 0; n < m_impl->m_galaxy.size(); n++)
	{
		if (n != systemFrom) // dont include system from in results
		{
			if (DistanceBetweenSystems(systemFrom, n) <= radius_LY)
				result.push_back(n);
		}
	}
}

size_t EliteCore::GetClosestSystem (unsigned int x, unsigned int y)
{
	float minDist = 1000000;
	size_t closestSys = NOT_FOUND;

	for (size_t n = 0; n < m_impl->m_galaxy.size(); n++)
	{
		const EliteCore::SystemInfo& s = m_impl->m_galaxy[n];
		float dist = m_impl->Distance(x, y, s.x, s.y);
		if (dist < minDist)
		{
			minDist = dist;
			closestSys = n;
		}
	}
	return closestSys;
}


