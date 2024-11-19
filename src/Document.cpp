#include "Document.h"

#include <Variant>

namespace
{
constexpr size_t max_undo_size{30};
constexpr size_t max_redo_size{30};
} // namespace

entt::meta_any* Document::GetMetaObject(entt::entity entity)
{
    return registry.try_get<entt::meta_any>(entity);
}

void Document::EraseObject(entt::entity entity, bool with_transaction)
{
    // 如果通过事务进行删除，则仅删除meta_any
    // 只有当其从Undo和Redo栈中排出时，才会真正执行删除
    if (with_transaction)
    {
        entt::meta_any* object{GetMetaObject(entity)};
        if (object == nullptr)
            return;

        CommitTransaciton();
        m_cur_transaction = std::make_unique<Transaction>();
        m_cur_transaction->PushTransactionObject(TransactionObjectAddDel{this, entity, std::move(*object)});
        RemoveComponent<entt::meta_any>(entity);
        PushUndoStack(std::move(m_cur_transaction));
        m_cur_transaction = nullptr;
    }
    else
    {
        registry.destroy(entity);
    }
}

void Document::OpenTransaction()
{
    CommitTransaciton();
    m_cur_transaction = std::make_unique<Transaction>();
}

void Document::CommitTransaciton()
{
    if (!m_cur_transaction)
        return;

    m_cur_transaction->Apply();
    PushUndoStack(std::move(m_cur_transaction));
    m_cur_transaction = nullptr;

    ClearRedo();
}

void Document::Undo()
{
    if (m_undo_stack.empty())
        return;

    m_undo_stack.top()->Apply();
    PushRedoStack(std::move(m_undo_stack.top()));
    m_undo_stack.pop();
}

void Document::Redo()
{
    if (m_redo_stack.empty())
        return;

    m_redo_stack.top()->Apply();
    PushUndoStack(std::move(m_redo_stack.top()));
    m_redo_stack.pop();
}

void Document::ClearRedo()
{
    while (!m_redo_stack.empty())
    {
        auto& transaction{m_redo_stack.top()};
        if (transaction != nullptr)
            transaction->Clean();
        m_redo_stack.pop();
    }
}

std::optional<TransactionObjectChn> Document::FromStrings(entt::entity entity, std::span<std::string> strings,
                                                          entt::meta_any&& change)
{
    if (strings.size() < 1)
        return std::nullopt;

    auto* object{GetMetaObject(entity)};
    if (!object)
        return std::nullopt;

    entt::meta_any cur_any{object->type().from_void(object)};
    if (!cur_any)
        return std::nullopt;

    auto generate_transaction_object{
        [&change](auto&& self, entt::meta_any pre_any, entt::meta_any cur_any,
                  std::span<std::string> remaining_strings) -> std::optional<TransactionObjectSubChn>
        {
            if (remaining_strings.empty())
            {
                TransactionObjectChnImpl transaction_object{std::move(cur_any), std::move(change)};
                return std::make_optional<TransactionObjectSubChn>(std::move(pre_any), std::move(transaction_object));
            }

            auto hash_value{entt::hashed_string{remaining_strings.front().data(), remaining_strings.front().length()}};
            entt::meta_any new_any{cur_any.get(hash_value)};

            if (!new_any)
                return std::nullopt;

            return self(self, std::move(cur_any), std::move(new_any),
                        std::span{remaining_strings.begin() + 1, remaining_strings.end()});
        }};

    auto transaction_object_opt{generate_transaction_object(
        generate_transaction_object, entt::meta_any{}, std::move(cur_any), std::span{strings.begin(), strings.end()})};

    if (!transaction_object_opt.has_value())
        return std::nullopt;

    return std::make_optional<TransactionObjectChn>(this, entity, std::move(transaction_object_opt.value()));
}

void Document::PushUndoStack(std::unique_ptr<Transaction> transaction)
{
    m_undo_stack.push(std::move(transaction));
    while (m_undo_stack.size() > max_undo_size)
    {
        m_undo_stack.top()->Clean();
        m_undo_stack.pop();
    }
}

void Document::PushRedoStack(std::unique_ptr<Transaction> transaction)
{
    m_redo_stack.push(std::move(transaction));
    while (m_redo_stack.size() > max_redo_size)
    {
        m_redo_stack.top()->Clean();
        m_redo_stack.pop();
    }
}
