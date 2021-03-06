#include "tool_add_vertex.hpp"
#include "common/polygon.hpp"
#include "imp/imp_interface.hpp"
#include <iostream>
#include "document/idocument.hpp"

namespace horizon {

ToolAddVertex::ToolAddVertex(IDocument *c, ToolID tid) : ToolBase(c, tid)
{
}

bool ToolAddVertex::can_begin()
{
    return std::count_if(selection.begin(), selection.end(),
                         [](const auto &x) { return x.type == ObjectType::POLYGON_EDGE; })
           == 1;
}

ToolResponse ToolAddVertex::begin(const ToolArgs &args)
{
    std::cout << "tool bend net line\n";

    int v = 0;
    for (const auto &it : args.selection) {
        if (it.type == ObjectType::POLYGON_EDGE) {
            poly = doc.r->get_polygon(it.uuid);
            v = it.vertex;
            break;
        }
    }

    if (!poly)
        return ToolResponse::end();

    auto v_next = (v + 1) % poly->vertices.size();
    vertex = &*poly->vertices.insert(poly->vertices.begin() + v_next, args.coords);
    selection.clear();
    selection.emplace(poly->uuid, ObjectType::POLYGON_VERTEX, v_next);
    imp->tool_bar_set_actions({
            {InToolActionID::LMB, "place vertex"},
            {InToolActionID::RMB, "cancel"},
    });
    return ToolResponse();
}
ToolResponse ToolAddVertex::update(const ToolArgs &args)
{
    if (args.type == ToolEventType::MOVE) {
        vertex->position = args.coords;
    }
    else if (args.type == ToolEventType::ACTION) {
        switch (args.action) {
        case InToolActionID::LMB:
            return ToolResponse::commit();

        case InToolActionID::RMB:
        case InToolActionID::CANCEL:
            selection.clear();
            return ToolResponse::revert();

        default:;
        }
    }
    return ToolResponse();
}
} // namespace horizon
