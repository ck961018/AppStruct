#pragma once

#include <stack>

#include "Transaction.h"

class Document
{
public:
    template <typename T>
    entt::entity AddObject(T&& object, bool with_transaction = true)
    {
        auto entity{registry.create()};
        registry.emplace<entt::meta_any>(entity, std::forward<T>(object));

        // 手动执行事务以减少移动开销
        if (with_transaction)
        {
            CommitTransaciton();
            m_cur_transaction = std::make_unique<Transaction>();
            m_cur_transaction->PushTransactionObject(TransactionObjectAddDel{this, entity, nullptr});
            PushUndoStack(std::move(m_cur_transaction));
            m_cur_transaction = nullptr;
            ClearRedo();
        }
        return entity;
    }

    template <typename T>
    T* GetObject(entt::entity entity)
    {
        auto any{registry.try_get<entt::meta_any>(entity)};
        if (any == nullptr)
            return nullptr;
        return any->try_cast<T>();
    }

    template <typename... Ts>
    void EmplaceComponent(entt::entity entity, Ts&&... args)
    {
        registry.emplace_or_replace<Ts...>(entity, std::forward<Ts>(args)...);
    }

    template <typename... Ts>
    size_t RemoveComponent(entt::entity entity)
    {
        return registry.remove<Ts...>(entity);
    }

    entt::meta_any* GetMetaObject(entt::entity entity);

    void EraseObject(entt::entity entity, bool with_transaction = true);

    void OpenTransaction();

    void CommitTransaciton();

    void Undo();

    void Redo();

    void ClearRedo();

    template <typename T>
    bool ChangePropertyWithNewTransaciton(entt::entity entity, std::span<std::string> cmds, T&& change)
    {
        auto opt{FromStrings(entity, cmds, std::forward<T>(change))};
        if (!opt.has_value())
            return false;

        OpenTransaction();
        m_cur_transaction->PushTransactionObject(std::move(*opt));
        CommitTransaciton();
        return true;
    }

// private:
    std::optional<TransactionObjectChn> FromStrings(entt::entity entity, std::span<std::string> strings,
                                                    entt::meta_any&& change);

    void PushUndoStack(std::unique_ptr<Transaction> transaction);
    void PushRedoStack(std::unique_ptr<Transaction> transaction);

    std::stack<std::unique_ptr<Transaction>> m_undo_stack;
    std::stack<std::unique_ptr<Transaction>> m_redo_stack;
    std::unique_ptr<Transaction> m_cur_transaction{nullptr};

    entt::registry registry;
};
