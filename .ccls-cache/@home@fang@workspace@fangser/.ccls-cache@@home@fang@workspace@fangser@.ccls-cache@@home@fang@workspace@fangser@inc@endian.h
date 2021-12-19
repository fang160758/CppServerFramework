#ifndef __FANG_ENDIAN_H_
#define __FANG_ENDIAN_H_

#define FANG_BIG_ENDIAN 1
#define FANG_LITTLE_ENDIAN 2

#include <stdint.h>
#include <byteswap.h>

namespace fang {

template <typename T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t)>::type
byteswap(T value) {
    return (T)bswap_16((uint16_t)value);
}

template <class T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t)>::type
byteswap(T value) {
    return (T)bswap_32((uint32_t)value);
}


template <class T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t)>::type
byteswap(T value) {
    return (T)bswap_64((uint64_t)value);
}

#if BYTE_ORDER == BIG_ENDIAN
#define FANG_BYTE_ENDIAN FANG_BIG_ENDIAN
#else
#define FANG_BYTE_ENDIAN FANG_LITTLE_ENDIAN
#endif


#if FANG_BYTE_ENDIAN == FANG_BIG_ENDIAN
template <class T>
T byteswapOnLittleEndian(T t) {
    return t;
}

template <class T>
T byteswapOnBigEndian(T t) {
    return byteswap(t);
}


#else
template <class T>
T byteswapOnLittleEndian(T t) {
    return byteswap(t);
}

template <class T>
T byteswapOnBigEndian(T t) {
    return t;
}

#endif

}

#endif /* ifndef __FANG_ENDIAN_H_ */





