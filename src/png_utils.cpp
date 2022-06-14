#include "png_utils.hpp"

#include "png.h"

namespace tg
{
class BytesWriter
{
public:
    BytesWriter(Bytes& bytes)
        : bytes_(bytes)
    {
    }
    void write(png_bytep in_bytes, png_size_t byte_count_to_write)
    {
        size_t cursor{bytes_.size()};
        bytes_.resize(bytes_.size() + byte_count_to_write);
        memcpy(&bytes_[cursor], in_bytes, byte_count_to_write);
    }

private:
    Bytes& bytes_;
};

void write_bytes(png_structp png_ptr, png_bytep in_bytes, size_t byte_count_to_write)
{
    png_voidp io_ptr{png_get_io_ptr(png_ptr)};
    if (io_ptr == nullptr)
        throw std::runtime_error("io_ptr == nullptr from write_bytes in png_writer.cpp");

    static_cast<BytesWriter*>(io_ptr)->write(in_bytes, byte_count_to_write);
}

void flush_bytes(png_structp /*png_ptr*/)
{
    // Nothing to do.
}

Bytes PNGUtils::write(int width,
                      int height,
                      const vector<uint8_t>& r_channel,
                      const vector<uint8_t>& g_channel,
                      const vector<uint8_t>& b_channel,
                      const vector<uint8_t>& a_channel)
{
    Bytes bytes;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_bytep row = NULL;

    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
        throw std::runtime_error("Could not allocate write struct");

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
        throw std::runtime_error("Could not allocate info struct");

#pragma warning(push)
#pragma warning(disable : 4611)
    // Setup Exception handling
    if (setjmp(png_jmpbuf(png_ptr)))
        throw std::runtime_error("Error during png creation");
#pragma warning(pop)

    BytesWriter bytes_writer{bytes};
    png_set_write_fn(png_ptr, &bytes_writer, write_bytes, flush_bytes);

    // Write header (8 bit colour depth)
    png_set_IHDR(png_ptr,
                 info_ptr,
                 width,
                 height,
                 8,
                 PNG_COLOR_TYPE_RGB_ALPHA,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    // Allocate memory for one row (4 bytes per pixel - RGB)
    row = (png_bytep)malloc(4 * width * sizeof(png_byte));

    // Write image data
    int x, y;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            row[x * 4 + 0] = r_channel[y * width + x];
            row[x * 4 + 1] = g_channel[y * width + x];
            row[x * 4 + 2] = b_channel[y * width + x];
            row[x * 4 + 3] = a_channel[y * width + x];
        }
        png_write_row(png_ptr, row);
    }

    // End write
    png_write_end(png_ptr, NULL);

    if (info_ptr != NULL)
        png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL)
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (row != NULL)
        free(row);

    return bytes;
}
} // namespace tg
