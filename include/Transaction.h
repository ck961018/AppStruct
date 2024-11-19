#pragma once

#include "TransactionObject.h"

#include <print>

struct Transaction
{
public:
    void PushTransactionObject(TransactionObject&& transaction_object)
    {
        m_transaction_objects.emplace_back(std::move(transaction_object));
    }

    void Apply()
    {
        auto apply{[](TransactionObject& transaction_object)
                   {
                       std::visit(
                           [](auto&& transaction_object)
                           {
                               transaction_object.Apply();
                           },
                           transaction_object);
                   }};

        if (undo)
            std::ranges::for_each(m_transaction_objects.rbegin(), m_transaction_objects.rend(), apply);
        else
            std::ranges::for_each(m_transaction_objects, apply);

        undo = !undo;
    }

    void Clean()
    {
        auto clean{[](TransactionObject& transaction_object)
                   {
                       std::visit(
                           [](auto&& transaction_object)
                           {
                               transaction_object.Clean();
                           },
                           transaction_object);
                   }};

        if (undo)
            std::ranges::for_each(m_transaction_objects.rbegin(), m_transaction_objects.rend(), clean);
        else
            std::ranges::for_each(m_transaction_objects, clean);
    }

private:
    std::vector<TransactionObject> m_transaction_objects;
    bool undo{false};
};
