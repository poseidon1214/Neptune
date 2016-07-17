// internal header, no inclusion guard needed

#include "app/qzap/common/recordio/internal/type_cast.h"
#include "app/qzap/common/recordio/internal/check_direct_include.h"

// namespace common {

// align insensitive archs

template <typename T>
T GetUnaligned(const void* p)
{
    return *static_cast<const T*>(p);
}

// introduce U make T must be given explicitly
template <typename T, typename U>
void PutUnaligned(void* p, const U& value)
{
    *static_cast<T*>(p) = implicit_cast<T>(value);
}

//} // namespace common
