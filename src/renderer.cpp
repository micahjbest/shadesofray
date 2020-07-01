//
// renderer.cpp
//
//
//  Created by Micah J Best on 2017-01-07.
//  Copyright © 2017 Micah J Best. All rights reserved.
//

#include "renderer.h"

#include <cmath>
#include <cstring>

namespace MJB {

ImageHandle Renderer::addNewImage(ImageRGBA&& image) {
    ownedImages.emplace_back(std::move(image));
    // expand the image cache
    imageScaleCache.emplace_back();

    return ownedImages.size() - 1;
}

ImageRGBA& Renderer::lookupImage(ImageHandle handle) {
    return ownedImages[handle];
}

ImageRGBA& Renderer::lookupImage(ImageHandle handle, uint32_t sizeX,
                                 uint32_t sizeY) {
    auto& baseImage = lookupImage(handle);
    // see if we're being scaled
    if (baseImage.sizeX() != sizeX or baseImage.sizeY() != sizeY) {
        // see if we've got this in cache
        auto scaleFind = imageScaleCache[handle].find({sizeX, sizeY});
        if (scaleFind != imageScaleCache[handle].end()) {
            // we've got it, return it
            return scaleFind->second;
        }

        // not found -- so we have to scale it
        auto [iterator, inserted] = imageScaleCache[handle].emplace(
            std::pair(std::pair{sizeX, sizeY},
                      std::move(resizeImage(baseImage, sizeX, sizeY))));

        return iterator->second;
    } else {
        return baseImage;
    }
}

/****
 * Helper functions for resizeImage
 **/
// project from dest to src
float projectDestToSrc(float srcSpan, float destSpan, float point) {
    auto normalizedPoint = point / destSpan;
    auto projectedPoint = normalizedPoint * srcSpan;
    return projectedPoint;
}

PixelRGBA averagePixels(PixelRGBA const& first, PixelRGBA const& second,
                        float amountOfFirst = 0.5) {
    float amountOfSecond = 1 - amountOfSecond;

    std::array<uint8_t, 4> averagedChannels;

    auto firstAsArray = (std::array<uint8_t, 4>)first;
    auto secondAsArray = (std::array<uint8_t, 4>)second;

    for (size_t i = 0; i < 4; ++i) {
        uint16_t total = (uint16_t)firstAsArray[i] + (uint16_t)secondAsArray[i];
        uint8_t average = (uint8_t)(total / 2);
        averagedChannels[i] = average;
    }

    return PixelRGBA(averagedChannels, PremultipliedAlpha());
}

PixelRGBA samplePixelInterpolate(ImageRGBA const& image, float locX,
                                 float locY) {
    // NOTE: for now, just do a linear interpolation between the sampled pixel
    // and its neighbour.. this could be a lot better
    // TODO: make it better

    // find our integer coordinates with a simple 'floor'
    int posX = (int)locX;
    int posY = (int)locY;

    PixelRGBA xPixel = image.getPixel(posX, posY);

    // see if we're looking to the left or right
    float xOffset = locX - (float)posX;

    if (xOffset < 0.5) {
        // if we're not at the boundary
        if (posX != 0) {
            // get our neighbour
            auto adjacentPixel = image.getPixel(posX - 1, posY);
            // find the difference from the neighbor's center
            float distance = 0.5 + xOffset;

            // average the two by the given factor
            xPixel = averagePixels(xPixel, adjacentPixel, distance);
        }
    } else {
        // see if we're not at the boundary
        if (posX < image.sizeX()) {
            // get our neighbour
            auto adjacentPixel = image.getPixel(posX + 1, posY);
            // find the difference from the neighbor's center
            float distance = 1.5 - xOffset;

            // average the two by the given factor
            xPixel = averagePixels(xPixel, adjacentPixel, distance);
        }
    }

    PixelRGBA yPixel = image.getPixel(posX, posY);

    // see if we're looking up or down
    float yOffset = locY - (float)posY;

    if (xOffset < 0.5) {
        // if we're not at the boundary
        if (posY != 0) {
            // get our neighbour
            auto adjacentPixel = image.getPixel(posX, posY - 1);
            // find the difference from the neighbor's center
            float distance = 0.5 + yOffset;

            // average the two by the given factor
            yPixel = averagePixels(yPixel, adjacentPixel, distance);
        }
    } else {
        // see if we're not at the boundary
        if (posY < image.sizeY()) {
            // get our neighbour
            auto adjacentPixel = image.getPixel(posX, posY + 1);
            // find the difference from the neighbor's center
            float distance = 1.5 - yOffset;

            // average the two by the given factor
            yPixel = averagePixels(yPixel, adjacentPixel, distance);
        }
    }

    // get our averaged pixel
    auto averagedDifference = averagePixels(xPixel, yPixel);
    // just mix in the directly sampled pixel
    return averagePixels(averagedDifference, image.getPixel(posX, posY));
}

ImageRGBA Renderer::resizeImage(ImageRGBA const& image, int newSizeX,
                                int newSizeY) {
    // NOTE: this is quick and dirty and not very good... needs improvement

    // create the new image
    ImageRGBA newImage(newSizeX, newSizeY);

    // figure out how many points we'll be sampling
    auto destSrcRatioX = (float)image.sizeX() / (float)newImage.sizeX();
    auto destSrcRatioY = (float)image.sizeY() / (float)newImage.sizeY();

    // see how many pixels we need to sample
    // we will always sample at least one pixel
    int samplePixelX = (int)ceil(destSrcRatioX);
    int samplePixelY = (int)ceil(destSrcRatioY);

    // make our sampling buffer
    std::vector<std::array<uint8_t, 4>> samples;
    samples.reserve(samplePixelX * samplePixelY);

    for (int destPosY = 0; destPosY < newImage.sizeY(); ++destPosY) {
        for (int destPosX = 0; destPosX < newImage.sizeX(); ++destPosX) {
            if (samplePixelX == 1) {
                // just sample corresponding pixel in 'center'
                float locX = projectDestToSrc((float)image.sizeX(),
                                              (float)newImage.sizeX(),
                                              (float)destPosX + 0.5);

                float locY = projectDestToSrc((float)image.sizeY(),
                                              (float)newImage.sizeY(),
                                              (float)destPosY + 0.5);
                newImage.setPixel(destPosX, destPosY,
                                  samplePixelInterpolate(image, locX, locY));
            } else {
                // start with the bounds of this pixel projected onto the source
                float srcMinX =
                    projectDestToSrc((float)image.sizeX(),
                                     (float)newImage.sizeX(), (float)destPosX);
                float srcMaxX = projectDestToSrc((float)image.sizeX(),
                                                 (float)newImage.sizeX(),
                                                 ((float)destPosX) + 0.999);
                float srcMinY =
                    projectDestToSrc((float)image.sizeY(),
                                     (float)newImage.sizeY(), (float)destPosY);
                float srcMaxY = projectDestToSrc((float)image.sizeY(),
                                                 (float)newImage.sizeY(),
                                                 ((float)destPosY) + 0.999);
                // figure out our sampling interval
                float srcSpanX = srcMaxX - srcMinX;
                float srcSpanY = srcMaxY - srcMinY;

                float intervalX = srcSpanX / (float)samplePixelX;
                float intervalY = srcSpanY / (float)samplePixelY;

                int samplesTaken = 0;

                float locX = srcMinX;
                float locY = srcMinY;

                samples.clear();

                for (int sampleNumY = 0; sampleNumY < samplePixelY;
                     ++sampleNumY) {

                    if ((int)locY >= image.sizeY())
                        break;
                    if (locY >= 0.0) {
                        for (int sampleNumX = 0; sampleNumX < samplePixelX;
                             ++sampleNumX) {

                            if ((int)locX >= image.sizeY())
                                break;

                            if (locX >= 0.0) {
                                auto pixel =
                                    samplePixelInterpolate(image, locX, locY);
                                samples.push_back(
                                    (std::array<uint8_t, 4>)pixel);
                            }
                            locX += intervalX;
                        }
                    }
                    locX = srcMinX;
                    locY += intervalY;
                }

                std::array<uint8_t, 4> resultPixel = {0, 0, 0, 0};
                for (int channel = 0; channel < 4; ++channel) {
                    uint32_t total = 0;
                    for (auto& sample : samples) {
                        total += sample[channel];
                    }
                    auto floatResult = (float)total / (float)samples.size();
                    resultPixel[channel] = (uint8_t)(round(floatResult));
                }
                newImage.setPixel(destPosX, destPosY,
                                  PixelRGBA(resultPixel, PremultipliedAlpha()));
            }
        }
    }
    return newImage;
}

void Renderer::drawImage(int destOriginX, int destOriginY,
                         ImageRGBA const& image) {

    if (destOriginX >= screenWidth or destOriginY >= screenHeight)
        return;

    int srcFirstX;
    int destFirstX;
    if (destOriginX < 0) {
        srcFirstX = -1 * destOriginX;
        destFirstX = 0;
    } else {
        srcFirstX = 0;
        destFirstX = destOriginX;
    }

    int srcFirstY;
    int destFirstY;
    if (destOriginY < 0) {
        srcFirstY = -1 * destOriginY;
        destFirstY = 0;
    } else {
        srcFirstY = 0;
        destFirstY = destOriginY;
    }

    int destAfterLastX = image.sizeX() + destOriginX;
    int srcAfterLastX = image.sizeX();
    if (destAfterLastX > screenWidth) {
        srcAfterLastX -= (destAfterLastX - screenWidth);
    }

    int destAfterLastY = image.sizeY() + destOriginY;
    int srcAfterLastY = image.sizeY();
    if (destAfterLastY > screenHeight) {
        srcAfterLastY -= (destAfterLastY - screenHeight);
    }

    if (srcAfterLastX <= 0 or srcAfterLastY <= 0)
        return;

    int destPosX = destFirstX;
    int destPosY = destFirstY;
    for (int srcPosY = srcFirstY; srcPosY < srcAfterLastY; ++srcPosY) {
        for (int srcPosX = srcFirstX; srcPosX < srcAfterLastX; ++srcPosX) {

            addPixel(destPosX, destPosY, image.getPixel(srcPosX, srcPosY));

            ++destPosX;
        }
        destPosX = destFirstX;
        ++destPosY;
    }
}

void Renderer::init() {

    // Initialize the video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        error(std::string("SDL could not initialize! SDL Error: ") +
              SDL_GetError());
    }

    std::cout << "screen dims: " << screenWidth << ", " << screenHeight << "\n";

#ifdef USE_SDL_RENDERER
    // Create the window
    SDLWindow = SDL_CreateWindow(
        applicationName.c_str(), SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);

    SDLRenderer = SDL_CreateRenderer(
        SDLWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (SDLWindow == NULL) {
        error(std::string("Window could not be created! SDL Error: ") +
              SDL_GetError());
    } else {
        // Get window surface
        screenSurface = SDL_GetWindowSurface(SDLWindow);
    }

    // Setup the destination for direct pixel drawing
    pixelTexture = SDL_CreateTexture(SDLRenderer, SDL_PIXELFORMAT_RGBA32,
                                     SDL_TEXTUREACCESS_STREAMING, screenWidth,
                                     screenHeight);

    SDL_SetTextureBlendMode(pixelTexture, SDL_BLENDMODE_NONE);

#else
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDLWindow = SDL_CreateWindow(
        applicationName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    renderingContext = SDL_GL_CreateContext(SDLWindow);

    pixelData = new PixelRGBA[screenWidth * screenHeight];

    glGenTextures(1, &pixelTextureID);
    glBindTexture(GL_TEXTURE_2D, pixelTextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth, screenHeight, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glGenFramebuffers(1, &readFBOId);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, readFBOId);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, pixelTextureID, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    // window is the SDL_Window*
    // contex is the SDL_GLContext
    ImGui_ImplSDL2_InitForOpenGL(SDLWindow, renderingContext);
    ImGui_ImplOpenGL3_Init();

#endif // USE_SDL_RENDERER

    auto frameTimingStart = std::chrono::high_resolution_clock::now();
}

void Renderer::draw() {
#ifdef USE_SDL_RENDERER
    SDL_RenderCopy(SDLRenderer, pixelTexture, NULL, NULL);
    SDL_RenderPresent(SDLRenderer);
#else

    /****
     * IMGUI stuff
     **/

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(SDLWindow);
    ImGui::NewFrame();

    ImGui::Begin("MyWindow");
    // ImGui::Checkbox("Boolean property", &this->someBoolean);
    if (ImGui::Button("Press Me")) {
        // This code is executed when the user clicks the button
        std::cout << "Button Pressed"
                  << "\n";
    }
    // ImGui::SliderFloat("Speed", &this->speed, 0.0f, 10.0f);
    ImGui::End();

    /****
     * openGL stuff
     **/

    glBindTexture(GL_TEXTURE_2D, pixelTextureID);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, screenWidth, screenHeight, GL_RGBA,
                    GL_UNSIGNED_BYTE, pixelData);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, readFBOId);
    glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth,
                      screenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(SDLWindow);

#endif // USE_SDL_RENDERER

    framesTimed++;

    auto frameTimingStop = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        frameTimingStop - frameTimingStart);
    auto elapsedTicks = elapsedTime.count();

    if (elapsedTicks > 1000) {
        float fps = ((float)framesTimed) / (((float)elapsedTicks) / 1000.0);
        std::cout << "FPS: " << fps << '\n';
        framesTimed = 0;
        frameTimingStart = std::chrono::high_resolution_clock::now();
    }
}

void Renderer::shutdown() {
#ifndef USE_SDL_RENDERER
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(renderingContext);
#endif // USE_SDL_RENDERER
    // Destroy our resources, if we got to creating them
    if (SDLWindow) {
        SDL_DestroyWindow(SDLWindow);
    }

    if (SDLRenderer) {
        SDL_DestroyRenderer(SDLRenderer);
    }

#ifdef USE_SDL_RENDERER
    if (pixelTexture) {
        SDL_DestroyTexture(pixelTexture);
    }
#endif
}

void Renderer::startFrame() {
#ifdef USE_SDL_RENDERER
    SDL_LockTexture(pixelTexture, NULL, &directPixels, &directPixelsPitch);
#endif // USE_SDL_RENDERER
}

void Renderer::endFrame() {
#ifdef USE_SDL_RENDERER
    SDL_UnlockTexture(pixelTexture);
#endif // USE_SDL_RENDERER
}

bool Renderer::processInput(SDL_Event const& event) {
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplSDL2_ProcessEvent(&event);

    return io.WantCaptureKeyboard || io.WantCaptureMouse;
}

void Renderer::drawLine(int x1, int y1, int x2, int y2, PixelRGBA p) {
    int dx = abs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        drawPixel(x1, y1, p);
        e2 = 2 * err;
        if (e2 >= dy) {
            if (x1 == x2)
                break;
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            if (y1 == y2)
                break;
            err += dx;
            y1 += sy;
        }
    }
}

PixelRGBA Renderer::getPixel(int const& x, int const& y, ImageHandle handle) {
    auto& image = lookupImage(handle);

    auto pixel = image.getPixel(x, y);

    return pixel;
}

void Renderer::drawLineAA(int x1, int y1, int x2, int y2, PixelRGBA p) {
    int dx = abs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = abs(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int xt;
    int e2;
    int err = dx - dy;
    int ed = dx + dy == 0 ? 1 : sqrt((float)dx * dx + (float)dy * dy);

    while (1) {
        uint8_t alpha = p.A() * (1.0 - (float)abs(err - dx + dy) / (float)ed);
        drawPixel(x1, y1, {p.R(), p.G(), p.B(), alpha});

        e2 = err;
        xt = x1;

        if (2 * e2 >= -dx) { /* x step */
            if (x1 == x2)
                break;

            if (e2 + dy < ed) {
                uint8_t alpha = p.A() * (1.0 - (float)abs(e2 + dy) / (float)ed);
                drawPixel(x1, y1 + sy, {p.R(), p.G(), p.B(), alpha});
            }

            err -= dy;
            x1 += sx;
        }

        if (2 * e2 <= dy) { /* y step */
            if (y1 == y2)
                break;
            if (dx - e2 < ed) {
                uint8_t alpha = p.A() * (1.0 - (float)(dx - e2) / (float)ed);
                drawPixel(xt + sx, y1, {p.R(), p.G(), p.B(), alpha});
            }

            err += dx;
            y1 += sy;
        }
    }
}

void Renderer::drawLineAA(int x0, int y0, int x1, int y1, float wd,
                          PixelRGBA p) {

    /* plot an anti-aliased line of width wd */
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    int e2, x2, y2; /* error value e_xy */
    float ed = dx + dy == 0 ? 1 : sqrt((float)dx * dx + (float)dy * dy);
    for (wd = (wd + 1) / 2;;) { /* pixel loop */
        uint8_t alpha =
            p.A() *
            (1 - fmax(0.0, ((float)abs(err - dx + dy) / (float)(ed)) - wd + 1));

        drawPixel(x0, y0, {p.R(), p.G(), p.B(), alpha});
        e2 = err;
        x2 = x0;
        if (2 * e2 >= -dx) { /* x step */
            for (e2 += dy, y2 = y0; e2 < ed * wd && (y1 != y2 || dx > dy);
                 e2 += dx) {
                uint8_t alpha =
                    p.A() *
                    (1 - fmax(0.0, ((float)abs(e2) / (float)(ed)) - wd + 1));
                drawPixel(x0, y2 += sy, {p.R(), p.G(), p.B(), alpha});
            }

            if (x0 == x1)
                break;
            e2 = err;
            err -= dy;
            x0 += sx;
        }
        if (2 * e2 <= dy) { /* y step */
            for (e2 = dx - e2; e2 < ed * wd && (x1 != x2 || dx < dy);
                 e2 += dy) {
                uint8_t alpha =
                    p.A() *
                    (1 - fmax(0.0, ((float)abs(e2) / (float)(ed)) - wd + 1));
                drawPixel(x2 += sx, y0, {p.R(), p.G(), p.B(), alpha});
            }

            if (y0 == y1)
                break;
            err += dx;
            y0 += sy;
        }
    }
}

void Renderer::drawRectangle(int x1, int y1, int x2, int y2, PixelRGBA p) {
    if (x2 < x1) {
        auto temp = x1;
        x1 = x2;
        x2 = temp;
    }

    for (auto x = x1; x <= x2; ++x) {
        for (auto y = y1; y <= y2; ++y) {
            drawPixel(x, y, p);
        }
    }
}

void Renderer::clear(PixelRGBA const& p) {
    for (int i = 0; i < screenHeight * screenWidth; ++i) {
        getPixelBuffer()[i] = p;
    }
}

std::pair<int, int> Renderer::getImageDimensions(ImageHandle handle) {
    auto& image = ownedImages[handle];
    return {image.sizeX(), image.sizeY()};
}

void Renderer::drawImage(int x1, int y1, ImageHandle handle) {
    drawImage(x1, y1, lookupImage(handle));
}

void Renderer::drawImage(int x1, int y1, int x2, int y2, ImageHandle handle) {
    auto sizeX = x2 - x1 + 1;
    auto sizeY = y2 - y1 + 1;
    drawImage(x1, y1, lookupImage(handle, sizeX, sizeY));
}

ImageHandle Renderer::loadImage(std::string const& filename) {
    auto image = readImageRGBA(filename);
    return addNewImage(std::move(image));
}

void Renderer::saveImage(ImageHandle handle, std::string const& filename,
                         bool sequential) {
    writeImageRGBA(lookupImage(handle), filename, sequential);
}

} // namespace MJB
