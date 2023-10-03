#ifndef BLURHASH_COMMON_HPP
#define BLURHASH_COMMON_HPP

#include <algorithm>
#include <array>
#include <cmath>
#include <math.h>

enum blurhashColor
{
	RED = 0,
	GREEN,
	BLUE
};

constexpr inline int linearTosRGB( float value )
{
	const float v { std::clamp( value, 0.0f, 1.0f ) };
	if ( v <= 0.0031308f )
		return static_cast< int >( std::round( v * 12.92f * 255.0f ) );
	else
		return static_cast< int >( std::round( ( 1.055f * std::pow( v, 1.0f / 2.4f ) - 0.055f ) * 255.0f ) );
}

#define USE_LUT 1

#ifdef USE_LUT
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

inline float sRGBToLinear( int value )
{
	if ( value >= sRGBLookupTable.size() )
	{
		throw std::runtime_error( "Value was higher then lookup table! Was:" + std::to_string( value ) );
	}
	return sRGBLookupTable[ value ];
}
#else
constexpr inline float sRGBToLinear( int value )
{
	const float v { static_cast< float >( value ) / 255.0f };
	if ( v <= 0.04045f )
		return v / 12.92f;
	else
		return std::pow( ( v + 0.055f ) / 1.055f, 2.4f );
}
#endif

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

constexpr auto chars = base_83_set;

#endif
