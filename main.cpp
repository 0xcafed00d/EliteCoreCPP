

#include <iostream>
#include <iomanip>


#include "EliteCore.h"



void main ()
{
	EliteCore core(0);

	for (size_t g = 0; g < 8; g++)
	{
		core.GenerateGalaxy(g);

		float d = core.DistanceBetweenSystems(7, 147);

		std::vector<size_t> results;
		core.FindSystemsWithin(7, 7, results);

		for (size_t n = 0; n < results.size(); n++)
		{
			EliteCore::SystemInfo s = core.GetSystemInfo(results[n]);
		}

		EliteCore::SystemInfo s = core.GetSystemInfo(core.GetClosestSystem (21, 180));


		std::cout << "********** GALAXY: " << g << " *********************" << std::endl << std::endl;

		for (size_t n = 0; n < core.GetNumSystems(); n++)
		{
			const EliteCore::SystemInfo& system = core.GetSystemInfo(n);

			std::cout << g << "," << n << " : " << system.name << "  (" << system.x << "," << system.y << ")" << std::endl;
			std::cout << "Economy:    " << core.GetEconomyName(system.economy) << "  (" << system.economy << ")" << std::endl;
			std::cout << "Government: " << core.GetGovernmentName(system.govtype) << "  (" << system.govtype << ")" << std::endl;
			std::cout << "Tech Level: " << system.techlev + 1<< std::endl;
			std::cout << "Turnover:   " << system.productivity << " MCr" << std::endl;
			std::cout << "Radius:     " << system.radius << "km" << std::endl;
			std::cout << "Population: " << (system.population >> 3) << " Billion  (" << system.inhabitantdesc << ")" << std::endl;
			std::cout << system.description << std::endl;

			std::vector<EliteCore::MarketItem> market; 
			core.GenerateMarket (system, 0, market);

			std::cout << "  ===================================" << std::endl;
			for (size_t n = 0; n < market.size(); n++)
			{
				std::cout << "  |" << std::setw(15) << market[n].name << " " << std::setw(6) << market[n].quantity << " " << std::setw(6) << market[n].price / 10 << "." << market[n].price % 10 << "  |" << std::endl; 

			}
			std::cout << "  ===================================" << std::endl;

			std::cout << std::endl;
		}
	}
}