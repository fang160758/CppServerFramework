#include "../inc/stream.h"


namespace fang {
    
    int Stream::readFixSize(void* buffer, size_t length) {
        size_t offest = 0;
        int64_t left = length;
        while(left > 0) {
            int64_t len = read((char*)buffer + offest, left);
            if (len <= 0) {
                return len;
            }
            offest += len;
            left -= len;
        } 
        return length;
    }
    int Stream::readFixSize(ByteArray::ptr ba, size_t length) {
        int64_t left = length;
        while(left > 0) {
            int64_t len = read(ba, left);
            if (len <= 0) {
                return len;
            }
            left -= len;
        }
        return length;
    }
    int Stream::writeFixSize(const void* buffer, size_t length) {
        size_t offest = 0;
        int64_t left = length;
        while (left > 0) {
            int64_t len = write((const char*)buffer + offest, left);
            if (len <= 0) {
                return len;
            }
            offest += len;
            left -= len;
        }
        return length;
    }
    int Stream::writeFixSize(ByteArray::ptr ba, size_t length) {
        int64_t left = length;
        while(left > 0) {
            int64_t len = write(ba, left);
            if (len <= 0) {
                return len;
            }
            left -= len;
        }
        return length;
    }
}
