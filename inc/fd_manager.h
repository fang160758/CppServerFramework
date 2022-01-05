/**
 * @file fd_manager.h
 * @Synopsis  文件描述符的封装、文件描述符管理类
 * @author Fang
 * @version 1.0
 * @date 2021-12-30
 */
#ifndef __FD_MANAGER_H__
#define __FD_MANAGER_H__

#include "mutex.h"
#include "singleton.h"
#include <memory>
#include <vector>

namespace fang{

/**
* @Synopsis  文件描述符上下文类，记录一个文件描述符上的信息
*/
class FdCtx : public std::enable_shared_from_this<FdCtx> {

public:
    typedef std::shared_ptr<FdCtx> ptr;
    
    /**
     * @Synopsis  构造器
     *
     * @Param[in] fd 文件描述符
     */
    FdCtx(int fd);
    ~FdCtx() {}

    /**
     * @Synopsis  是否初始化
     */
    bool isInit() const { return m_isInit; }
    
    /**
     * @Synopsis  是否socket
     */
    bool isSocket() const { return m_isSocket; }
    
    /**
     * @Synopsis  是否已经关闭
     */
    bool isClose() const { return  m_isClose; }

    void setUserNonblock(bool v) { m_userNonblock = v; }
    
    bool getUserNonblock() const { return m_userNonblock; }
    
    void setSysNonblock(bool v) { m_sysNonblock = v; }
    
    bool getSysNonblock() const {return m_sysNonblock;}
    
    /**
     * @Synopsis  设置超时时间
     *
     * @Param[in] type 类型(读/写)
     * @Param[in] v 超时时间
     */
    void setTimeout(int type, uint64_t v);
    
    /**
     * @Synopsis  获取超时时间
     *
     * @Param[in] type 类型(读/写)
     *
     * @Returns   超时时间
     */
    uint64_t getTimeout(int type);

private:
    bool init();

private:
    bool m_isInit: 1;       // 是否初始化
    bool m_isSocket: 1;     // 是否socket
    bool m_sysNonblock: 1;  // 是否系统设置非阻塞
    bool m_userNonblock: 1; // 是否用户主动设置非阻塞
    bool m_isClose: 1;      // 是否关闭
    int m_fd;               // 文件描述符
    uint64_t m_recvTimeout; // 读超时时间
    uint64_t m_sendTimeout; // 写超时时间
};

class FdManager {

public:
    FdManager();

    /**
     * @Synopsis  获取/创建文件描述符类FdCTX
     *
     * @Param[in] fd 文件描述符
     * @Param[in] auto_create 是否自动创建
     *
     * @Returns   对应文件描述符的fdctx类
     */
    FdCtx::ptr get(int fd, bool auto_create = false);

    /**
     * @Synopsis  删除对应的文件描述符的fdctx类
     *
     * @Param[in] fd 文件描述符
     */
    void del(int fd);

private:
    RWMutex m_mutex;
    std::vector<FdCtx::ptr> m_datas;
};

typedef Singleton<FdManager> FdMgr;

}

#endif
