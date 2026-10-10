#ifndef IMPACTO_SHADER_UTIL_H_HEADER_GUARD
#define IMPACTO_SHADER_UTIL_H_HEADER_GUARD

bool toBool(vec4 value) { return value.x != 0.0f; }

#if BGFX_SHADER_LANGUAGE_GLSL
#define mtxFromVals(_m11, _m12, _m13, _m14, \
                    _m21, _m22, _m23, _m24, \
                    _m31, _m32, _m33, _m34, \
                    _m41, _m42, _m43, _m44) \
    mat4(                                   \
        _m11, _m21, _m31, _m41,             \
        _m12, _m22, _m32, _m42,             \
        _m13, _m23, _m33, _m43,             \
        _m14, _m24, _m34, _m44              \
    )
#else
#define mtxFromVals(_m11, _m12, _m13, _m14, \
                    _m21, _m22, _m23, _m24, \
                    _m31, _m32, _m33, _m34, \
                    _m41, _m42, _m43, _m44) \
    mat4(                                   \
        _m11, _m12, _m13, _m14,             \
        _m21, _m22, _m23, _m24,             \
        _m31, _m32, _m33, _m34,             \
        _m41, _m42, _m43, _m44              \
    )
#endif

#endif // IMPACTO_SHADER_UTIL_H_HEADER_GUARD
