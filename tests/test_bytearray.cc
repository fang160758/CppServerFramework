#include "../inc/bytearray.h"
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <stdint.h>

void test_FData() {
    //fang::ByteArray::ptr bt(new fang::ByteArray(1));
    fang::ByteArray bt(1);
    typedef uint64_t type;
    std::vector<type> vec;
    for (int i = 0; i < 100; i ++) {
        vec.push_back(rand());
    }
    for (auto &i : vec) {
    //    bt.write(&i, sizeof(type));
        bt.writeFData(i);
    //bt.writeFint32(i);
        //std::cout << i << std::endl;
    }
    bt.setPosition(0);
    
    for (size_t i = 0; i < vec.size(); i ++) {
        type v;
        //bt.read(&v, sizeof(type));
        bt.readFData(v);

        //if (i && i % 15 == 0)
        //    std::cout << std::endl;
        //std::cout << v << " " << vec[i];
        //char tmp;
        //std::cin >> tmp;
        if (v != vec[i])
            std::cout << i << " error --->" << "v=" << v << "\tvec_=" << vec[i] << std::endl;
    }
    std::cout << "all enqule!" <<std::endl;
}

void test_ZData() {
    
    fang::ByteArray bt(1);
    typedef int64_t type;
    std::vector<type> vec;
    for (int i = 0; i < 100; i ++) {
        vec.push_back((type)rand() * rand());
    }
    for (auto &i : vec) {
        bt.writeInt64(i);
    }
    bt.setPosition(0);
    
    for (size_t i = 0; i < vec.size(); i ++) {
        type v;
        bt.readInt64(v);

        if (v != vec[i])
            std::cout << i << " error --->" << "v=" << v << "\tvec_=" << vec[i] << std::endl;
    }
    std::cout << "all enqule!" <<std::endl;
    std::cout << "This Nodes size=" << bt.getSize() << std::endl;
}

void test_String() {

    fang::ByteArray bt(3);
    std::string str = "你好呀！";
    bt.writeString(str);

    bt.setPosition(0);

    std::string str_;
    bt.readString(str_);
    std::cout << str << std::endl << str_ << std::endl;
}

void test_File() {
    fang::ByteArray bt(10);
    std::vector<int32_t> vec;
    for (int i = 0; i < 100; i++) {
        vec.push_back(rand());
        bt.writeInt32(vec[i]);
    }
    bt.setPosition(0);
    bt.writeToFile("./bytearray_sys");
    bt.clear();
    bt.readFromFile("./bytearray_sys");
    bt.setPosition(0);
    for (int i = 0; i < 100; i++) {
        int32_t v;
        bt.readInt32(v);
        
        if (v != vec[i])
            std::cout << i << " error --->" << "v=" << v << "\tvec_=" << vec[i] << std::endl;
    }
    std::cout << "all data equle and the size=" << bt.getSize() << std::endl;

}

int main()
{
    //test_FData();
    //test_ZData();
    //test_String();
    test_File();
}
