#ifndef _FIXED_H_
#define _FIXED_H_

#define FRACTIONAL_BITS 10
#define HALF_FRACTIONAL_BITS 5

/// Fixed point format
typedef int fixed;
typedef long lfixed;
typedef unsigned int ufixed;
typedef unsigned long ulfixed;


inline unsigned int ufixed2uint(ufixed input);
inline ufixed uint2ufixed(unsigned int input);
inline float ufixed2float(ufixed input);
inline ufixed float2ufixed(float input);
inline ufixed ufixed_mul(ufixed mer, ufixed mcand);
inline ufixed ufixed_div(ufixed num, ufixed den);


inline int fixed2int(fixed input);
inline fixed int2fixed(int input);
inline float fixed2float(fixed input);
inline fixed float2fixed(float input);
inline fixed fixed_mul(fixed mer, fixed mcand);
inline fixed fixed_div(fixed num, fixed den);


inline unsigned int ulfixed2uint(ulfixed input);
inline unsigned long ulfixed2ulong(ulfixed input);
inline ulfixed uint2ulfixed(unsigned int input);
inline ulfixed ulong2ulfixed(unsigned long input);
inline float ulfixed2float(ulfixed input);
inline ulfixed float2ulfixed(float input);
inline ulfixed ulfixed_mul(ulfixed mer, ulfixed mcand);
inline ulfixed ulfixed_div(ulfixed num, ulfixed den);

inline int lfixed2int(lfixed input);
inline long lfixed2long(lfixed input);
inline lfixed int2lfixed(int input);
inline lfixed long2lfixed(long input);
inline float lfixed2float(lfixed input);
inline lfixed float2lfixed(float input);
inline lfixed lfixed_mul(lfixed mer, lfixed mcand);
inline lfixed lfixed_div(lfixed num, lfixed den);


#endif
