//
// Created by kj16609 on 10/1/23.
//
#include <benchmark/benchmark.h>

#include "blur-encode.h"
#include "fast-decode.hpp"

#define TEST_TEMPLATE( num )                                                                                           \
	case num:                                                                                                          \
		{                                                                                                              \
			hash = blurhash::encode< num, num >( size, size, rand_buffer[ 0 ][ 0 ], 3 );                               \
			break;                                                                                                     \
		}

static void BM_Decode_Current_CPP( benchmark::State& state )
{
	const auto size { state.range( 1 ) };
	const auto comp_size { state.range( 0 ) };

	uint8_t rand_buffer[ size ][ size ][ 3 ];

	for ( int y = 0; y < size; ++y )
	{
		for ( int x = 0; x < size; ++x )
		{
			rand_buffer[ y ][ x ][ 0 ] = static_cast< std::uint8_t >( rand() );
			rand_buffer[ y ][ x ][ 1 ] = static_cast< std::uint8_t >( rand() );
			rand_buffer[ y ][ x ][ 2 ] = static_cast< std::uint8_t >( rand() );
		}
	}

	std::string hash;
	switch ( comp_size )
	{
		TEST_TEMPLATE( 2 )
		TEST_TEMPLATE( 3 )
		TEST_TEMPLATE( 4 )
		TEST_TEMPLATE( 5 )
		TEST_TEMPLATE( 6 )
		TEST_TEMPLATE( 7 )
		TEST_TEMPLATE( 8 )
	}

	std::vector< std::uint8_t > out;

	for ( auto _ : state )
	{
		out = blurhash::testing::decode( hash, size, size, 0 );
	}
}

BENCHMARK( BM_Decode_Current_CPP )->ArgsProduct( { { 2, 4, 6, 8 }, { 128, 512, 1024 } } )->Unit( benchmark::kMillisecond );
