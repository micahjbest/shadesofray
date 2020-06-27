#include <iostream>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "3dmath.h"
#include "renderer.h"

struct ScreenPoint {
    float x = 0.0f;
    float y = 0.0f;

    ScreenPoint() = default;

    ScreenPoint(float x$, float y$) : x(x$), y(y$) {}

    ScreenPoint(ScreenPoint const&) = default;
};

struct ScreenLine {
    ScreenPoint src;
    ScreenPoint dest;

    ScreenLine() = default;
    ScreenLine(ScreenPoint from, ScreenPoint to) : src(from), dest(to) {}
    ScreenLine(ScreenLine const&) = default;
};

using LineSegments = std::vector<ScreenPoint>;

int main(int argc, char const* argv[]) {

    MJB::Renderer* renderer;

    renderer = new MJB::Renderer;
    renderer->setWidth(800);
    renderer->setHeight(600);
    renderer->init();

    bool running = true;

    SDL_Event e;

    std::vector<LineSegments> lineGroups;
    bool lineInProgress = false;

    while (running) {

        renderer->clear({255, 255, 255, 255});

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT: {
                    running = false;
                } break;

                case SDL_KEYDOWN:
                case SDL_KEYUP: {
                } break;

                case SDL_MOUSEBUTTONDOWN: {
                    switch (e.button.button) {
                        case SDL_BUTTON_LEFT: {
                            // if we don't have a line in progress,
                            // make it so we do
                            if (!lineInProgress) {
                                lineGroups.emplace_back();
                            }
                            auto& x = e.button.x;
                            auto& y = e.button.y;
                            lineGroups.back().emplace_back(x, y);

                            lineInProgress = true;

                        } break;
                        case SDL_BUTTON_RIGHT: {
                            // interupt any line in progress
                            lineInProgress = false;

                            // if we have a single point line, remove it
                            if (lineGroups.back().size() == 1) {
                                lineGroups.pop_back();
                            }

                        } break;
                    }
                } break;

                case SDL_MOUSEBUTTONUP: {
                } break;

                default: {
                    // just ignore this
                } break;
            }
        }

        // draw all of our lines
        uint8_t R = 100;
        uint8_t G = 100;
        uint8_t B = 255;

        size_t groupCount = 0;
        for (auto& lineGroup : lineGroups) {
            // this will skip line groups that don't have more than one point
            for (size_t pointIndex = 1; pointIndex < lineGroup.size();
                 ++pointIndex) {
                auto& x1 = lineGroup[pointIndex - 1].x;
                auto& y1 = lineGroup[pointIndex - 1].y;
                auto& x2 = lineGroup[pointIndex].x;
                auto& y2 = lineGroup[pointIndex].y;

                R = static_cast<uint8_t>((R + (20 * groupCount)) % 256);
                G = 255 - R;

                renderer->drawLineAA(x1, y1, x2, y2, {R, G, B});
            }
            ++groupCount;
        }

        renderer->draw();
    }

    return 0;
}
