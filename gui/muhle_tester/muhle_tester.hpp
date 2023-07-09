#pragma once

#include <gui_base.hpp>

struct MuhleTester : public gui_base::GuiApplication {
    virtual void start() override;
    virtual void update() override;
};
