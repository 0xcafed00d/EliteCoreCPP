#include "EliteRNG.h"

EliteRNG::EliteRNG( const seed_t& seed ) : m_rnd_seed (seed)
{

}

void EliteRNG::SetSeed( const seed_t& seed )
{
	m_rnd_seed = seed;
}

int EliteRNG::GenRnd()
{
	int a,x;
	x = (m_rnd_seed.a * 2) & 0xFF;
	a = x + m_rnd_seed.c;
	if (m_rnd_seed.a > 127)	a++;
	m_rnd_seed.a = a & 0xFF;
	m_rnd_seed.c = x;

	a = a / 256;	/* a = any carry left from above */
	x = m_rnd_seed.b;
	a = (a + x + m_rnd_seed.d) & 0xFF;
	m_rnd_seed.b = a;
	m_rnd_seed.d = x;
	return a;
}

