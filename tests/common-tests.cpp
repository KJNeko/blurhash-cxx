//
// Created by kj16609 on 10/23/23.
//

extern "C" {
#include "encode.h"
}

#include <gtest/gtest-assertion-result.h>
#include <gtest/gtest.h>

#include <random>

#include "blur-common.h"

/*
TEST( CommonTests, RBGConversion )
{
	for ( int a = 0; a < 512; ++a )
	{
		float test[ 4 ];
		//Fill with random data
		std::random_device r;
		std::mt19937 engine { r() };
		std::uniform_real_distribution< float > dist { 0, 0.05 };

		for ( int i = 0; i < 4; ++i ) test[ i ] = dist( engine );

		int normal_out[ 4 ];

		//Original
		for ( int i = 0; i < 4; ++i ) normal_out[ i ] = linearTosRGB( test[ i ] );

		//SIMD
		//Pack
		__m128 packed = _mm_load_ps( test );
		__m128i simd_out = wideLinearTosRGB( packed );

		//Unpack simd_out
		int simd_out_arr[ 4 ];
		_mm_storeu_si128( reinterpret_cast< __m128i* >( simd_out_arr ), simd_out );

		ASSERT_EQ( simd_out_arr[ 0 ], normal_out[ 0 ] );
		ASSERT_EQ( simd_out_arr[ 1 ], normal_out[ 1 ] );
		ASSERT_EQ( simd_out_arr[ 2 ], normal_out[ 2 ] );
		ASSERT_EQ( simd_out_arr[ 3 ], normal_out[ 3 ] );
	}
}*/