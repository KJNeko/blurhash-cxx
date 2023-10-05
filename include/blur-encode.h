#ifndef BLURHASH_ENCODE_HPP
#define BLURHASH_ENCODE_HPP

#include <cstdint>
#include <cstring>
#include <string>

#include "blur-common.h"

constexpr int number_of_components_size { 1 };
constexpr int max_ac_components_size { 1 };
constexpr int dc_component_size { 4 };
constexpr int characters_per_pixel { 2 };
constexpr int sRGB_size { 24 };
constexpr int color_size { 24 / 8 };

namespace blurhash
{
	namespace internal
	{

		inline char* encode_int( const int value, const int length, char* buffer )
		{
			if ( value < 0 )
				throw std::runtime_error( "Value given to encode was less than zero. Which is unsupported" );
			int divisor = 1;
			for ( int i = 0; i < length - 1; ++i ) divisor *= 83;

			for ( int i = 0; i < length; ++i )
			{
				const int digit { ( value / divisor ) % 83 };
				divisor /= 83;
				if ( digit < 0 ) throw std::runtime_error( "Digit was below zero" );
				*buffer++ = chars[ digit ];
			}
			return buffer;
		}

		//! Constexpr encodeInt used for encoding an integer during compile time
		template < std::uint64_t length >
		inline constexpr std::array< char, length > constEncodeInt( int value )
		{
			std::array< char, length > buffer;

			encode_int( value, length, buffer.data() );

			return buffer;
		}

		inline std::tuple< float, float, float > multiplyBasisFunction(
			const float* x_basis,
			const float* y_basis,
			const int x_comp,
			const int y_comp,
			const int width,
			const int height,
			const uint8_t* rgb,
			const int bytes_per_row )
		{
			float r = 0.0f, g = 0.0f, b = 0.0f;
			const float normalisation = ( x_comp == 0 && y_comp == 0 ) ? 1.0f : 2.0f;

			for ( int y = 0; y < height; ++y )
			{
				const int y_idx { y * bytes_per_row };
				for ( int x = 0; x < width; ++x )
				{
					const float basis { x_basis[ x ] * y_basis[ y ] };

					const int x_idx { 3 * x };
					const int idx { x_idx + y_idx };

					r += basis * sRGBToLinear( rgb[ idx + RED ] );
					g += basis * sRGBToLinear( rgb[ idx + GREEN ] );
					b += basis * sRGBToLinear( rgb[ idx + BLUE ] );
				}
			}

			const float scale { normalisation / static_cast< float >( width * height ) };

			return std::make_tuple( r * scale, g * scale, b * scale );
		}

		constexpr inline int encodeDC( float r, float g, float b )
		{
			const int roundedR { linearTosRGB( r ) };
			const int roundedG { linearTosRGB( g ) };
			const int roundedB { linearTosRGB( b ) };
			return ( roundedR << 16 ) + ( roundedG << 8 ) + roundedB;
		}

		constexpr inline int encodeAC( float r, float g, float b, float maximumValue )
		{
			const int quantR { static_cast<
				int >( std::clamp( std::floor( signPow( r / maximumValue, 0.5f ) * 9.0f + 9.5f ), 0.0f, 18.0f ) ) };
			const int quantG { static_cast<
				int >( std::clamp( std::floor( signPow( g / maximumValue, 0.5f ) * 9.0f + 9.5f ), 0.0f, 18.0f ) ) };
			const int quantB { static_cast<
				int >( std::clamp( std::floor( signPow( b / maximumValue, 0.5f ) * 9.0f + 9.5f ), 0.0f, 18.0f ) ) };

			return ( quantR * 19 * 19 ) + ( quantG * 19 ) + quantB;
		}

	} // namespace internal

	template < int x_comp = 8, int y_comp = 8 >
	std::string encode( int width, int height, const std::uint8_t* rgb, const int channels )
	{
		using namespace internal;
		constexpr int component_count { x_comp * y_comp - 1 };
		static_assert( x_comp > 1 && x_comp < 9 );
		static_assert( y_comp > 1 && y_comp < 9 );
		const int bytes_per_row { channels * width };

		std::array< std::array< std::array< float, 3 >, x_comp >, y_comp > factors;

		float basis_x_precalc[ x_comp ][ width ];
		for ( int x = 0; x < x_comp; ++x )
		{
			for ( int w = 0; w < width; ++w )
			{
				basis_x_precalc[ x ][ w ] = cosf( std::numbers::pi_v< float > * x * w / width );
			}
		}

		for ( int y = 0; y < y_comp; ++y )
		{
			float y_basis[ height ];
			for ( int h = 0; h < height; ++h )
			{
				y_basis[ h ] = cosf( std::numbers::pi_v< float > * y * h / height );
			}
			for ( int x = 0; x < x_comp; ++x )
			{
				const auto factor {
					multiplyBasisFunction( basis_x_precalc[ x ], y_basis, x, y, width, height, rgb, bytes_per_row )
				};
				factors[ y ][ x ][ RED ] = std::get< RED >( factor );
				factors[ y ][ x ][ GREEN ] = std::get< GREEN >( factor );
				factors[ y ][ x ][ BLUE ] = std::get< BLUE >( factor );
			}
		}

		const float* const dc { factors[ 0 ][ 0 ].data() };
		const float* const ac { dc + 3 };
		constexpr int acCount { x_comp * y_comp - 1 };

		constexpr int sizeFlag { ( x_comp - 1 ) + ( y_comp - 1 ) * 9 };

		std::array<
			char,
			number_of_components_size + max_ac_components_size + ( component_count * characters_per_pixel )
				+ dc_component_size >
			buffer;

		char* ptr { buffer.data() };

		//Encoding byte 1
		const auto size_encoding { constEncodeInt< number_of_components_size >( sizeFlag ) };
		std::memcpy( ptr, size_encoding.data(), size_encoding.size() );
		ptr += size_encoding.size();

		float max_value { 0.0f };
		if ( acCount > 0 )
		{
			float actual_max_value { 0.0f };
			for ( int i = 0; i < acCount * 3; ++i )
			{
				actual_max_value = std::max( std::abs( ac[ i ] ), actual_max_value );
			}

			const int quantized_max_value {
				std::clamp( static_cast< int >( std::floor( actual_max_value * 166.0f - 0.5f ) ), 0, 82 )
			};

			max_value = static_cast< float >( quantized_max_value + 1 ) / 166.0f;

			//Encoding byte 2
			ptr = encode_int( quantized_max_value, max_ac_components_size, ptr );
		}
		else
		{
			//Encoding byte 2
			max_value = 1.0f;
			ptr = encode_int( 0, max_ac_components_size, ptr );
		}

		ptr = encode_int( encodeDC( dc[ RED ], dc[ GREEN ], dc[ BLUE ] ), dc_component_size, ptr );

		for ( int i = 0; i < acCount; ++i )
		{
			const auto idx { i * 3 };
			const auto ac_val { encodeAC( ac[ idx + RED ], ac[ idx + 1 ], ac[ idx + 2 ], max_value ) };

			ptr = encode_int( ac_val, characters_per_pixel, ptr );
		}

		static_assert( buffer.size() > 6 );

		return std::string( buffer.data(), buffer.size() );
	}

} // namespace blurhash

#endif
