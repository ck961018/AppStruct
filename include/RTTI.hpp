#pragma once

#include <entt/entt.hpp>

#include "Meta.hpp"

using namespace entt::literals;

#define BC_REGISTRATION(t) \
    template <typename T>  \
    static void BC_CAT(BCRegister, t)(entt::meta_factory<T> factory)

#define BC_DO_REGISTRATION(t)                                 \
    static void BC_CAT(BCAutoRegisterReflectionFunction, t)() \
    {                                                         \
        static bool is_registered = false;                    \
        if (is_registered)                                    \
            return;                                           \
        is_registered = true;                                 \
                                                              \
        entt::meta_factory factory{entt::meta<t>()};          \
        factory.type(#t##_hs);                                \
        BC_CAT(BCRegister, t)(factory);                       \
    }                                                         \
    namespace                                                 \
    {                                                         \
    struct BC_CAT(BCAutoRegister, t)                          \
    {                                                         \
        BC_CAT(BCAutoRegister, t)()                           \
        {                                                     \
            BC_CAT(BCAutoRegisterReflectionFunction, t)();    \
        }                                                     \
    };                                                        \
    }                                                         \
    static const BC_CAT(BCAutoRegister, t) BC_CAT(auto_register_, __LINE__)

#define BC_REGISTER_BASE(b)     \
    factory.template base<b>(); \
    BC_CAT(BCRegister, b)(factory)

#define BC_REGISTER_PROPERTY(p) factory.template data<&T::p, entt::as_ref_t>(#p##_hs)
#define BC_REGISTER_FUNCTION(f) factory.template func<&T::f>(#f##_hs)
