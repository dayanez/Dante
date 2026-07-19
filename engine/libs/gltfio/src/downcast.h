
#ifndef TNT_DANTE_DOWNCAST_H
#define TNT_DANTE_DOWNCAST_H

/*
 * Generates functions to safely downcast a pointer Bar* to FBar*
 * DANTE_DOWNCAST() should be included in the header file
 * declaring FBar, e.g.:
 *
 * #include <Bar.h>
 *
 * class FBar : public Bar {
 * };
 *
 * DANTE_UPCAST(Bar)
 *
 */

#define DANTE_DOWNCAST(CLASS)                                    \
    inline F##CLASS& downcast(CLASS& that) noexcept {               \
        return static_cast<F##CLASS &>(that);                       \
    }                                                               \
    inline const F##CLASS& downcast(const CLASS& that) noexcept {   \
        return static_cast<const F##CLASS &>(that);                 \
    }                                                               \
    inline F##CLASS* downcast(CLASS* that) noexcept {               \
        return static_cast<F##CLASS *>(that);                       \
    }                                                               \
    inline F##CLASS const* downcast(CLASS const* that) noexcept {   \
        return static_cast<F##CLASS const *>(that);                 \
    }

#endif // TNT_DANTE_DOWNCAST_H
