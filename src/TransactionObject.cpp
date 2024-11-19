#include "TransactionObject.h"

#include <entt/entt.hpp>
#include <print>

#include "Document.h"

using namespace entt::literals;

BC_DO_REGISTRATION(TransactionObjectChnImpl);
BC_DO_REGISTRATION(TransactionObjectSubChn);
BC_DO_REGISTRATION(TransactionObjectChn);
BC_DO_REGISTRATION(TransactionObjectAddDel);

TransactionObjectChnImpl::TransactionObjectChnImpl(entt::meta_any&& old_property, entt::meta_any&& new_property)
    : m_old_property{std::move(old_property)}, m_new_property{std::move(new_property)}
{
}

void TransactionObjectChnImpl::Apply()
{
    if (m_old_property == nullptr)
        return;

    auto tmp{m_old_property.type().construct()};
    if (!tmp)
        return;

    tmp.assign(m_new_property);
    m_new_property.assign(m_old_property);
    m_old_property.assign(tmp);
}

TransactionObjectSubChn::TransactionObjectSubChn(entt::meta_any&& object, entt::meta_any&& transaction_object)
    : m_object{std::move(object)}, m_transaction_object{std::move(transaction_object)}
{
}

void TransactionObjectSubChn::Apply()
{
    m_object.invoke("OnBeforeChange"_hs);
    m_transaction_object.invoke("Apply"_hs);
    m_object.invoke("OnChanged"_hs);
}

TransactionObjectChn::TransactionObjectChn(Document* doc, entt::entity entity, entt::meta_any&& transaction_object)
    : m_doc{doc}, m_entity{entity}, m_transaction_object{std::move(transaction_object)}
{
}

void TransactionObjectChn::Apply()
{
    if (m_doc == nullptr || m_doc->GetMetaObject(m_entity) == nullptr || m_transaction_object == nullptr)
        return;

    m_transaction_object.invoke("Apply"_hs);
}

TransactionObjectAddDel::TransactionObjectAddDel(Document* doc, entt::entity entity, entt::meta_any&& new_object)
    : m_doc{doc}, m_entity{entity}, m_new_object{std::move(new_object)}
{
}

void TransactionObjectAddDel::Apply()
{
    if (m_doc == nullptr)
        return;

    if (m_new_object == nullptr)
    {
        entt::meta_any* old_object{m_doc->GetMetaObject(m_entity)};
        if (old_object == nullptr)
            return;

        m_new_object = std::move(*old_object);
        m_doc->RemoveComponent<entt::meta_any>(m_entity);
    }
    else
    {
        m_doc->EmplaceComponent<entt::meta_any>(m_entity, std::move(m_new_object));
        m_new_object = nullptr;
    }
}

void TransactionObjectAddDel::Clean()
{
    // 延迟删除
    if (m_new_object != nullptr)
        m_doc->EraseObject(m_entity, false);
}
