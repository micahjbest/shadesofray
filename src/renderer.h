//
// renderer.h
//
//
//  Created by Micah J Best on 2017-01-07.
//  Copyright © 2017 Micah J Best. All rights reserved.
//

#ifndef MJB_RENDERER
#define MJB_RENDERER

#include <iostream>
#include <map>
#include <string>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

#include "image.h"
#include "pixel.h"

#include "SDL.h"

namespace MJB {

// Data type for refering to a loaded texture
using ImageHandle = int;

/**************************************************************************/
/**
 * @name Renderer
 * @brief Wrap some basic SDL functions in a sorta clean OO way
 *
 */
class Renderer {

    int screenWidth = 1024; /**<window width in pixels (default: 1024)*/
    int screenHeight = 768; /**<window height in pixels (default: 768)*/

    SDL_Renderer* SDLRenderer = nullptr; /**< SDL renderer abstraction */
    SDL_Window* SDLWindow = nullptr; /**< The window we'll be rendering to */
    SDL_Surface* screenSurface =
        nullptr; /**< The surface contained by the window */

    SDL_Texture* pixelTexture =
        nullptr; /**< texture for pixel by pixel drawing  */
    SDL_Surface* pixelSurface =
        nullptr; /**< raw pixels to write to for texture */

    std::string applicationName = "Shades of Ray    ";

    std::vector<ImageRGBA> ownedImages;

    std::vector<std::map<std::pair<int, int>, ImageRGBA>> imageScaleCache;

    std::map<std::string, ImageHandle> imageNameToHandle; /**< image names */

    // uint32_t frameTimingStart;
    std::chrono::time_point<std::chrono::high_resolution_clock>
        frameTimingStart;

    int framesTimed = 0;

    // abstraction to get the current pixel buffer
    PixelRGBA* getPixelBuffer() {
        return (PixelRGBA*)pixelSurface->pixels;
    }

    // abstraction to get the current pixel buffer
    const PixelRGBA* getPixelBuffer() const {
        return (PixelRGBA*)pixelSurface->pixels;
    }

    // add image to registry of images and give it a handle
    ImageHandle addNewImage(ImageRGBA&& image);

    /**************************************************************************/
    /**
     * @brief  Retreive the actual image associated with a given handle
     *
     * @param handle The handle for the image
     *
     * @return Reference to the assocaited image
     *
     */
    ImageRGBA& lookupImage(ImageHandle handle);

    /**************************************************************************/
    /**
     * @brief  Retreive the actual image associated with a given handle and
     *         a specific size (will create a new entry in the scale cache if
     *         that size hasn't been created before)
     *
     * @param handle The handle for the image
     *
     * @return Reference to the assocaited image
     *
     */
    ImageRGBA& lookupImage(ImageHandle handle, uint32_t sizeX, uint32_t sizeY);

    /**************************************************************************/
    /**
     * @brief  Produce a copy of an image with a new size
     *
     * @param image Image to resize
     * @param newSizeX new x dimension
     * @param newSizeX new x dimension
     *
     * @return newly created scaled image
     *
     */
    ImageRGBA resizeImage(ImageRGBA const& image, int newSizeX, int newSizeY);

    /**************************************************************************/
    /**
     * @param destOriginX upper left x coordinate
     * @param destOriginY upper left y coordinate
     * @param image Image to draw
     *
     */
    void drawImage(int destOriginX, int destOriginY, ImageRGBA const& image);

  public:
    /**************************************************************************/
    /**
     * @brief  Constructor
     *
     */
    Renderer() {}

    /**************************************************************************/
    /**
     * @brief  Sets the window/screen width
     *
     * @param  width New width
     *
     */
    Renderer& setWidth(int width) {
        screenWidth = width;
        return *this;
    }

    /**************************************************************************/
    /**
     * @brief  Sets the window/screen height
     *
     * @param  width New width
     *
     */
    Renderer& setHeight(int height) {
        screenHeight = height;
        return *this;
    }

    /**************************************************************************/
    /**
     * @brief  Retreive the x, y dimensions of the rendering target
     *
     * @return pair of integers representing the dimensions
     *
     */
    std::pair<int, int> getScreenDimensions() {
        return {screenWidth, screenHeight};
    }

    /**************************************************************************/
    /**
     * @brief  Initialize the renderer
     *
     */
    void init();

    /**************************************************************************/
    /**
     * @brief Shut down the renderer and clean up
     *
     */
    void shutdown();

    /**************************************************************************/
    /**
     * @brief  Draw a single pixel
     *
     * @param x x coordinate
     * @param y y coordinate
     * @param p Pixel to draw
     *
     */
    inline void drawPixel(int const& x, int const& y, PixelRGBA const& p) {
        // if( x > 0 and y > 0 and x < screenWidth and y < screenHeight ) {
        // blend with (or replace) the current pixel
        getPixelBuffer()[y * screenWidth + x] += p;
        //}
    }

    /**************************************************************************/
    /**
     * @brief  Retreive a single pixel
     *
     * @param x x coordinate
     * @param y y coordinate
     * @param p  pixel to use for drawing
     *
     * @return  the pixel at the current location
     *
     */
    PixelRGBA const& getPixel(int const& x, int const& y) const {
        return getPixelBuffer()[y * screenWidth + x];
    }

    /**************************************************************************/
    /**
     * @brief  Set a single pixel to the render target
     *
     * @param x x coordinate
     * @param y y coordinate
     * @param p pixel to use for drawing
     *
     * @return  the pixel at the current location
     *
     */
    void setPixel(int const& x, int const& y, PixelRGBA const& p) {
        // if( x >= 0 and x < screenWidth and y >= 0 and y < screenHeight ) {
        getPixelBuffer()[y * screenWidth + x] = p;
        //}
    }

    /**************************************************************************/
    /**
     * @brief  Add a single pixel to the render target
     *
     * @param x x coordinate
     * @param y y coordinate
     * @param p  pixel to use for drawing
     *
     * @return  the pixel at the current location
     *
     */
    void addPixel(int const& x, int const& y, PixelRGBA const& p) {
        // if( x >= 0 and x < screenWidth and y >= 0 and y < screenHeight ) {
        getPixelBuffer()[y * screenWidth + x] += p;
        //}
    }

    /**************************************************************************/
    /**
     * @brief  Retreive a single pixel from a given image
     *
     * @param x x coordinate
     * @param y y coordinate
     *
     * @return  the pixel at the current location
     *
     */
    PixelRGBA getPixel(int const& x, int const& y, ImageHandle handle);

    /**************************************************************************/
    /**
     * @brief  Draw a line
     *
     * @param x1 starting x coordinate
     * @param y1 starting y coordinate
     * @param x2 ending x coordinate
     * @param y2 ending y coordinate
     * @param p  pixel to use for drawing
     *
     */
    void drawLine(int x1, int y1, int x2, int y2, PixelRGBA p);

    /**************************************************************************/
    /**
     * @brief  Draw a line - with anti-aliasing
     *
     * @param x1 starting x coordinate
     * @param y1 starting y coordinate
     * @param x2 ending x coordinate
     * @param y2 ending y coordinate
     * @param p  pixel to use for drawing
     *
     */
    void drawLineAA(int x1, int y1, int x2, int y2, PixelRGBA p);

    /**************************************************************************/
    /**
     * @brief  Draw a line - with anti-aliasing (and width)
     *
     * @param x0 starting x coordinate
     * @param y0 starting y coordinate
     * @param x1 ending x coordinate
     * @param y1 ending y coordinate
     * @param p  pixel to use for drawing
     *
     */
    void drawLineAA(int x0, int y0, int x1, int y1, float wd, PixelRGBA p);

    /**************************************************************************/
    /**
     * @brief  Draw a rectangle
     *
     * @param x1 upper left x coordinate
     * @param y1 upper left y coordinate
     * @param x2 lower right x coordinate
     * @param y2 lower right y coordinate
     * @param p  pixel to use for drawing
     *
     */
    void drawRectangle(int x1, int y1, int x2, int y2, PixelRGBA p);

    /**************************************************************************/
    /**
     * @brief  Clears the drawing to a specified color
     *
     * @param p The color to use
     *
     */
    void clear(PixelRGBA const& p = {0, 0, 0, 255});

    /**************************************************************************/
    /**
     * @brief  Retreive the x, y dimensions a given image
     *
     * @param handle Handle for the image
     *
     * @return pair of integers representing the dimensions
     *
     */
    std::pair<int, int> getImageDimensions(ImageHandle handle);

    /**************************************************************************/
    /**
     * @brief  Actual draw the image (coordinates are inclusive)
     *
     * @param x1 upper left x coordinate
     * @param y1 upper left y coordinate
     * @param handle Reference to the texture to be drawn
     *
     */
    void drawImage(int x1, int y1, ImageHandle handle);

    /**************************************************************************/
    /**
     * @brief  Actual draw the image (coordinates are inclusive)
     *
     * @param x1 upper left x coordinate
     * @param y1 upper left y coordinate
     * @param x2 lower right x coordinate
     * @param y2 lower right y coordinate
     * @param handle Reference to the texture to be drawn
     *
     */
    void drawImage(int x1, int y1, int x2, int y2, ImageHandle handle);

    /**************************************************************************/
    /**
     * @brief  Update the screens with all changes
     *
     */
    void draw();

    /**************************************************************************/
    /**
     * @brief  Load a texture from disk
     *
     * @param filename File to load
     *
     * @returns handle for referring to the loaded image
     *
     */
    ImageHandle loadImage(std::string const& filename);

    /**************************************************************************/
    /**
     * @brief  Write the image to disk
     *
     * @param handle Handle associate with image
     * @param filename Name of file to write
     * @param sequential If true, don't overwrite file, add a consecutive
     *                   number
     *
     * Currently assumes .png files (extention not necessary)
     *
     */
    void saveImage(ImageHandle handle, std::string const& filename,
                   bool sequential = false);

    /**************************************************************************/
    /**
     * @brief  Display a critical error and exit
     *
     * @param errorMsg Error description
     *
     */
    void error(std::string const& errorMsg) {
        std::cerr << "Error: " << errorMsg << '\n';
        shutdown();
        exit(EXIT_FAILURE);
    }
};

} // namespace MJB

#endif // MJB_RENDERER
