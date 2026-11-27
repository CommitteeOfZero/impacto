#pragma once

#include <string>

#include <ankerl/unordered_dense.h>

namespace Impacto::Overlay {

void ShowAboutPage(
    std::string const& selectedGame,
    ankerl::unordered_dense::map<std::string, uint32_t> const& iconTextureMap,
    float imguiScale);
}
