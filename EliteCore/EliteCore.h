#pragma once

#include <string>
#include <vector>

class EliteCore
{
	private:
		struct Impl;
		Impl* m_impl;

	public:
		struct SystemInfo
		{
			unsigned int x;
			unsigned int y;       
			unsigned int economy; 
			unsigned int govtype;   
			unsigned int techlev;	 
			unsigned int population;  
			unsigned int productivity; 
			unsigned int radius;
			std::string name;
			std::string description;
			std::string inhabitantdesc;
		};
		
		enum MarketUnitType
		{
			UNIT_T = 0,
			UNIT_KG = 1,
			UNIT_G = 2
		};

		struct MarketItem
		{
			unsigned int itemID;
			unsigned int price;
			unsigned int quantity;
			MarketUnitType unit;
			const char *name;
		};

	public:
		EliteCore(size_t galaxyNum);
		~EliteCore();

        void GenerateGalaxy (size_t galaxyNum);
        size_t GetGalaxyNumber () const;

        size_t GetNumSystems () const;
        const SystemInfo& GetSystemInfo (size_t systemIndex);

        std::string GetEconomyName (unsigned int economy) const;
        std::string GetGovernmentName (unsigned int govtype) const;

		void GenerateMarket (const EliteCore::SystemInfo& system, unsigned int fluctuation, std::vector<MarketItem>& marketInfo) const;

		static const size_t NOT_FOUND = 0xffffffff;

		size_t FindSystemByName (const std::string& name);
		size_t GetClosestSystem (unsigned int x, unsigned int y);
		float  DistanceBetweenSystems (size_t system1, size_t system2);
		void FindSystemsWithin (size_t systemFrom, float radius_LY, std::vector<size_t>& result);
};
