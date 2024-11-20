#include "Serializer.h"

#include "RTTI.hpp"

namespace
{
bool AppendToNode(pugi::xml_node& node, entt::meta_any const& any)
{
    if (!any)
        return false;

    entt::meta_type const& type{any.type()};

    // id与哈希值不等则为自定义类型
    if (type.id() != type.info().hash())
    {
        std::string_view type_name{};
        size_t size{TypeNameMap::Instance().visit(type.id(),
                                                  [&type_name](auto const& pair)
                                                  {
                                                      type_name = pair.second;
                                                  })};
        if (size == 0)
            return false;

        std::println("{}", type_name);
        node.set_name(type_name.data(), type_name.length());

        // 如果自定义了序列化函数，则直接调用
        if (any.invoke("Serialize"_hs, node))
            return true;

        std::ranges::for_each(type.data(),
                              [&](std::pair<entt::id_type, entt::meta_data> const& pair)
                              {
                                  entt::meta_any sub_any{any.get(pair.first)};
                                  pugi::xml_node sub_node{node.append_child()};
                                  AppendToNode(sub_node, sub_any);
                              });
    }
    return true;
}
} // namespace

pugi::xml_document Serializer::SaveMeta(Document* doc)
{
    pugi::xml_document xml_doc;

    auto view{doc->View<entt::entity, entt::meta_any>()};
    std::ranges::for_each(view.each(),
                          [&](std::tuple<entt::entity, entt::meta_any> const& tuple)
                          {
                              auto const& [entity, any]{tuple};
                              std::println("{}", any.type().info().name());
                              pugi::xml_node node{xml_doc.append_child()};
                              AppendToNode(node, any);
                          });
    return xml_doc;
}
