#include <any>
#include <execution>
#include <iostream>
#include <print>
#include <ranges>

#include <entt/entt.hpp>
#include <pugixml.hpp>

#include "Document.h"
#include "RTTI.hpp"
#include "Serializer.h"
#include "TransactionObject.h"

// #include "Test.h"

using namespace entt::literals;
using namespace std::literals;

struct A
{
    // A() = default;
    // A(A const&) = default;
    // A& operator=(A const&) = default;

    // A(A&&) noexcept = default;
    // A& operator=(A&&) noexcept = default;

    void Print()
    {
        std::print("A: {}, {}: {}\n", (void*)this, (void*)&a, a);
    }

    int a{3};
};

struct C
{
    C() = default;
    C(C const&)
    {
        std::println("C:Copy");
    }
    C& operator=(C const&) = default;

    C(C&&) noexcept
    {
        std::println("C:Move");
    }
    C& operator=(C&&) noexcept = default;
    int c{77};
};

class B : public A
{
public:
    B() = default;
    B(B const&)
    {
        std::println("B:Copy");
    }
    B& operator=(B const&) = default;

    B(B&&) noexcept
    {
        std::println("B:Move");
    }
    B& operator=(B&&) noexcept = default;

    void OnBeforeChange()
    {
        std::print("before change\n");
    }

    void OnChanged()
    {
        std::print("changed\n");
    }

    void Print()
    {
        std::print("B: {}, {}: {}, {}: {}, {}: {}\n", (void*)this, (void*)&a, a, (void*)&b, b, (void*)&c, c.c);
    }

    int b{342};
    C c{};
};

BC_REGISTRATION(A)
{
    BC_REGISTER_PROPERTY(a);
    BC_REGISTER_FUNCTION(Print);
}

BC_REGISTRATION(B)
{
    BC_REGISTER_PROPERTY(b);
    BC_REGISTER_PROPERTY(c);
    BC_REGISTER_FUNCTION(OnChanged);
    BC_REGISTER_FUNCTION(OnBeforeChange);
    BC_REGISTER_FUNCTION(Print);
    BC_REGISTER_BASE(A);
}

BC_REGISTRATION(C)
{
    BC_REGISTER_PROPERTY(c);
}

int main()
{
    // pugi::xml_document doc;

    // // tag::code[]
    // // add node with some name
    // pugi::xml_node node = doc.append_child(pugi::node_element);
    // node.set_name("fdsa");
    // node.append_attribute("sss") = 32;

    // // add description node with text child
    // pugi::xml_node descr = doc.append_child("description");
    // descr.append_child(pugi::node_cdata).set_value("Simple node");

    // // add param node before the description
    // pugi::xml_node param = doc.insert_child_before("param", descr);

    // // add attributes to param node
    // param.append_attribute("name") = "version";
    // param.append_attribute("value") = 1.1;
    // param.insert_attribute_after("type", param.attribute("name")) = "float";
    // // end::code[]

    // doc.print(std::cout);

    B b;

    Document doc;
    auto entity_b{doc.AddObject(std::move(b))};
    auto entity_a{doc.AddObject(A{})};

    auto xml{Serializer::SaveMeta(&doc)};
    xml.print(std::cout);

    // std::println("{}", (void*)doc.GetMetaObject(entity_a));
    // doc.Undo();
    // std::println("{}", (void*)doc.GetMetaObject(entity_a));
    // doc.Redo();
    // std::println("{}", (void*)doc.GetMetaObject(entity_a));

    // std::println("{}", (void*)doc.GetMetaObject(entity_b));
    // doc.Undo();
    // doc.Undo();
    // std::println("{}", (void*)doc.GetMetaObject(entity_b));
    // doc.Redo();
    // std::println("{}", (void*)doc.GetMetaObject(entity_b));

    // doc.GetObject<B>(entity_b)->Print();

    // std::vector cmds{"c"s, "c"s};
    // doc.ChangePropertyWithNewTransaciton(entity_b, std::span{cmds.begin(), cmds.end()}, 3211);

    // doc.GetObject<B>(entity_b)->Print();

    // doc.Undo();

    // doc.GetObject<B>(entity_b)->Print();
    // std::unordered_map<int, int> map{};
    // auto type{entt::resolve<std::unordered_map<int, int>>()};
    // auto any{type.from_void(&map)};
    // std::println("{}", type.is_associative_container());
    // std::println("{}", type.is_template_specialization());
    // std::println("{}", any.type().info().name());
}
