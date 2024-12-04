#pragma once

#if !NOSTDLIB

#include "all.h"

#if defined(USE_SIMD)
#include <immintrin.h>
#endif

#if defined(USE_AVX512)
typedef __m512i vepi16;
typedef __m512i vepi32;

vepi16  vec_zero_epi16();
vepi32  vec_zero_epi32();
vepi16  vec_set1_epi16(const int16_t n);
vepi16  vec_loadu_epi(const vepi16* src);
vepi16  vec_max_epi16(const vepi16 vec0, const vepi16 vec1);
vepi16  vec_min_epi16(const vepi16 vec0, const vepi16 vec1);
vepi16  vec_mullo_epi16(const vepi16 vec0, const vepi16 vec1);
vepi32  vec_madd_epi16(const vepi16 vec0, const vepi16 vec1);
vepi32  vec_add_epi32(const vepi32 vec0, const vepi32 vec1);
int32_t vec_reduce_add_epi32(const vepi32 vec);

#elif defined(USE_AVX2)
typedef __m256i vepi16;
typedef __m256i vepi32;

vepi16  vec_zero_epi16();
vepi32  vec_zero_epi32();
vepi16  vec_set1_epi16(const int16_t n);
vepi16  vec_loadu_epi(const vepi16* src);
vepi16  vec_max_epi16(const vepi16 vec0, const vepi16 vec1);
vepi16  vec_min_epi16(const vepi16 vec0, const vepi16 vec1);
vepi16  vec_mullo_epi16(const vepi16 vec0, const vepi16 vec1);
vepi32  vec_madd_epi16(const vepi16 vec0, const vepi16 vec1);
vepi32  vec_add_epi32(const vepi32 vec0, const vepi32 vec1);
int32_t vec_reduce_add_epi32(const vepi32 vec);
#endif
#endif
