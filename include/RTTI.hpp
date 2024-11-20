#pragma once

#include <entt/entt.hpp>

#include "Meta.hpp"

using namespace entt::literals;
using namespace std::literals;

template <typename T>
struct BCAutoRegister;

#define BC_REGISTRATION(TYPE)                                                         \
    template <>                                                                       \
    struct BCAutoRegister<TYPE>                                                       \
    {                                                                                 \
        BCAutoRegister(std::string_view name)                                         \
        {                                                                             \
            entt::meta_factory factory{entt::meta<TYPE>()};                           \
            factory.type(entt::hashed_string(name.data(), name.length()));            \
            BCRegister<TYPE>(factory);                                                \
        }                                                                             \
                                                                                      \
        template <typename U>                                                         \
        static void BCRegister(entt::meta_factory<U>& factory);                       \
    };                                                                                \
                                                                                      \
    inline const BCAutoRegister<TYPE> BC_CAT(auto_register_, TYPE)(BC_CAT(#TYPE, s)); \
                                                                                      \
    template <typename U>                                                             \
    void BCAutoRegister<TYPE>::BCRegister(entt::meta_factory<U>& factory)

#define BC_REGISTER_BASE(BASE)     \
    factory.template base<BASE>(); \
    BCAutoRegister<BASE>::BCRegister(factory)

#define BC_REGISTER_PROPERTY(p) factory.template data<&U::p, entt::as_ref_t>(BC_CAT(#p, _hs))

#define BC_REGISTER_FUNCTION(f) factory.template func<&U::f>(BC_CAT(#f, _hs))
