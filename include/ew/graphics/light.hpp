#pragma once

#include <ew/ew_config.hpp>

#include <ew/core/types/types.hpp>

#include <ew/maths/vector.hpp>

#include <ew/graphics/color.hpp>

namespace ew
{
namespace graphics
{

using namespace ew::core::types;
using namespace ew::maths;
using namespace ew::graphics::colors;

class Light
{
public:
	enum LightType { POSITION, SPOT, DIRECTIONAL };

	inline  Light(void);
	inline  Light(const vec4 & position,
		      const vec4 & direction,
		      const color4 & ambient,
		      const color4 & diffuse,
		      const color4 & specular);

	inline void SetType(enum LightType & type);
	inline enum LightType GetType(void) const ;

	inline void SetConstantAttenuation(f32 value);
	inline void SetLinearAttenuation(f32 value);
	inline void SetQuadraticAttenuation(f32 value);

public:
	enum LightType m_type;
	vec4 m_position;
	vec4 m_direction;
	color4 m_ambient;
	color4 m_diffuse;
	color4 m_specular;
	f32 m_constant_attenuation;
	f32 m_linear_attenuation;
	f32 m_quadratic_attenuation;

	/*
	  dx
	  float Range;
	  float Falloff;
	*/
};

}
}

#include <ew/graphics/light.hxx>
