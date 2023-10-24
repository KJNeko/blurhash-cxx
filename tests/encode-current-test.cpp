//
// Created by kj16609 on 10/23/23.
//

extern "C" {
#include "decode.h"
#include "encode.h"
}

#include <gtest/gtest-assertion-result.h>
#include <gtest/gtest.h>

#include <random>

#include "blur-encode.h"
#include "fast-decode.hpp"

template < int comp_size_x, int comp_size_y >
const std::string createHash()
{
	//Encode
	//TODO: Make a real example image and not just loading random shit into a 'rbg' array.
	constexpr int x_size { 512 };
	constexpr int y_size { 512 };
	std::array< std::array< std::array< uint8_t, 3 >, y_size >, x_size > rand_buffer;

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

	const auto my_output {
		blurhash::encode< comp_size_x, comp_size_y >( x_size, y_size, rand_buffer[ 0 ][ 0 ].data(), 3 )
	};

	const auto their_output {
		blurHashForPixels( comp_size_x, comp_size_y, x_size, y_size, rand_buffer[ 0 ][ 0 ].data(), 3 * x_size )
	};

	return std::string( their_output, strlen( their_output ) );
}

TEST( CurrentDecodeTest, Decode_2 )
{
	const auto test_hash { createHash< 2, 2 >() };

	constexpr int image_size { 32 };

	const auto my_output { blurhash::testing::decode( test_hash, image_size, image_size, 0, 3 ) };

	const auto their_output { decode( test_hash.data(), image_size, image_size, 0, 3 ) };

	for ( int y = 0; y < image_size; ++y )
		for ( int x = 0; x < image_size; ++x )
		{
			const auto idx { ( y * image_size * 3 ) + x * 3 };
			const auto* my_pixel { my_output.data() };
			const auto* their_pixel { their_output + idx };

			ASSERT_LE( std::abs( *my_pixel - *their_pixel ), 2 );
			ASSERT_LE( std::abs( *( my_pixel + 1 ) - *( their_pixel + 1 ) ), 2 );
			ASSERT_LE( std::abs( *( my_pixel + 2 ) - *( their_pixel + 2 ) ), 2 );
		}
}

TEST( CurrentDecodeTest, Decode_2_512 )
{
	const auto test_hash { createHash< 2, 2 >() };

	constexpr int image_size { 512 };

	const auto my_output { blurhash::testing::decode( test_hash, image_size, image_size, 0, 3 ) };

	const auto their_output { decode( test_hash.data(), image_size, image_size, 0, 3 ) };

	for ( int y = 0; y < image_size; ++y )
		for ( int x = 0; x < image_size; ++x )
		{
			const auto idx { ( y * image_size * 3 ) + x * 3 };
			const auto* my_pixel { my_output.data() };
			const auto* their_pixel { their_output + idx };

			ASSERT_LE( std::abs( *my_pixel - *their_pixel ), 2 );
			ASSERT_LE( std::abs( *( my_pixel + 1 ) - *( their_pixel + 1 ) ), 2 );
			ASSERT_LE( std::abs( *( my_pixel + 2 ) - *( their_pixel + 2 ) ), 2 );
		}
}

TEST( CurrentDecodeTest, Decode_3 )
{
	const auto test_hash { createHash< 3, 3 >() };

	constexpr int image_size { 512 };

	const auto my_output { blurhash::testing::decode( test_hash, image_size, image_size, 0, 3 ) };

	const auto their_output { decode( test_hash.data(), image_size, image_size, 0, 3 ) };

	for ( int y = 0; y < image_size; ++y )
		for ( int x = 0; x < image_size; ++x )
		{
			const auto idx { ( y * image_size * 3 ) + x * 3 };
			const auto* my_pixel { my_output.data() };
			const auto* their_pixel { their_output + idx };

			ASSERT_LE( std::abs( *my_pixel - *their_pixel ), 2 );
			ASSERT_LE( std::abs( *( my_pixel + 1 ) - *( their_pixel + 1 ) ), 2 );
			ASSERT_LE( std::abs( *( my_pixel + 2 ) - *( their_pixel + 2 ) ), 2 );
		}
}

TEST( CurrentDecodeTest, Decode_8 )
{
	const auto test_hash { createHash< 8, 8 >() };

	constexpr int image_size { 512 };

	const auto my_output { blurhash::testing::decode( test_hash, image_size, image_size, 0, 3 ) };

	const auto their_output { decode( test_hash.data(), image_size, image_size, 0, 3 ) };

	for ( int y = 0; y < image_size; ++y )
		for ( int x = 0; x < image_size; ++x )
		{
			const auto idx { ( y * image_size * 3 ) + x * 3 };
			const auto* my_pixel { my_output.data() };
			const auto* their_pixel { their_output + idx };

			ASSERT_EQ( *my_pixel, *their_pixel );
			ASSERT_EQ( *( my_pixel + 1 ), *( their_pixel + 1 ) );
			ASSERT_EQ( *( my_pixel + 2 ), *( their_pixel + 2 ) );
		}
}

TEST( CurrentDecodeTest, Decode_Odd )
{
	const auto test_hash { createHash< 8, 8 >() };

	constexpr int image_size { 130 };

	const auto my_output { blurhash::testing::decode( test_hash, image_size, image_size, 0, 3 ) };

	const auto their_output { decode( test_hash.data(), image_size, image_size, 0, 3 ) };

	for ( int y = 0; y < image_size; ++y )
		for ( int x = 0; x < image_size; ++x )
		{
			const auto idx { ( y * image_size * 3 ) + x * 3 };
			const auto* my_pixel { my_output.data() };
			const auto* their_pixel { their_output + idx };


//			ASSERT_EQ( *my_pixel, *their_pixel );
//			ASSERT_EQ( *( my_pixel + 1 ), *( their_pixel + 1 ) );
//			ASSERT_EQ( *( my_pixel + 2 ), *( their_pixel + 2 ) );

			ASSERT_LE( std::abs( *my_pixel - *their_pixel ), 6 );
			ASSERT_LE( std::abs( *( my_pixel + 1 ) - *( their_pixel + 1 ) ), 6 );
			ASSERT_LE( std::abs( *( my_pixel + 2 ) - *( their_pixel + 2 ) ), 6 );
		}
}


