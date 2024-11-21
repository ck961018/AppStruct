#pragma once

#include <boost/unordered/concurrent_flat_map.hpp>
#include <entt/entt.hpp>

#include "Meta.hpp"
#include <print>

using namespace entt::literals;
using namespace std::literals;

struct TypeNameMap
{
    inline static boost::concurrent_flat_map<size_t, std::string>& Instance()
    {
        static boost::concurrent_flat_map<size_t, std::string> single;
        return single;
    }
};

struct BCAutoRegisterWrapper
{
    template <typename T>
    struct BCAutoRegister{};
};

#define BC_REGISTRATION(TYPE)                                                                                \
    template <>                                                                                              \
    struct BCAutoRegisterWrapper::BCAutoRegister<TYPE>                                                       \
    {                                                                                                        \
        explicit BCAutoRegister(std::string_view name)                                                       \
        {                                                                                                    \
            entt::meta_factory factory{entt::meta<TYPE>()};                                                  \
            auto hashed_value{entt::hashed_string(name.data(), name.length()).value()};                      \
            assert(hashed_value != entt::type_hash<TYPE>().value());                                         \
            factory.type(hashed_value);                                                                      \
            TypeNameMap::Instance().emplace(hashed_value, name);                                             \
            BCRegister(factory);                                                                             \
        }                                                                                                    \
                                                                                                             \
        template <typename U>                                                                                \
        static void BCRegister(entt::meta_factory<U>& factory);                                              \
    };                                                                                                       \
                                                                                                             \
    inline const BCAutoRegisterWrapper::BCAutoRegister<TYPE> BC_CAT(auto_register_, TYPE){BC_CAT(#TYPE, s)}; \
                                                                                                             \
    template <typename U>                                                                                    \
    void BCAutoRegisterWrapper::BCAutoRegister<TYPE>::BCRegister(entt::meta_factory<U>& factory [[maybe_unused]])

#define BC_REGISTER_BASE(BASE)     \
    factory.template base<BASE>(); \
    BCAutoRegister<BASE>::BCRegister(factory)

#define BC_REGISTER_PROPERTY(p) factory.template data<&U::p, entt::as_ref_t>(BC_CAT(#p, _hs))

#define BC_REGISTER_FUNCTION(f) factory.template func<&U::f>(BC_CAT(#f, _hs))

// private域注册
// 方案一：
// 仿照RTTR，在类中声明Friend类；
#define BC_REGISTER_FRIEND friend BCAutoRegisterWrapper;

// 方案二：
// 利用编译器漏洞访问类私有对象 https://gist.github.com/dabrahams/1528856
