//
// Created by kj16609 on 10/1/23.
//

extern "C" {
#include "encode.h"
}

#include <gtest/gtest-assertion-result.h>
#include <gtest/gtest.h>

#include <random>

#include "blurhash-cxx.hpp"

template < int x_size, int y_size >
std::array< std::array< std::array< std::uint8_t, 3 >, y_size >, x_size > createData()
{
	std::array< std::array< std::array< std::uint8_t, 3 >, y_size >, x_size > rand_buffer;

	std::random_device r;
	std::mt19937 engine { r() };
	std::uniform_int_distribution< std::uint8_t > dist { 0, 255 };

	for ( int y = 0; y < y_size; ++y )
	{
		for ( int x = 0; x < x_size; ++x )
		{
			rand_buffer[ y ][ x ][ 0 ] = dist( engine );
			rand_buffer[ y ][ x ][ 1 ] = dist( engine );
			rand_buffer[ y ][ x ][ 2 ] = dist( engine );
		}
	}

	return rand_buffer;
}

//Tests that our encode matches
TEST( EncodeTest, Encode_2 )
{
	//TODO: Make a real example image and not just loading random shit into a 'rbg' array.
	constexpr int x_size { 128 };
	constexpr int y_size { 128 };
	auto rand_buffer { createData< x_size, y_size >() };

	constexpr int comp_size_x { 2 };
	constexpr int comp_size_y { 2 };
	const auto my_output {
		blurhash::encode< comp_size_x, comp_size_y >( x_size, y_size, rand_buffer[ 0 ][ 0 ].data(), 3 )
	};

	const auto their_output {
		blurHashForPixels( comp_size_x, comp_size_y, x_size, y_size, rand_buffer[ 0 ][ 0 ].data(), 3 * x_size )
	};

	ASSERT_EQ( strlen( their_output ), my_output.size() );

	ASSERT_EQ( std::string_view( their_output, strlen( their_output ) ), my_output );

	for ( int i = 0; i < my_output.size(); ++i )
	{
		ASSERT_EQ( my_output[ i ], their_output[ i ] );
	}
}

//Tests that our encode matches
TEST( EncodeTest, Encode_2_512 )
{
	//TODO: Make a real example image and not just loading random shit into a 'rbg' array.
	constexpr int x_size { 512 };
	constexpr int y_size { 512 };
	auto rand_buffer { createData< x_size, y_size >() };

	constexpr int comp_size_x { 2 };
	constexpr int comp_size_y { 2 };
	const auto my_output {
		blurhash::encode< comp_size_x, comp_size_y >( x_size, y_size, rand_buffer[ 0 ][ 0 ].data(), 3 )
	};

	const auto their_output {
		blurHashForPixels( comp_size_x, comp_size_y, x_size, y_size, rand_buffer[ 0 ][ 0 ].data(), 3 * x_size )
	};

	ASSERT_EQ( strlen( their_output ), my_output.size() );

	ASSERT_EQ( std::string_view( their_output, strlen( their_output ) ), my_output );

	for ( int i = 0; i < my_output.size(); ++i )
	{
		ASSERT_EQ( my_output[ i ], their_output[ i ] );
	}
}

//Tests that our encode matches
TEST( EncodeTest, Encode_3 )
{
	//TODO: Make a real example image and not just loading random shit into a 'rbg' array.
	constexpr int x_size { 128 };
	constexpr int y_size { 128 };
	auto rand_buffer { createData< x_size, y_size >() };

	constexpr int comp_size_x { 3 };
	constexpr int comp_size_y { 3 };
	const auto my_output {
		blurhash::encode< comp_size_x, comp_size_y >( x_size, y_size, rand_buffer[ 0 ][ 0 ].data(), 3 )
	};

	const auto their_output {
		blurHashForPixels( comp_size_x, comp_size_y, x_size, y_size, rand_buffer[ 0 ][ 0 ].data(), 3 * x_size )
	};

	ASSERT_EQ( strlen( their_output ), my_output.size() );

	ASSERT_EQ( std::string_view( their_output, strlen( their_output ) ), my_output );

	for ( int i = 0; i < my_output.size(); ++i )
	{
		ASSERT_EQ( my_output[ i ], their_output[ i ] );
	}
}

//Tests that our encode matches
TEST( EncodeTest, Encode_8 )
{
	//TODO: Make a real example image and not just loading random shit into a 'rbg' array.
	constexpr int x_size { 128 };
	constexpr int y_size { 128 };
	auto rand_buffer { createData< x_size, y_size >() };

	constexpr int comp_size_x { 8 };
	constexpr int comp_size_y { 8 };
	const auto my_output {
		blurhash::encode< comp_size_x, comp_size_y >( x_size, y_size, rand_buffer[ 0 ][ 0 ].data(), 3 )
	};

	const auto their_output {
		blurHashForPixels( comp_size_x, comp_size_y, x_size, y_size, rand_buffer[ 0 ][ 0 ].data(), 3 * x_size )
	};

	ASSERT_EQ( strlen( their_output ), my_output.size() );

	ASSERT_EQ( std::string_view( their_output, strlen( their_output ) ), my_output );

	for ( int i = 0; i < my_output.size(); ++i )
	{
		ASSERT_EQ( my_output[ i ], their_output[ i ] );
	}
}
