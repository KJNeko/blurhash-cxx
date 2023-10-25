#ifndef BLURHASH_DECODE_HPP
#define BLURHASH_DECODE_HPP

#include <assert.h>
#include <cstdint>
#include <immintrin.h>
#include <iostream>
#include <math.h>
#include <stdbool.h>
#include <stdexcept>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string_view>
#include <vector>

#include "blur-common.h"

namespace blurhash::testing
{
	namespace internal
	{

		constexpr inline uint8_t clampToUByte( const int val )
		{
			if ( val <= 0 )
				return 0;
			else if ( val >= 255 )
				return 255;
			else [[likely]]
				return static_cast< uint8_t >( val );
		}

		/*
		32 | 32 | 32 | 32
		-------------------
		24 + 8 | 24 + 8 | 24 + 8 | 24 + 8
		 */

		__m128i wideClampToUByte( const __m128i val )
		{
			//Clamp down each integer to a max of 255
			const __m128i max { _mm_set1_epi32( 255 ) };
			const __m128i min { _mm_set1_epi32( 0 ) };
			const __m128i clamped { _mm_max_epi32( _mm_min_epi32( val, max ), min ) };

			return clamped;
		}

		constexpr inline int decodeToInt( const std::string_view string, int start, int end )
		{
			constexpr int chars_size { chars.size() };
			int value { 0 };
			for ( int iter1 = start; iter1 < end; ++iter1 )
			{
				int index { -1 };
				const char character { string[ iter1 ] };

				for ( int i = 0; i < chars_size; ++i )
				{
					if ( character == chars[ i ] )
					{
						index = i;
						break;
					}
				}

				if ( index == -1 ) [[unlikely]]
					throw std::runtime_error( "Failed to decode value " + std::to_string( character ) + " to int" );
				value = value * chars_size + index;
			}
			return value;
		}

		constexpr inline bool isValidBlurhash( const std::string_view blurhash )
		{
			const int hash_length { static_cast< int >( blurhash.size() ) };

			if ( hash_length < 6 )
				throw std::
					runtime_error( "Hash was not larger then min size. Was: " + std::to_string( blurhash.size() ) );

			const int sizeFlag { decodeToInt( blurhash, 0, 1 ) }; // Get size from first character
			const int numY { static_cast< int >( sizeFlag / 9 ) + 1 };
			const int numX { ( sizeFlag % 9 ) + 1 };

			if ( hash_length != 4 + 2 * numX * numY ) return false;
			return true;
		}

		constexpr inline std::tuple< float, float, float > decodeDC( int value )
		{
			const float r { sRGBToLinear( value >> 16 ) }; // R-component
			const float g { sRGBToLinear( ( value >> 8 ) & 255 ) }; // G-Component
			const float b { sRGBToLinear( value & 255 ) }; // B-Component

			return { r, g, b };
		}

		constexpr inline std::tuple< float, float, float > decodeAC( int value, const float maximumValue )
		{
			const int quantR { static_cast< int >( floorf( static_cast< float >( value ) / ( 19.0f * 19.0f ) ) ) };
			const int quantG { static_cast< int >( floorf( static_cast< float >( value ) / 19.0f ) ) % 19 };
			const int quantB { value % 19 };

			const float r { signPow( static_cast< float >( quantR - 9 ) / 9.0f, 2.0f ) * maximumValue };
			const float g { signPow( static_cast< float >( quantG - 9 ) / 9.0f, 2.0f ) * maximumValue };
			const float b { signPow( static_cast< float >( quantB - 9 ) / 9.0f, 2.0f ) * maximumValue };

			return { r, g, b };
		}
	} // namespace internal

	inline std::vector< std::uint8_t > decode( const std::string_view hash, int width, int height, int punch )
	{
		constexpr int channels { 4 };
		using namespace internal;

		const int bytes_per_row { channels * width };

		std::vector< std::uint8_t > buffer;
		buffer.resize( height * bytes_per_row );

		if ( !isValidBlurhash( hash ) ) throw std::runtime_error( "Invalid blurhash" );
		if ( punch < 1 ) punch = 1;

		const int size_flag { decodeToInt( hash, 0, 1 ) };
		const int components_y { static_cast< int >( std::floor( size_flag / 9 ) ) + 1 };
		const int components_x { ( size_flag % 9 ) + 1 };
		assert( components_x >= min_comp_size );
		assert( components_y >= min_comp_size );
		assert( components_x <= max_comp_size );
		assert( components_y <= max_comp_size );

		const int quantized_max_value { decodeToInt( hash, 1, 2 ) };
		assert( quantized_max_value <= 166 );

		const float maxValue { static_cast< float >( quantized_max_value + 1 ) / 166.0f };
		assert( maxValue < 1.0f );

		int colors_size { components_x * components_y };
		assert( colors_size <= max_comp_size * max_comp_size );
		float colors[ colors_size ][ 4 ];

		const auto dc { decodeDC( decodeToInt( hash, 2, 6 ) ) };
		colors[ 0 ][ RED ] = std::get< RED >( dc );
		colors[ 0 ][ GREEN ] = std::get< GREEN >( dc );
		colors[ 0 ][ BLUE ] = std::get< BLUE >( dc );
		colors[ 0 ][ ALPHA ] = 255.0f;

		for ( int itter = 1; itter < colors_size; ++itter )
		{
			const int value { decodeToInt( hash, 4 + itter * 2, 6 + itter * 2 ) };
			const auto ac { decodeAC( value, maxValue * static_cast< float >( punch ) ) };
			colors[ itter ][ RED ] = std::get< RED >( ac );
			colors[ itter ][ GREEN ] = std::get< GREEN >( ac );
			colors[ itter ][ BLUE ] = std::get< BLUE >( ac );
			colors[ itter ][ ALPHA ] = 255.0f;
		}

		constexpr float pi { std::numbers::pi_v< float > };

		//Calculate x basics
		float basics_x_precalc[ width ][ components_x ];
		for ( int x = 0; x < width; ++x )
		{
			const float x_pi { pi * static_cast< float >( x ) };
			for ( int i = 0; i < components_x; ++i )
				basics_x_precalc[ x ][ i ] =
					std::cos( ( x_pi * static_cast< float >( i ) ) / static_cast< float >( width ) );
		}

		for ( int y = 0; y < height; ++y )
		{
			const auto y_pi { pi * static_cast< float >( y ) };
			const int y_idx { y * bytes_per_row };

			float basics_y[ components_y ];
			for ( int j = 0; j < components_y; ++j )
				basics_y[ j ] = std::cos( ( y_pi * static_cast< float >( j ) ) / static_cast< float >( height ) );

			for ( int x = 0; x < width; x += 4 )
			{
				const auto x_idx { channels * x };
				__m128 one { _mm_setzero_ps() };
				__m128 two { _mm_setzero_ps() };
				__m128 three { _mm_setzero_ps() };
				__m128 four { _mm_setzero_ps() };

				const float* const basis_x_one { basics_x_precalc[ x ] };
				const float* const basis_x_two { basics_x_precalc[ x + 1 ] };
				const float* const basis_x_three { basics_x_precalc[ x + 2 ] };
				const float* const basis_x_four { basics_x_precalc[ x + 3 ] };

				for ( int y_c = 0; y_c < components_y; ++y_c )
				{
					for ( int x_c = 0; x_c < components_x; ++x_c )
					{
						const int colors_idx { y_c * components_x + x_c };
						const float* const color { colors[ colors_idx ] };

						const auto color_vec { _mm_load_ps( color ) };

						const __m128 basics_vec_one { _mm_set_ps1( basis_x_one[ x_c ] * basics_y[ y_c ] ) };
						one = _mm_add_ps( one, _mm_mul_ps( color_vec, basics_vec_one ) );

						const __m128 basics_vec_two { _mm_set_ps1( basis_x_two[ x_c ] * basics_y[ y_c ] ) };
						two = _mm_add_ps( two, _mm_mul_ps( color_vec, basics_vec_two ) );

						const __m128 basics_vec_three { _mm_set_ps1( basis_x_three[ x_c ] * basics_y[ y_c ] ) };
						three = _mm_add_ps( three, _mm_mul_ps( color_vec, basics_vec_three ) );

						const __m128 basics_vec_four { _mm_set_ps1( basis_x_four[ x_c ] * basics_y[ y_c ] ) };
						four = _mm_add_ps( four, _mm_mul_ps( color_vec, basics_vec_four ) );
					}
				}

				const __m128i one_i { wideLinearTosRGB( one ) };
				const __m128i two_i { wideLinearTosRGB( two ) };
				const __m128i three_i { wideLinearTosRGB( three ) };
				const __m128i four_i { wideLinearTosRGB( four ) };

				//Shift dance time
				const auto first { _mm_packs_epi32( four_i, three_i ) };
				const auto second { _mm_packs_epi32( two_i, one_i ) };
				const auto third { _mm_packus_epi16( second, first ) };

				//memcpy
				//Check that the index will be aligned
				_mm_storeu_si128( reinterpret_cast< __m128i* >( buffer.data() + x_idx + y_idx ), third );
			}

			for ( int x = width - ( width % 4 ); x < width; ++x )
			{
				const int x_idx { channels * x };
				const float* const basics_x { basics_x_precalc[ x ] };

				__m128 vec { _mm_set_ps( 0.0f, 0.0f, 0.0f, 0.0f ) };

				for ( int j = 0; j < components_y; ++j )
				{
					const int j_idx { j * components_x };
					for ( int i = 0; i < components_x; ++i )
					{
						const int idx { j_idx + i };

						const __m128 basics { _mm_set_ps1( basics_x[ j ] * basics_y[ i ] ) };

						const __m128 colors_vec { _mm_load_ps( colors[ idx ] ) };

						vec = _mm_add_ps( vec, _mm_mul_ps( basics, colors_vec ) );
					}
				}

				const auto idx { static_cast< std::uint64_t >( x_idx + y_idx ) };

				const __m128i simd_out { wideClampToUByte( wideLinearTosRGB( vec ) ) };

				//Unpack simd_out
				int simd_out_arr[ 4 ];
				_mm_storeu_si128( reinterpret_cast< __m128i* >( simd_out_arr ), simd_out );
				buffer[ idx + RED ] = simd_out_arr[ 0 ];
				buffer[ idx + GREEN ] = simd_out_arr[ 1 ];
				buffer[ idx + BLUE ] = simd_out_arr[ 2 ];
				buffer[ idx + ALPHA ] = 255; // If nChannels=4, treat each pixel as RGBA instead of RGB
			}
		}

		return buffer;
	}
} // namespace blurhash::testing

#endif
