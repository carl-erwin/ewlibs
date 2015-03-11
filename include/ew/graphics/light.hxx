#ifndef __EW_GRAPHICS_LIGHT_HXX__
#define __EW_GRAPHICS_LIGHT_HXX__

#include <ew/graphics/light.hpp>


namespace ew
{
    namespace graphics
    {

        inline Light::Light(void)
            :
            m_type(DIRECTIONAL),
            m_position(0, 0, 0, 1),
            m_direction(0, 0, 1, 1),
            m_ambient(1, 1, 1, 1),
            m_diffuse(0, 0, 0, 0),
            m_specular(0, 0, 0, 0),
            m_constant_attenuation(1),
            m_linear_attenuation(0),
            m_quadratic_attenuation(0)
        {
        }

        inline Light::Light(const vec4 & position,
                            const vec4 & direction,
                            const color4 & ambient,
                            const color4 & diffuse,
                            const color4 & specular)
            :
            m_type(DIRECTIONAL),
            m_position(position),
            m_direction(direction),
            m_ambient(ambient),
            m_diffuse(diffuse),
            m_specular(specular),
            m_constant_attenuation(1),
            m_linear_attenuation(0),
            m_quadratic_attenuation(0)
        {
        }

        inline     void Light::SetType(enum LightType & type)
        {
            m_type = type;
        }

        inline     enum ew::graphics::Light::LightType Light::GetType(void) const
        {
            return (m_type);
        }

        inline     void Light::SetConstantAttenuation(f32 value)
        {
            m_constant_attenuation = value;
        }

        inline     void Light::SetLinearAttenuation(f32 value)
        {
            m_constant_attenuation = value;
        }

        inline     void Light::SetQuadraticAttenuation(f32 value)
        {
            m_constant_attenuation = value;
        }

    }
} // ! namespace graphics

#endif
