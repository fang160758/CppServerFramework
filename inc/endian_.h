#ifndef __FANG_ENDIAN_H__
#define __FANG_ENDIAN_H__

#include <type_traits>
#include <bits/byteswap.h>
#include <stdint.h>
#include <endian.h>

namespace fang {

    template <typename T>
    typename std::enable_if<sizeof(T) == sizeof(uint8_t), T>::type
    byteswap(T v) {
        return (T)v;
    }
    template <typename T>
    typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type 
    byteswap(T v) {
        return (T)__bswap_16(v);
    }
    template <typename T>
    typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type 
    byteswap(T v) {
        return (T)__bswap_32(v);
    }
    template <typename T>
    typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type 
    byteswap(T v) {
        return (T)__bswap_64(v);
    }



#if __BYTE_ORDER == __BIG_ENDIAN
    template <typename T>
    T byteswapOnLittleEndian(T v) {
        return byteswap(v);
    }

    template <typename T>
    T byteswapOnBigEndian(T v) {
        return v;
    }

#elif __BYTE_ORDER == __LITTLE_ENDIAN
    template <typename T>
    T byteswapOnLittleEndian(T v) {
        return v;
    }

    template <typename T>
    T byteswapOnBigEndian(T v) {
        return byteswap(v);
    }
#endif

}

#endif

