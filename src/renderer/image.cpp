#include "stb/stb_image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "image.hpp"

namespace MjB {

// std::filesystem::path findFreeSequentialFile( std::string const& prefix,
//                                               std::string const& extention )
//                                               {
//     std::filesystem::path base = std::filesystem::current_path();
//     std::filesystem::path results;
//     int sequenceNumber = 0;

//     do {
//         auto sequenceString = std::to_string(sequenceNumber);
//         while( sequenceString.length() < 4 ) {
//             sequenceString = "0" + sequenceString;
//         }
//         auto filename = prefix + "-" + sequenceString + "." + extention;

//         results = base / filename;

//         ++sequenceNumber;
//     } while( std::filesystem::exists(results) );

//     return results;
// }

ImageRGBA readImageRGBA(std::string const& filename) {
    int width;
    int height;
    int orig_format;

    // TODO: re-enable to other formats
    int req_format = STBI_rgb_alpha;

    unsigned char* data =
        stbi_load(filename.c_str(), &width, &height, &orig_format, req_format);

    if (data == NULL) {
        std::cout << "Loading image " << filename
                  << " failed: " << stbi_failure_reason() << "\n";
        // error(std::string("Loading image failed: ") + stbi_failure_reason());
        exit(1);
    }

    ImageRGBA newImage(width, height);

    std::size_t bufferOffset = 0;
    auto bufferAsArrays = reinterpret_cast<std::array<uint8_t, 4>*>(data);
    for (uint32_t y = 0; y < width; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            newImage.setPixel(x, y, bufferAsArrays[bufferOffset]);
            ++bufferOffset;
        }
    }

    return newImage;
}

void writeImageRGBA(ImageRGBA const& image, std::string const& filename,
                    bool sequential) {
    std::string fullFilename;

    if (not sequential) {
        fullFilename = filename + ".png";
    } else {
        // fullFilename = cadmium::findFreeSequentialFile(filename,
        // "png").string();
        std::cout << "Sequential filename unavailable\n";
        exit(1);
    }

    stbi_write_png(fullFilename.c_str(), image.sizeX(), image.sizeY(), 4,
                   image.getChannelDataBuffer(),
                   image.sizeX() * sizeof(PixelRGBA));
}

} // namespace MjB