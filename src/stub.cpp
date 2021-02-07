#include <iostream>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "math/3dmath.hpp"
#include "renderer/renderer.hpp"

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

    MjB::Renderer* renderer;

    renderer = new MjB::Renderer;
    renderer->setWidth(800);
    renderer->setHeight(600);
    renderer->init();

    renderer->setGUIUpdate([]() {
        ImGui::Begin("MyWindow");
        // ImGui::Checkbox("Boolean property", &this->someBoolean);
        if (ImGui::Button("Press Me")) {
            // This code is executed when the user clicks the button
            std::cout << "Button Pressed"
                      << "\n";
        }
        // ImGui::SliderFloat("Speed", &this->speed, 0.0f, 10.0f);
        ImGui::End();
    });

    bool running = true;

    SDL_Event currentEvent;

    std::vector<LineSegments> lineGroups;
    bool lineInProgress = false;

    while (running) {
        renderer->startFrame();
        renderer->clear({255, 255, 255, 255});

        while (SDL_PollEvent(&currentEvent)) {
            if (currentEvent.type == SDL_QUIT) {
                running = false;
                break;
            }

            if (not renderer->processEvent(currentEvent)) {
                switch (currentEvent.type) {
                    case SDL_KEYDOWN:
                    case SDL_KEYUP: {
                    } break;

                    case SDL_MOUSEBUTTONDOWN: {
                        switch (currentEvent.button.button) {
                            case SDL_BUTTON_LEFT: {
                                // if we don't have a line in progress,
                                // make it so we do
                                if (!lineInProgress) {
                                    lineGroups.emplace_back();
                                }
                                auto& x = currentEvent.button.x;
                                auto& y = currentEvent.button.y;
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

        renderer->endFrame();
        renderer->draw();
    }

    return 0;
}
