#ifndef MJB_RENDERER_IMAGE
#define MJB_RENDERER_IMAGE

#include <iostream>
#include <memory>
#include <string>

#include "pixel.hpp"

namespace MjB {

/**************************************************************************/
/**
 * @name Image
 * @brief Contain the pixel data and metadata for an image
 *
 */
template <typename PixelType>
class Image {
    uint32_t xDim;
    uint32_t yDim;
    PixelType* pixels = nullptr;

  public:
    static const uint8_t channels = PixelType::channels;

    /**************************************************************************/
    /**
     * @brief  Constructor
     *
     * @param x width of the new image
     * @param y height of the new image
     *
     */
    Image(int x, int y) : xDim(x), yDim(y) { pixels = new PixelType[x * y]; }

    // non-copyable
    Image(Image const& other) = delete;

    /**************************************************************************/
    /**
     * @brief  Move constructor
     *
     * @param other Image to move
     *
     */
    Image(Image&& other) :
        xDim(other.xDim), yDim(other.yDim), pixels(other.pixels) {
        other.pixels = nullptr;
    }

    /**************************************************************************/
    /**
     * @brief  Move assignment
     *
     * @param other Image to move
     *
     */
    Image&& operator=(Image&& other) {
        xDim = other.xDim;
        yDim = other.yDim;
        pixels = other.pixels;
        other.pixels = nullptr;
    }

    /**************************************************************************/
    /**
     * @brief Get the width of the image
     *
     * @param
     *
     */
    uint32_t sizeX() const { return xDim; }

    /**************************************************************************/
    /**
     * @brief Get the height of the image
     *
     * @param
     *
     */
    uint32_t sizeY() const { return yDim; }

    /**************************************************************************/
    /**
     * @brief  Set a single pixel
     *
     * @param x x coordinate of the target pixel
     * @param y y coordinate of the target pixel
     * @param colour New value for the pixel
     *
     */
    void setPixel(uint32_t x, uint32_t y, PixelType const& colour) {
        int pos = y * (xDim) + x;
        pixels[pos] = colour;
    }

    /**************************************************************************/
    /**
     * @brief  Get a single pixel
     *
     * @param x x coordinate of the source pixel
     * @param y y coordinate of the source pixel
     * @return current value of the pixel
     *
     */
    PixelType const& getPixel(uint32_t x, uint32_t y) const {
        size_t pos = y * (xDim) + x;
        return pixels[pos];
    }

    /**************************************************************************/
    /**
     * @brief  Fill the image with a single color (default white)
     *
     * @param color Color to fill (as RGBA pixel)
     *
     */
    void clear(PixelType const& color = PixelType(255, 255, 255, 255)) {
        for (uint32_t x = 0; x < xDim; ++x) {
            for (uint32_t y = 0; y < yDim; ++y) {
                setPixel(x, y, color);
            }
        }
    }

    /**************************************************************************/
    /**
     * @brief  Access the raw buffer of pixel/channel data
     *
     * @return a pointer to the image data
     *
     */
    void* getChannelDataBuffer() { return reinterpret_cast<void*>(pixels); }

    /**************************************************************************/
    /**
     * @brief  Access the raw buffer of pixel/channel data
     *
     * @return a pointer to the image data
     *
     */
    const void* getChannelDataBuffer() const {
        return reinterpret_cast<void*>(pixels);
    }

    /**************************************************************************/
    /**
     * @brief  Destructor
     *
     */
    ~Image() {
        if (pixels != nullptr) {
            delete pixels;
        }
    }
};

using ImageRGBA = Image<PixelRGBA>;

ImageRGBA readImageRGBA(std::string const& filename);
void writeImageRGBA(ImageRGBA const& image, std::string const& filename,
                    bool sequential);

} // namespace MjB

#endif // MJB_RENDERER_IMAGE