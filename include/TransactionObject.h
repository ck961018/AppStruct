#pragma once

#include <variant>

#include <entt/entt.hpp>

#include "RTTI.hpp"

using namespace entt::literals;

class Document;

// TODO: 更改其它组件中的内容
class TransactionObjectChnImpl
{
public:
TransactionObjectChnImpl() = default;
    TransactionObjectChnImpl(entt::meta_any&& old_property, entt::meta_any&& new_property);

    void Apply();

private:
    entt::meta_any m_old_property;
    entt::meta_any m_new_property;
};

class TransactionObjectSubChn
{
public:
    TransactionObjectSubChn(entt::meta_any&& object, entt::meta_any&& transaction_object);

    void Apply();

private:
    entt::meta_any m_object;
    entt::meta_any m_transaction_object;
};

class TransactionObjectChn
{
public:
    TransactionObjectChn(Document* object_manager, entt::entity entity, entt::meta_any&& transaction_object);

    void Apply();
    void Clean() {};

private:
    Document* m_doc;
    entt::entity m_entity;
    entt::meta_any m_transaction_object;
};

class TransactionObjectAddDel
{
public:
    TransactionObjectAddDel(Document* object_manager, entt::entity entity, entt::meta_any&& new_object);

    void Apply();
    void Clean();

private:
    Document* m_doc;
    entt::entity m_entity;
    entt::meta_any m_new_object;
};

using TransactionObject = std::variant<TransactionObjectChn, TransactionObjectAddDel>;

BC_REGISTRATION(TransactionObjectChnImpl)
{
    BC_REGISTER_FUNCTION(Apply);
}

BC_REGISTRATION(TransactionObjectSubChn)
{
    BC_REGISTER_FUNCTION(Apply);
}

BC_REGISTRATION(TransactionObjectChn)
{
    BC_REGISTER_FUNCTION(Apply);
}

BC_REGISTRATION(TransactionObjectAddDel)
{
    BC_REGISTER_FUNCTION(Apply);
}
