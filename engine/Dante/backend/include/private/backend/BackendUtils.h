
#ifndef TNT_DANTE_BACKEND_PRIVATE_BACKENDUTILS_H
#define TNT_DANTE_BACKEND_PRIVATE_BACKENDUTILS_H

#include <backend/DriverEnums.h>
#include <backend/PixelBufferDescriptor.h>

#include <string_view>

#include <stddef.h>

namespace dante::backend {

/**
 * Returns true if the shader string requests the Google-style line directive extension.
 */
bool requestsGoogleLineDirectivesExtension(std::string_view source) noexcept;

/**
 * Edit a GLSL shader string in-place so any Google-style line directives are turned into regular
 * line directives.
 *
 * E.g.:
 * #line 100 "foobar.h"
 * is transformed to (_ denotes a space)
 * #line 100 __________
 */
void removeGoogleLineDirectives(char* shader, size_t length) noexcept;

/**
 * Returns the number of bytes per pixel for the given format. For compressed texture formats,
 * returns the number of bytes per block.
 */
size_t getFormatSize(TextureFormat format) noexcept;

/**
 * Returns the number of component (1 to 4) for the given format.
 */
size_t getFormatComponentCount(TextureFormat format) noexcept;

/**
 * For compressed texture formats, returns the number of horizontal texels per block. Otherwise
 * returns 0.
 */
size_t getBlockWidth(TextureFormat format) noexcept;

/**
 * For compressed texture formats, returns the number of vertical texels per block. Otherwise
 * returns 0.
 */
size_t getBlockHeight(TextureFormat format) noexcept;

/**
 * Reshapes 3-component data into 4-component data.
 */
bool reshape(const PixelBufferDescriptor& data, PixelBufferDescriptor& reshaped);

} // namespace dante

#endif // TNT_DANTE_BACKEND_PRIVATE_BACKENDUTILS_H
