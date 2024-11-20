#pragma once

#include <pugixml.hpp>

#include "Document.h"

struct Serializer{
    static pugi::xml_document SaveMeta(Document* doc);
};
