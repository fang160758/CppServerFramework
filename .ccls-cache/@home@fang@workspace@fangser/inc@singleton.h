#ifndef __FANG_SINGLETON_H__
#define __FANG_SINGLETON_H__

namespace fang {


//类的单例
template <typename T>
class Singleton {

public:
    static T * GetInstance() {
        static T t;
        return &t;
    }
};

//禁用类赋值与拷贝函数
class Noncopyable {
public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable &) = delete;
    Noncopyable &operator=(const Noncopyable &) = delete;
};

}


#endif
