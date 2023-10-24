#ifndef BLURHASH_COMMON_HPP
#define BLURHASH_COMMON_HPP

#include <algorithm>
#include <array>
#include <cmath>
#include <immintrin.h>
#include <math.h>
#include <stdexcept>

enum blurhashColor
{
	RED = 0,
	GREEN,
	BLUE,
	ALPHA
};

constexpr inline int linearTosRGB( float value )
{
	if ( value <= 0.0031308f ) return static_cast< int >( std::round( value * 12.92f * 255.0f ) );
	const float v { std::min( value, 1.0f ) };
	return static_cast< int >( std::round( ( 1.055f * std::pow( v, 1.0f / 2.4f ) - 0.055f ) * 255.0f ) );
}

inline __m128i wideLinearTosRGB( __m128 values )
{
	constexpr float comp_const { 0.0031308f };
	const __m128 comp { _mm_set1_ps( comp_const ) };

	//The mask will have 4 segments. If the value is 0 then the value is GREATER then the comp_const. Else it is LESS OR EQUAL to the comp_const.
	const __m128 mask { _mm_cmp_ps( values, comp, _MM_CMPINT_LE ) };
	const auto mask_bits { _mm_movemask_ps( mask ) };

	// Make new set of only trues
	__m128 true_values { _mm_and_ps( mask, values ) }; // Use bottom path

	if ( mask_bits & 0b1111 )
	{
		//Check if there are any true values
		true_values = _mm_mul_ps( true_values, _mm_set1_ps( 12.92f * 255.0f ) );
		true_values = _mm_and_ps( true_values, mask );
	}

	// Shift all false results of the mask to a new register
	__m128 false_values { _mm_andnot_ps( mask, values ) }; // Use top path
	false_values = _mm_min_ps( false_values, _mm_set1_ps( 1.0f ) );

	if ( ( ~mask_bits ) & 0b1111 )
	{
		//Split up values to do pow operation
		float data[ 4 ];
		_mm_store_ps( data, false_values );
		for ( int i = 0; i < 4; ++i )
		{
			if ( data[ i ] == 0.0f ) continue;
			constexpr float exponent { 1.0f / 2.4f };
			data[ i ] = std::pow( data[ i ], exponent );
		}
		//Repack
		false_values = _mm_load_ps( data );

		false_values = _mm_mul_ps( false_values, _mm_set1_ps( 1.055f ) );
		false_values = _mm_sub_ps( false_values, _mm_set1_ps( 0.055f ) );
		false_values = _mm_mul_ps( false_values, _mm_set1_ps( 255.0f ) );

		//Round
		false_values = _mm_round_ps( false_values, _MM_ROUND_NEAREST );
		false_values = _mm_andnot_ps( mask, false_values );
	}

	//Recombine the two
	const auto combined { _mm_or_ps( true_values, false_values ) };

	//Convert to integer
	return _mm_cvtps_epi32( combined );
}

namespace internal
{
	constexpr inline float sRGBToLinear( int value )
	{
		const float v { static_cast< float >( value ) / 255.0f };
		if ( v <= 0.04045f )
			return v / 12.92f;
		else
			return std::pow( ( v + 0.055f ) / 1.055f, 2.4f );
	}
} // namespace internal

inline static constexpr std::array< float, 256 > sRGBLookupTable {
	[]() constexpr
	{
		std::array< float, 256 > buffer {};

		for ( size_t i = 0; i < buffer.size(); ++i )
		{
			buffer[ i ] = internal::sRGBToLinear( static_cast< int >( i ) );
		}

		return buffer;
	}()
};

constexpr inline float sRGBToLinear( int value )
{
	if ( static_cast< std::size_t >( value ) >= sRGBLookupTable.size() )
	{
		throw std::runtime_error( "Value was higher then lookup table! Was:" + std::to_string( value ) );
	}
	return sRGBLookupTable[ static_cast< std::size_t >( value ) ];
}

constexpr inline float signPow( float value, float exp )
{
	return std::copysign( std::pow( std::abs( value ), exp ), value );
}

constexpr std::array< char, 83 > base_83_set {
	[]() constexpr
	{
		const auto str { "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz#$%*+,-.:;=?@[]^_{|}~" };

		std::array< char, 83 > buffer {};

		for ( size_t i = 0; i < buffer.size(); ++i ) buffer[ i ] = str[ i ];

		return buffer;
	}()
};

inline static constexpr auto chars = base_83_set;
inline static constexpr int min_comp_size { 2 };
inline static constexpr int max_comp_size { 8 };

#endif
