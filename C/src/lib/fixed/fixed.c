#include "fixed.h"

inline unsigned int ufixed2uint(ufixed input)
{
    return (input >> FRACTIONAL_BITS);
}
inline ufixed uint2ufixed(unsigned int input)
{
    return (input << FRACTIONAL_BITS);
}
inline float ufixed2float(ufixed input)
{
    return ((float)input / (float)(1u << FRACTIONAL_BITS));
}
inline ufixed float2ufixed(float input)
{
    return ((ufixed)(input * (1u << FRACTIONAL_BITS)));
}
inline ufixed ufixed_mul(ufixed mer, ufixed mcand)
{
    return ((mer >> 1) * (mcand >> (FRACTIONAL_BITS-1)));
}
inline ufixed ufixed_div(ufixed num, ufixed den)
{
    return ((ufixed)( ( ((unsigned long)num) << FRACTIONAL_BITS ) / den ));
}


inline int fixed2int(fixed input)
{
    //return input / (1 << FRACTIONAL_BITS);
    return (input >> FRACTIONAL_BITS);
}
inline fixed int2fixed(int input)
{
    //return (input * (1 << FRACTIONAL_BITS));
    return (input << FRACTIONAL_BITS);
}
inline float fixed2float(fixed input)
{
    return ((float)input / (float)(1 << FRACTIONAL_BITS));
}
inline fixed float2fixed(float input)
{
    return ((fixed)(input * (1 << FRACTIONAL_BITS)));
}
inline fixed fixed_mul(fixed mer, fixed mcand)
{
    //return ( (mer * mcand) >> FRACTIONAL_BITS );
    //return ( (((long)mer) * mcand) >> FRACTIONAL_BITS );
    return (mer >> HALF_FRACTIONAL_BITS) * (mcand >> HALF_FRACTIONAL_BITS);
    //return (mer >> 1)  * (mcand >> (FRACTIONAL_BITS-1));
    //return lfixed_mul(mer, mcand);
}
inline fixed fixed_div(fixed num, fixed den)
{
    return ( ( ((long)num) << FRACTIONAL_BITS ) / den );
    //return ((fixed)( ( (long)num * (1l << FRACTIONAL_BITS) ) / (long)den ));
    //return ((fixed)( ( ((long)num) << FRACTIONAL_BITS ) / (long)den ));
}


inline unsigned int ulfixed2uint(ulfixed input)
{
    return ((unsigned int)(input >> FRACTIONAL_BITS));
}
inline unsigned long ulfixed2ulong(ulfixed input)
{
    return (input >> FRACTIONAL_BITS);
}
inline ulfixed uint2ulfixed(unsigned int input)
{
    return ((ulfixed)(input << FRACTIONAL_BITS));
}
inline ulfixed ulong2ulfixed(unsigned long input)
{
    return (input << FRACTIONAL_BITS);
}
inline float ulfixed2float(ulfixed input)
{
    return ((float)input / (float)(1 << FRACTIONAL_BITS));
}
inline ulfixed float2ulfixed(float input)
{
    return ((ulfixed)(input * (1 << FRACTIONAL_BITS)));
}
inline ulfixed ulfixed_mul(ulfixed mer, ulfixed mcand)
{
    return ( (mer * mcand) >> FRACTIONAL_BITS );
}
inline ulfixed ulfixed_div(ulfixed num, ulfixed den)
{
    return ( (num << FRACTIONAL_BITS) / den );
}


inline int lfixed2int(lfixed input)
{
    return ((int)(input >> FRACTIONAL_BITS));
}
inline long lfixed2long(lfixed input)
{
    return (input >> FRACTIONAL_BITS);
}
inline lfixed int2lfixed(int input)
{
    return ((lfixed)(input << FRACTIONAL_BITS));
}
inline lfixed long2lfixed(long input)
{
    return (input << FRACTIONAL_BITS);
}
inline float lfixed2float(lfixed input)
{
    return ((float)input / (float)(1 << FRACTIONAL_BITS));
}
inline lfixed float2lfixed(float input)
{
    return ((lfixed)(input * (1 << FRACTIONAL_BITS)));
}
inline lfixed lfixed_mul(lfixed mer, lfixed mcand)
{
    return ( (mer * mcand) >> FRACTIONAL_BITS );
}
inline lfixed lfixed_div(lfixed num, lfixed den)
{
    //return (lfixed)( ( num * (1l << FRACTIONAL_BITS) ) / den );
    return ( ( num << FRACTIONAL_BITS ) / den );
}

