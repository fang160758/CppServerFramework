#ifndef __FANG_BYTEARRAY_H__
#define __FANG_BYTEARRAY_H__

#include <memory>
#include <string.h>
#include <stdint.h>

#define DEFAULT_NODE_SIZE 4096
namespace fang {

    class ByteArray {

        public:
            typedef std::shared_ptr<ByteArray> ptr;
            
            struct Node {
                Node()
                    : ptr(nullptr)
                    , next(nullptr)
                    , size(0) {}
                Node(size_t s) 
                    : ptr(new char[s])
                    , next(nullptr)
                    , size(s) {}

                ~Node() {
                    if (ptr != nullptr) delete[] ptr;
                }

                char *ptr;
                Node *next;
                size_t size;
            };

            ByteArray(size_t base_size = DEFAULT_NODE_SIZE); 
            ~ByteArray();


            void write(const void* buf, size_t size);
            void read(void* buf, size_t size);
            void read(void* buf, size_t size, size_t position) const;
            

            bool writeToFile(const std::string& name) const;
            bool readFromFile(const std::string& name);

            std::string toString() const;
            std::string toHexString() const;

            void clear();
            void addCapacity(size_t size);
            void setPosition(size_t v);

            size_t getCapacity() const { return m_capacity - m_position; }
            size_t getReadSize() const { return m_size - m_position; }
        private:
            size_t m_baseSize;
            size_t m_position;
            size_t m_capacity;
            size_t m_size;
            size_t m_endian;
            Node* m_root;
            Node* m_cur;
    };
}


#endif
