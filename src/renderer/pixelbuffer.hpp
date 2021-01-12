//
// pixlebuffer.h
//
//
//  Created by Micah J Best on 2020-07-01.
//  Copyright © 2020 Micah J Best. All rights reserved.
//

#pragma once

#include "pixel.h"

/**************************************************************************/
/**
 * @name PixelBuffer
 * @brief  Holds a square, contigious region of pixels and various operations to
 * manipulate them
 *
 */
template <typename PIXELFORMAT>
class PixelBuffer {
    size_t height_ = 0;
    size_t width_ = 0;

    PIXELFORMAT* pixels_ = nullptr;

  public:
    /**************************************************************************/
    /**
     * @brief  Default constructor
     *
     * Leaves the buffer uninitialized
     *
     */
    PixelBuffer() {}

    /**************************************************************************/
    /**
     * @brief  Constructor
     *
     * @param height height in pixels
     * @param width width in pixels
     *
     */
    PixelBuffer(height, width) : height_(height), width_(width) {
        pixels_ = new PIXELFORMAT
    }

    /**************************************************************************/
    /**
     * @brief Clear (deallocate) the data
     *
     */
    void clear() {
        if (not pixels_) {
            delete pixels_;
            pixels = nullptr;
        }
    }

    /**************************************************************************/
    /**
     * @brief  Bool conversion operator,
     *
     * @return true if data has been initialized, false otherwise
     *
     */
    bool operator bool() { return pixels_ != nullptr; }

    /**************************************************************************/
    /**
     * @brief  Destructor, frees memory
     *
     */
    ~PixelBuffer() { clear(); }
};
