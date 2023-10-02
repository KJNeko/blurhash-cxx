//
// Created by kj16609 on 10/1/23.
//

extern "C" {
#include "encode.h"
}

#include <gtest/gtest-assertion-result.h>
#include <gtest/gtest.h>

#include "blurhash-cxx.hpp"

//Tests that our encode matches
TEST( Encode_2, EncodeRefTest )
{
	//TODO: Make a real example image and not just loading random shit into a 'rbg' array.
	constexpr int x_size { 128 };
	constexpr int y_size { 128 };
	std::array< std::array< std::array< uint8_t, 3 >, y_size >, x_size > rand_buffer;
	for ( int y = 0; y < y_size; ++y )
	{
		for ( int x = 0; x < x_size; ++x )
		{
			rand_buffer[ y ][ x ][ 0 ] = static_cast< std::uint8_t >( rand() & 0xffffff );
			rand_buffer[ y ][ x ][ 1 ] = static_cast< std::uint8_t >( rand() & 0xffffff );
			rand_buffer[ y ][ x ][ 2 ] = static_cast< std::uint8_t >( rand() & 0xffffff );
		}
	}

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
TEST( Encode_2_512, EncodeRefTest )
{
	//TODO: Make a real example image and not just loading random shit into a 'rbg' array.
	constexpr int x_size { 512 };
	constexpr int y_size { 512 };
	std::array< std::array< std::array< uint8_t, 3 >, y_size >, x_size > rand_buffer;
	for ( int y = 0; y < y_size; ++y )
	{
		for ( int x = 0; x < x_size; ++x )
		{
			rand_buffer[ y ][ x ][ 0 ] = static_cast< std::uint8_t >( rand() & 0xffffff );
			rand_buffer[ y ][ x ][ 1 ] = static_cast< std::uint8_t >( rand() & 0xffffff );
			rand_buffer[ y ][ x ][ 2 ] = static_cast< std::uint8_t >( rand() & 0xffffff );
		}
	}

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
TEST( Encode_3, EncodeRefTest )
{
	//TODO: Make a real example image and not just loading random shit into a 'rbg' array.
	constexpr int x_size { 128 };
	constexpr int y_size { 128 };
	std::array< std::array< std::array< uint8_t, 3 >, y_size >, x_size > rand_buffer;
	for ( int y = 0; y < y_size; ++y )
	{
		for ( int x = 0; x < x_size; ++x )
		{
			rand_buffer[ y ][ x ][ 0 ] = static_cast< std::uint8_t >( rand() & 0xffffff );
			rand_buffer[ y ][ x ][ 1 ] = static_cast< std::uint8_t >( rand() & 0xffffff );
			rand_buffer[ y ][ x ][ 2 ] = static_cast< std::uint8_t >( rand() & 0xffffff );
		}
	}

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
TEST( Encode_8, EncodeRefTest )
{
	//TODO: Make a real example image and not just loading random shit into a 'rbg' array.
	constexpr int x_size { 128 };
	constexpr int y_size { 128 };
	std::array< std::array< std::array< uint8_t, 3 >, y_size >, x_size > rand_buffer;
	for ( int y = 0; y < y_size; ++y )
	{
		for ( int x = 0; x < x_size; ++x )
		{
			rand_buffer[ y ][ x ][ 0 ] = static_cast< std::uint8_t >( rand() & 0xffffff );
			rand_buffer[ y ][ x ][ 1 ] = static_cast< std::uint8_t >( rand() & 0xffffff );
			rand_buffer[ y ][ x ][ 2 ] = static_cast< std::uint8_t >( rand() & 0xffffff );
		}
	}

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
