#ifndef BLURHASH_DECODE_HPP
#define BLURHASH_DECODE_HPP

#include <assert.h>
#include <cstdint>
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

namespace blurhash
{
	namespace internal
	{

		inline uint8_t clampToUByte( const int val )
		{
			if ( val <= 0 )
				return 0;
			else if ( val >= 255 )
				return 255;
			else [[likely]]
				return static_cast< uint8_t >( val );
		}

		inline int decodeToInt( const std::string_view string, int start, int end )
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

				if ( index == -1 )
					throw std::runtime_error( "Failed to decode value " + std::to_string( character ) + " to int" );
				value = value * chars_size + index;
			}
			return value;
		}

		inline bool isValidBlurhash( const std::string_view blurhash )
		{
			const int hash_length { static_cast< int >( blurhash.size() ) };

			if ( hash_length < 6 )
				throw std::
					runtime_error( "Hash was not larger then min size. Was: " + std::to_string( blurhash.size() ) );

			const int sizeFlag { decodeToInt( blurhash, 0, 1 ) }; // Get size from first character
			const int numY { static_cast< int >( std::div( sizeFlag, 9 ).quot ) + 1 };
			const int numX { ( sizeFlag % 9 ) + 1 };

			if ( hash_length != 4 + 2 * numX * numY ) return false;
			return true;
		}

		inline std::tuple< float, float, float > decodeDC( int value )
		{
			const float r { sRGBToLinear( value >> 16 ) }; // R-component
			const float g { sRGBToLinear( ( value >> 8 ) & 255 ) }; // G-Component
			const float b { sRGBToLinear( value & 255 ) }; // B-Component

			return { r, g, b };
		}

		inline std::tuple< float, float, float > decodeAC( int value, const float maximumValue )
		{
			const int quantR { static_cast< int >( floorf( static_cast< float >( value ) / ( 19.0f * 19.0f ) ) ) };
			const int quantG { static_cast< int >( floorf( static_cast< float >( value ) / 19.0f ) ) % 19 };
			const int quantB { value % 19 };

			const float r { signPow( ( static_cast< float >( quantR ) - 9.0f ) / 9.0f, 2.0f ) * maximumValue };
			const float g { signPow( ( static_cast< float >( quantG ) - 9.0f ) / 9.0f, 2.0f ) * maximumValue };
			const float b { signPow( ( static_cast< float >( quantB ) - 9.0f ) / 9.0f, 2.0f ) * maximumValue };

			return { r, g, b };
		}
	} // namespace internal

	inline std::vector< std::uint8_t >
		decode( const std::string_view hash, int width, int height, int punch, int channels )
	{
		using namespace internal;

		const int bytes_per_row { channels * width };

		std::vector< std::uint8_t > buffer;
		buffer.resize( height * bytes_per_row );

		if ( !isValidBlurhash( hash ) ) throw std::runtime_error( "Invalid blurhash" );
		if ( punch < 1 ) punch = 1;

		const int size_flag { decodeToInt( hash, 0, 1 ) };
		const int components_y { static_cast< int >( std::floor( size_flag / 9 ) ) + 1 };
		const int components_x { ( size_flag % 9 ) + 1 };
		assert( hash_components_x > 1 );
		assert( hash_components_y > 1 );
		assert( hash_components_x < 9 );
		assert( hash_components_y < 9 );

		const int quantized_max_value { decodeToInt( hash, 1, 2 ) };

		const float maxValue { static_cast< float >( quantized_max_value + 1 ) / 166.0f };

		int colors_size { components_x * components_y };
		float colors[ colors_size ][ 3 ];

		const auto dc { decodeDC( decodeToInt( hash, 2, 6 ) ) };
		colors[ 0 ][ RED ] = std::get< RED >( dc );
		colors[ 0 ][ GREEN ] = std::get< GREEN >( dc );
		colors[ 0 ][ BLUE ] = std::get< BLUE >( dc );

		for ( int itter = 1; itter < colors_size; ++itter )
		{
			const int value { decodeToInt( hash, 4 + itter * 2, 6 + itter * 2 ) };
			const auto ac { decodeAC( value, maxValue * static_cast< float >( punch ) ) };
			colors[ itter ][ RED ] = std::get< RED >( ac );
			colors[ itter ][ GREEN ] = std::get< GREEN >( ac );
			colors[ itter ][ BLUE ] = std::get< BLUE >( ac );
		}

		for ( int y = 0; y < height; ++y )
		{
			const float y_pi { std::numbers::pi_v< float > * static_cast< float >( y ) };
			const int y_idx { y * bytes_per_row };
			for ( int x = 0; x < width; ++x )
			{
				const float x_pi { std::numbers::pi_v< float > * static_cast< float >( x ) };
				const int x_idx { channels * x };
				float r { 0.0f };
				float g { 0.0f };
				float b { 0.0f };

				float basics_x[ components_x ];
				float basics_y[ components_y ];
				for ( int j = 0; j < components_y; ++j )
					basics_y[ j ] = cosf( ( y_pi * static_cast< float >( j ) ) / static_cast< float >( height ) );
				for ( int i = 0; i < components_x; ++i )
					basics_x[ i ] = cosf( ( x_pi * static_cast< float >( i ) ) / static_cast< float >( width ) );

				for ( int j = 0; j < components_y; ++j )
				{
					const int j_idx { j * components_x };
					for ( int i = 0; i < components_x; ++i )
					{
						const float basics { basics_x[ j ] * basics_y[ i ] };

						const int idx { j_idx + i };
						r += colors[ idx ][ RED ] * basics;
						g += colors[ idx ][ GREEN ] * basics;
						b += colors[ idx ][ BLUE ] * basics;
					}
				}

				const auto idx { static_cast< std::uint64_t >( x_idx + y_idx ) };
				buffer[ idx + RED ] = clampToUByte( linearTosRGB( r ) );
				buffer[ idx + GREEN ] = clampToUByte( linearTosRGB( g ) );
				buffer[ idx + BLUE ] = clampToUByte( linearTosRGB( b ) );

				if ( channels == 4 ) buffer[ idx + 3 ] = 255; // If nChannels=4, treat each pixel as RGBA instead of RGB
			}
		}

		return buffer;
	}
} // namespace blurhash

#endif
