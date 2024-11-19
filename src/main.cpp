#include <any>
#include <execution>
#include <iostream>
#include <print>
#include <ranges>

#include <entt/entt.hpp>

#include "Document.h"
#include "RTTI.hpp"
#include "TransactionObject.h"

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

struct B : public A
{
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

BC_DO_REGISTRATION(A);

BC_REGISTRATION(B)
{
    BC_REGISTER_PROPERTY(b);
    BC_REGISTER_PROPERTY(c);
    BC_REGISTER_FUNCTION(OnChanged);
    BC_REGISTER_FUNCTION(OnBeforeChange);
    BC_REGISTER_FUNCTION(Print);
    BC_REGISTER_BASE(A);
}

BC_DO_REGISTRATION(B);

BC_REGISTRATION(C)
{
    BC_REGISTER_PROPERTY(c);
}

BC_DO_REGISTRATION(C);

int main()
{
    B b;

    Document doc;
    auto entity_b{doc.AddObject(std::move(b))};
    auto entity_a{doc.AddObject(A{})};

    std::println("{}, {}", (size_t)entity_b, (size_t)entity_a);
    std::println("{}, {}", doc.m_undo_stack.size(), doc.m_redo_stack.size());

    std::println("{}", (void*)doc.GetMetaObject(entity_a));
    doc.Undo();
    std::println("{}", (void*)doc.GetMetaObject(entity_a));
    doc.Redo();
    std::println("{}", (void*)doc.GetMetaObject(entity_a));

    std::println("{}", (void*)doc.GetMetaObject(entity_b));
    doc.Undo();
    doc.Undo();
    std::println("{}", (void*)doc.GetMetaObject(entity_b));
    doc.Redo();
    std::println("{}", (void*)doc.GetMetaObject(entity_b));

    // doc.GetObject<B>(entity_b)->Print();

    // std::vector cmds{"c"s, "c"s};
    // doc.ChangePropertyWithNewTransaciton(entity_b, std::span{cmds.begin(), cmds.end()}, 3211);

    // doc.GetObject<B>(entity_b)->Print();

    // doc.Undo();

    // doc.GetObject<B>(entity_b)->Print();

    // auto new_any{any->type().from_void(any)};

    // TransactionObjectChnImpl to{std::move(new_any), "b", 32};
    // TransactionObjectChn toc{entt::meta_any{}, std::move(to)};
    // toc.Apply();
    // auto anyy = doc.FindObject<B>("B");
    // anyy->Print();
    // toc.Apply();
    // anyy->Print();
}
