#pragma once

class EliteRNG
{
	public:
		/* four byte random number used for planet description */
		struct seed_t
		{
			unsigned char a,b,c,d;
		};  

	private:
		seed_t m_rnd_seed;

	public:
		EliteRNG(const seed_t& seed);
		void SetSeed (const seed_t& seed);
		int GenRnd ();
};
