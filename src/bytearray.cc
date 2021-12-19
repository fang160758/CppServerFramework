#include "../inc/bytearray.h"
#include "../inc/endian_.h"
#include <math.h>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace fang {
    static uint32_t EncodeZigzag32(const int32_t& v) {
        if (v < 0)
            return ((uint32_t)(-v)) * 2 - 1;
        else
            return v * 2;
    }

    static uint64_t EncodeZigzag64(const int64_t& v) {
        if (v < 0)
            return ((uint64_t)(-v)) * 2 - 1;
        else
            return v * 2;
    }

    static int32_t DecodeZigzag32(const uint32_t& v) {
        return (v >> 1) ^ -(v & 1);
    }

    static int64_t DecodeZigzag64(const uint64_t& v) {
        return (v >> 1) ^ -(v & 1);
    }

    ByteArray::ByteArray(size_t base_size)
        : m_baseSize(base_size)
        , m_position(0)
        , m_capacity(base_size)
        , m_size(0)
        , m_endian(1)
        , m_root(new Node(base_size))
        , m_cur(m_root) {}

    ByteArray::~ByteArray() {
        struct Node* tmp = m_root;
        while(tmp) {
            m_cur = tmp;
            tmp = tmp->next;
            delete m_cur;
        }
    }
    
    template<typename T>
    void ByteArray::writeFData(T value) {
       value = byteswapOnLittleEndian(value);
       write(&value, sizeof(value));
    }

    template<typename T>
    void ByteArray::readFData(T& value) {
        read(&value, sizeof(value));
        value = byteswapOnLittleEndian(value);
    }

#define DEF_XX(type) template void ByteArray::writeFData< type >( type )
    DEF_XX(uint8_t);
    DEF_XX(uint16_t);
    DEF_XX(uint32_t);
    DEF_XX(uint64_t);
#undef DEF_XX

#define DEF_XX(type) template void ByteArray::readFData< type >( type &)
    DEF_XX(uint8_t);
    DEF_XX(uint16_t);
    DEF_XX(uint32_t);
    DEF_XX(uint64_t);
#undef DEF_XX

    void ByteArray::writeInt32(int32_t value) {
        writeUInt32(EncodeZigzag32(value));
    }

    void ByteArray::writeUInt32(uint32_t value) {
        uint8_t tmp[5];
        int i = 0;
        while(value >= 0x80) {
            tmp[i++] = (value & 0x7f) | 0x80;
            value >>= 7;
        }
        tmp[i++] = value;
        write(tmp, i);
    }

    void ByteArray::writeInt64(int64_t value) {
        writeUInt64(EncodeZigzag64(value));
    }

    void ByteArray::writeUInt64(uint64_t value) {
        uint8_t tmp[10];
        int i = 0;
        while(value >= 0x80) {
            tmp[i++] = (value & 0x7f) | 0x80;
            value >>= 7;
        }
        tmp[i++] = value;
        write(tmp, i);
    }
    

    void ByteArray::readInt32(int32_t& value) {
        uint32_t value_;
        readUInt32(value_);
        value = DecodeZigzag32(value_);
    }
    
    void ByteArray::readUInt32(uint32_t& value) {
        value = 0;
        for (int i = 0; i < 32; i += 7) {
            uint8_t tmp;
            readFData(tmp);
            if (tmp < 0x80) {
                value |= ((uint32_t)tmp) << i;
                break;
            } else {
                value |= (((uint32_t)(tmp & 0x7f)) << i);
            }
        }
    }

    void ByteArray::readInt64(int64_t& value) {
        uint64_t value_;
        readUInt64(value_);
        value = DecodeZigzag64(value_);
    }

    void ByteArray::readUInt64(uint64_t& value) {
        value = 0;
        for (int i = 0; i < 64; i += 7) {
            uint8_t tmp;
            readFData(tmp);
            if (tmp < 0x80) {
                value |= ((uint64_t)tmp) << i;
                break;
            } else {
                value |= (((uint64_t)(tmp & 0x7f)) << i);
            }
        }
    }
            
            
    void ByteArray::writeString(const std::string& value) {
        writeUInt32(value.size());
        write(value.c_str(), value.size());
    }
    void ByteArray::readString(std::string& value) {
        uint32_t len;
        readUInt32(len);
        value.resize(len);
        read(&value[0], len);
    }
    

    //void ByteArray::writeFint32(int32_t value) {
    //    value = byteswapOnLittleEndian(value);
    //    write(&value, sizeof(value));
    //}
    //
    //int32_t ByteArray::readFint32(void) {
    //    int32_t value;
    //    read(&value, sizeof(value));
    //    return byteswapOnLittleEndian(value);
    //}

    void ByteArray::clear() {
        m_position = m_size = 0;
        m_capacity = m_baseSize;
        struct Node* tmp = m_root;
        while(tmp) {
            m_cur = tmp;
            tmp = tmp->next;
            delete m_cur;
        }
        m_cur = m_root = new Node(m_baseSize);
        m_root->next = nullptr;
    }
    
    void ByteArray::write(const void* buf, size_t size) {
        if (size == 0) return ;

        addCapacity(size);

        size_t npos = m_position % m_baseSize;
        size_t ncap = m_cur->size - npos;
        size_t bpos = 0;

        while(size > 0) {
            if (ncap >= size) {
                memcpy(m_cur->ptr + npos, (const char*)buf + bpos, size);
                if (m_cur->size == npos + size) {
                    m_cur = m_cur->next;
                }
                m_position += size;
                bpos += size;
                size = 0;
            } else {
                memcpy(m_cur->ptr + npos, (const char*)buf + bpos, ncap);
                m_position += ncap;
                bpos += ncap;
                size -= ncap;
                m_cur = m_cur->next;
                ncap = m_cur->size;
                npos = 0;
            }
        }
        if (m_position > m_size) {
            m_size = m_position;
        }
    }

    void ByteArray::read(void* buf, size_t size) {
        if (size > getReadSize()) {
            throw std::out_of_range("not enough len");
        }
        
        size_t npos = m_position % m_baseSize;
        size_t ncap = m_cur->size - npos;
        size_t bpos = 0;
        while (size > 0) {
            if (ncap >= size) {
                memcpy((char*)buf + bpos, m_cur->ptr + npos, size);
                if (m_cur->size == npos + size) {
                    m_cur = m_cur->next;
                }
                m_position += size;
                bpos += size;
                size = 0;
            } else {
                memcpy((char*)buf + bpos, m_cur->ptr + npos, ncap);
                m_position += ncap;
                bpos += ncap;
                size -= ncap;
                m_cur = m_cur->next;
                ncap = m_baseSize; 
                npos = 0;
            }
        }
    }
    
    void ByteArray::read(void* buf, size_t size, size_t position) const {
        if (size > (m_size - position)) {
            throw std::out_of_range("not enough len");
        }
        size_t npos = position % m_baseSize;
        size_t ncap = m_cur->size - npos;
        size_t bpos = 0;
        struct Node* cur = m_cur;
        while(size > 0) {
            if (ncap >= size) {
                memcpy((char*)buf + bpos, cur->ptr + npos, size);
                break;
            } else {
                memcpy((char*)buf + bpos, cur->ptr + npos, ncap);
                position += ncap;
                bpos += ncap;
                size -= ncap;
                cur = cur->next;
                ncap = m_baseSize; 
                npos = 0;
            }
        }
    }


    bool ByteArray::writeToFile(const std::string& name) const {
        std::ofstream ofs;
        ofs.open(name, std::ios::trunc | std::ios::binary);
        if (!ofs) {
            perror(strerror(errno));
            return false; 
        }

        int read_size = getReadSize();
        int npos = m_position % m_baseSize;
        int ncap = m_cur->size - npos;
        struct Node *cur = m_cur;
        while(read_size > 0) {
            uint64_t len = (read_size > ncap) ? ncap : read_size;
            ofs.write(cur->ptr + npos, len);
            cur = cur->next;
            read_size -= len;
            npos = 0;
            ncap = m_baseSize;
        }  
        return true;
    }

    bool ByteArray::readFromFile(const std::string& name) {
        std::ifstream ifs;
        ifs.open(name, std::ios::binary);
        if (!ifs) {
            perror(strerror(errno));
        }
        std::shared_ptr<char> buf(new char[m_baseSize], [](char *ptr) { delete[] ptr;});
        while(!ifs.eof()) {
            ifs.read(buf.get(), m_baseSize);
            write(buf.get(), ifs.gcount());
        }
        return true;
    } 
    std::string ByteArray::toString() const {
        std::string buf;
        buf.resize(getReadSize());
        if (buf.empty()) {
            return buf;
        }

        read(&buf[0], buf.size(), m_position);
        return buf; 
    }
    std::string ByteArray::toHexString() const {
        std::string buf = toString();
        std::stringstream t_ss;
        for (size_t i = 0; i < buf.size(); i++) {
            if (i > 0 && i % 32 == 0) {
                t_ss << std::endl;
            }
            t_ss << std::setw(2) << std::setfill('0') << std::hex << (int)(uint8_t)buf[i] << " ";
        }
        return t_ss.str();
    }
    
    void ByteArray::addCapacity(size_t size) {
        if (size == 0) return ;
        
        size_t old_cap = getCapacity();
        if (old_cap >= size) return ;

        size -= old_cap;
        size_t count = ceil(1.0 * size / m_baseSize);

        struct Node* tmp = m_root;
        while(tmp && tmp->next) {
            tmp = tmp->next;
        }

        struct Node* first = nullptr;
        for (size_t i = 0; i < count; i++) {
            tmp->next = new Node(m_baseSize);
            if (first == nullptr) {
                first = tmp->next;
            }
            tmp = tmp->next;
            m_capacity += m_baseSize;
        }
        if (old_cap == 0) {
            m_cur = first;
        }    
    }

    void ByteArray::setPosition(size_t v) {
        if (v > m_capacity) {
            throw std::out_of_range("set_position out of range");
        }

        m_position = v;
        if (m_position > m_size) {
            m_size = m_position;
        }
        m_cur = m_root;
        while(m_cur && v >= m_cur->size) {
            v -= m_cur->size;
            m_cur = m_cur->next;
        }
    }
}
