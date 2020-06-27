#ifndef MJB_RENDERER_PIXEL
#define MJB_RENDERER_PIXEL

#include <cstdint>

#include <string>
#include <iostream>
#include <array>

namespace MJB {

struct PixelRawRGB {
    uint8_t R_; /**< red value */
    uint8_t G_; /**< green value */
    uint8_t B_; /**< blue value */    
};

struct PixelRawRGBA {
    uint8_t R_; /**< red value */
    uint8_t G_; /**< green value */
    uint8_t B_; /**< blue value */ 
    uint8_t A_; /**< alpha value */   
};

// 'tag' to indicate that values given are already premultiplied
struct PremultipliedAlpha {};

/**************************************************************************/
/**
 * @name PixelRGB
 * @brief Represent a single pixel with Red, Green and Blue channels
 *
 *  Mostly to make the code cleaner
 *
 */
class PixelRGB : public PixelRawRGB {

    uint8_t multiplyAlpha( uint8_t color, uint8_t alpha ) {
        return (uint8_t)(color * ((float)alpha/255.0) );
    }

public:

    static const uint8_t channels = 4;

    /**************************************************************************/
    /**
     * @brief  Constructor
     *
     */
    PixelRGB() {}

    /**************************************************************************/
    /**
     * @brief  Initialization constructor - will premultiply the alpha
     *
     * @param _R Red value
     * @param _G Green value
     * @param _B Blue value
     *
     */
    PixelRGB( uint8_t _R, uint8_t _G, uint8_t _B ) :
        PixelRawRGB{_R, _G, _B}
    {}

    /**************************************************************************/
    /**
     * @brief  Initialization constructor from 'Raw' pixel struct
     *
     * @param _R Red value
     * @param _G Green value
     * @param _B Blue value
     *
     */
    PixelRGB( PixelRawRGB const& other ) :
        PixelRawRGB( other )
    {}

    /**************************************************************************/
    /**
     * @brief  Initialization constructor with alpha (will premultiply)
     *
     * @param _R Red value
     * @param _G Green value
     * @param _B Blue value
     * @param _A Alpha value
     *
     */
    PixelRGB( uint8_t _R, uint8_t _G, uint8_t _B, uint8_t _A ) :
        PixelRawRGB{ multiplyAlpha(_R, _A),
                     multiplyAlpha(_G, _A),
                     multiplyAlpha(_B, _A) }
    {}

    /**************************************************************************/
    /**
     * @brief  Initialization constructor from size 3 array 
     *
     * @param channelData Array of four bytes
     *
     */
    PixelRGB( std::array<uint8_t, 3> const& channelData ) :
        PixelRawRGB{ channelData[0],
                     channelData[1],
                     channelData[2] }
    {}

    /**************************************************************************/
    /**
     * @brief  Initialization constructor from size 4 array 
     *
     * @param channelData Array of four bytes
     *
     */
    PixelRGB( std::array<uint8_t, 4> const& channelData ) :
        PixelRawRGB{ multiplyAlpha(channelData[0], channelData[3]),
                     multiplyAlpha(channelData[1], channelData[3]),
                     multiplyAlpha(channelData[2], channelData[3])}
    {}

    /**************************************************************************/
    /**
     * @brief  Initialization constructor from size 4 array 
     *
     * @param channelData Array of four bytes
     *
     */
    PixelRGB( std::array<uint8_t, 4> const& channelData, 
              PremultipliedAlpha pma ) :
        PixelRawRGB{ channelData[0], channelData[1], channelData[2] }
    {}

    /**************************************************************************/
    /**
     * @brief  Copy constructor
     *
     * @param other Pixel to copy
     *
     */
    PixelRGB( PixelRGB const& other ) = default;

    /**************************************************************************/
    /**
     * @brief  Copy constructor from 'raw' RGBA pixel (with premultiplied alpha)
     *
     * @param other Pixel to copy
     *
     */
    PixelRGB( PixelRawRGBA const& other ) :
        PixelRawRGB{ other.R_, other.G_, other.B_ }
    {}

        /**************************************************************************/
    /**
     * @brief  Component access - red
     *
     * @return reference to component
     *
     */
    uint8_t& R() {
        return R_;
    }

    /**************************************************************************/
    /**
     * @brief  Component access - green
     *
     * @return reference to component
     *
     */
    uint8_t& G() {
        return G_;
    }

    /**************************************************************************/
    /**
     * @brief  Component access - blue
     *
     * @return reference to component
     *
     */
    uint8_t& B() {
        return B_;
    }

    /**************************************************************************/
    /**
     * @brief  Assignment operator
     *
     * @param other Source pixel
     *
     */
    PixelRGB& operator=(PixelRGB const& other) = default;

    /**************************************************************************/
    /**
     * @brief  'User defined' conversion operator to size 3 array
     *
     */
    explicit operator std::array<uint8_t, 3>() const {
        return { R_, G_, B_ };
    }

    std::string toString() const {
        return "[ R: " + std::to_string( R_ ) + ", " +
                " G: " + std::to_string( G_ ) + ", " +
                " B: " + std::to_string( B_ ) + " ]";
    }

};

/**************************************************************************/
/**
 * @name PixelRGBA
 * @brief Represent a single pixel with an alpha channel
 *
 *  Mostly to make the code cleaner
 *
 */
class PixelRGBA : public PixelRawRGBA {

    uint8_t multiplyAlpha( uint8_t color, uint8_t alpha ) {
        return (uint8_t)(color * (alpha/255.0) );
    }

public:

    static const uint8_t channels = 4;

    /**************************************************************************/
    /**
     * @brief  Constructor
     *
     */
    PixelRGBA() {}

    /**************************************************************************/
    /**
     * @brief  Initialization constructor - will premultiply the alpha
     *
     * @param _A Alpha value
     * @param _R Red value
     * @param _G Green value
     * @param _B Blue value
     *
     */
    PixelRGBA( uint8_t _R, uint8_t _G, uint8_t _B, uint8_t _A ) :
        PixelRawRGBA{ multiplyAlpha(_R, _A), 
                      multiplyAlpha(_G, _A), 
                      multiplyAlpha(_B, _A), 
                      _A }
    {}

    /**************************************************************************/
    /**
     * @brief  Initialization constructor - will not premultiply the alpha
     *
     * @param _A Alpha value
     * @param _R Red value
     * @param _G Green value
     * @param _B Blue value
     *
     */
    PixelRGBA( uint8_t _R, uint8_t _G, uint8_t _B, uint8_t _A, 
               PremultipliedAlpha pma ) :
        PixelRawRGBA{ _R, _G, _B, _A }
    {}


    /**************************************************************************/
    /**
     * @brief  Initialization constructor - no alpha
     *
     * @param _R Red value
     * @param _G Green value
     * @param _B Blue value
     *
     */
    PixelRGBA( uint8_t _R, uint8_t _G, uint8_t _B ) :
        PixelRawRGBA{ _R, _G, _B, 255 }
    {}

    /**************************************************************************/
    /**
     * @brief  Initialization constructor - from RGA pixel
     *
     * @param other Raw pixel value
     *
     */
    PixelRGBA( PixelRawRGB const& other ) :
        PixelRawRGBA{ other.R_, other.G_, other.B_, 255 }
    {}

    /**************************************************************************/
    /**
     * @brief  Initialization constructor from size 3 vector
     *
     * @param channelData Array of byes in RGBA order
     *
     */
    PixelRGBA( std::array<uint8_t, 3> const& channelData ) :
        PixelRawRGBA{ channelData[0], 
                      channelData[1], 
                      channelData[2], 
                      255 }
    {}

    /**************************************************************************/
    /**
     * @brief  Initialization constructor from size 4 vector - will premultiply
     *
     * @param channelData Array of byes in RGBA order
     * 
     */
    PixelRGBA( std::array<uint8_t, 4> const& channelData ) :
        PixelRawRGBA{ multiplyAlpha(channelData[0], channelData[3]), 
                      multiplyAlpha(channelData[1], channelData[3]), 
                      multiplyAlpha(channelData[2], channelData[3]), 
                      channelData[3] }
    {}

    /**************************************************************************/
    /**
     * @brief  Initialization constructor from size 4 vector - no premultiply
     *
     * @param channelData Array of byes in RGBA order
     * 
     */
    PixelRGBA( std::array<uint8_t, 4> const& channelData, 
               PremultipliedAlpha pma ) :
        PixelRawRGBA{ channelData[0], 
                      channelData[1], 
                      channelData[2], 
                      channelData[3] }
    {}


    /**************************************************************************/
    /**
     * @brief  Copy constructor
     *
     * @param other Pixel to copy
     *
     */
    PixelRGBA( PixelRGBA const& other ) = default;

    /**************************************************************************/
    /**
     * @brief  Component access - red
     *
     * @return reference to component
     *
     */
    uint8_t& R() {
        return R_;
    }

    /**************************************************************************/
    /**
     * @brief  Component access - red
     *
     * @return reference to component
     *
     */
    uint8_t const& R() const {
        return R_;
    }

    /**************************************************************************/
    /**
     * @brief  Component access - green
     *
     * @return reference to component
     *
     */
    uint8_t& G() {
        return G_;
    }

    /**************************************************************************/
    /**
     * @brief  Component access - green
     *
     * @return reference to component
     *
     */
    uint8_t const& G() const {
        return G_;
    }

    /**************************************************************************/
    /**
     * @brief  Component access - blue
     *
     * @return reference to component
     *
     */
    uint8_t& B() {
        return B_;
    }

    /**************************************************************************/
    /**
     * @brief  Component access - blue
     *
     * @return reference to component
     *
     */
    uint8_t const& B() const {
        return B_;
    }

    /**************************************************************************/
    /**
     * @brief  Component access - alpha
     *
     * @return reference to component
     *
     */
    uint8_t& A() {
        return A_;
    }

    /**************************************************************************/
    /**
     * @brief  Component access - alpha
     *
     * @return reference to component
     *
     */
    uint8_t const& A() const {
        return A_;
    }

    /**************************************************************************/
    /**
     * @brief  Assignment operator
     *
     * @param other Source pixel
     *
     */
    PixelRGBA& operator=(PixelRGBA const& other) = default;

    /**************************************************************************/
    /**
     * @brief  Additive operator
     *
     * @param other Pixel to add to this one ('source')
     *
     */
    PixelRGBA& operator+=( PixelRGBA const& other ) {

        // check to see if we need to blend
        if( other.A_ == 255 ) {
            R_ = other.R_;
            G_ = other.G_;
            B_ = other.B_;
            A_ = other.A_;
        }
        else {
            float sourceFactor = (255.0 - (float)other.A_)/255.0;

            R_ = other.R_ + (uint8_t)((float)R_ * sourceFactor );
            G_ = other.G_ + (uint8_t)((float)G_ * sourceFactor );
            B_ = other.B_ + (uint8_t)((float)B_ * sourceFactor );
            A_ = other.A_ + (uint8_t)((float)A_ * sourceFactor );
        }

        return *this;
    }

    /**************************************************************************/
    /**
     * @brief  'User defined' conversion operator to size 4 array
     *
     */
    explicit operator std::array<uint8_t, 4>() const {
        return { R_, G_, B_, A_ };
    } 

    std::string toString() const {
        return "[ R: " + std::to_string( R_ ) + ", " +
                " G: " + std::to_string( G_ ) + ", " +
                " B: " + std::to_string( B_ ) + ", " +
                " A: " + std::to_string( A_ ) + " ]";
    }

};

inline std::ostream& operator<<( std::ostream& os, PixelRGB const& p ) {
    os << p.toString();
    return os;
}

inline std::ostream& operator<<( std::ostream& os, PixelRGBA const& p ) {
    os << p.toString();
    return os;
}

} // namespace MJB

#endif //MJB_RENDERER_PIXEL