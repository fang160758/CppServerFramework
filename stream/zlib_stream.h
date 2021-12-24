#pragma once 
#include "../inc/stream.h"
#include <vector>
#include <sys/uio.h>
#include <zlib.h>

namespace fang {

class ZlibStream : public Stream {
private:
    z_stream m_zstream;
    uint32_t m_buffSize;
    bool m_encode;
    bool m_free;
    std::vector<iovec> m_buffs;

public:
    typedef std::shared_ptr<ZlibStream> ptr;

    enum Type {
        ZLIB,
        DEFLATE,
        GZIP
    };
    
    enum Strategy {
        DEFAULT = Z_DEFAULT_STRATEGY,
        FILTERD = Z_FILTERED,
        HUFFMAN = Z_HUFFMAN_ONLY,
        FIXED   = Z_FIXED,
        RLE     = Z_RLE
    };

    enum CompressLevel {
        NO_COMPRESSION      = Z_NO_COMPRESSION,
        BEST_SPEED          = Z_BEST_SPEED,
        BESCOMPRESSION      = Z_BEST_COMPRESSION,
        DEFAULT_COMPRESSION = Z_DEFAULT_COMPRESSION
    };


    static ZlibStream::ptr CreateGzip(   bool encode, uint32_t buff_size = 4096);
    static ZlibStream::ptr CreateZlib(   bool encode, uint32_t buff_size = 4096);
    static ZlibStream::ptr CreateDeflate(bool encode, uint32_t buff_size = 4096);
    static ZlibStream::ptr Create(       bool encode, uint32_t buff_size = 4096
            , Type type = DEFAULT, int level = DEFAULT_COMPRESSION, int window_bits = 15
            , int memlevel = 8, Strategy strategy = DEFAULT);

    ZlibStream(bool encode, uint32_t buff_size = 4096);
    ~ZlibStream();








};






}
