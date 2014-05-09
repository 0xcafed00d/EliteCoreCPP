#pragma once

/* six byte random number used as seed for planets */
struct PlanetSeed_t
{ 
	inline PlanetSeed_t (unsigned short w0_, unsigned short w1_, unsigned short w2_) : w0(w0_), w1(w1_), w2(w2_) {}
	inline PlanetSeed_t () {}

	unsigned short w0, w1, w2;
}; 

/* rotate 8 bit number leftwards */
inline unsigned short rotatel(unsigned short x) 
{ 
	unsigned short temp = x & 128;
	return (2*(x&127))+(temp>>7);
} 

inline unsigned short twist(unsigned short x)
{ 
	return (unsigned short)((256*rotatel(x>>8))+rotatel(x&255));
}

/* Apply to base seed; once for galaxy 2  */
/* twice for galaxy 3, etc. */
/* Eighth application gives galaxy 1 again*/

inline PlanetSeed_t nextGalaxy(const PlanetSeed_t& s) 
{ 
	return PlanetSeed_t (twist(s.w0), twist(s.w1), twist(s.w2));
}


inline PlanetSeed_t tweakseed(const PlanetSeed_t& s)
{ 
	unsigned short temp = s.w0 + s.w1 + s.w2; 
	return PlanetSeed_t (s.w1, s.w2, temp);
}
