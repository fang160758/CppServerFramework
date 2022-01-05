#pragma once
#include <ios>
#include <limits>
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include "http_session.h"
#include "http.h"
#include "../inc/mutex.h"
#include "../inc/helpc.h"
namespace fang{
namespace http{

class Servlet {
public:
    typedef std::shared_ptr<Servlet> ptr;

    /**
     * @Synopsis  构造器
     *
     * @Param[in] name 名称
     */
    Servlet(const std::string& name)
        :m_name(name) {}

    virtual ~Servlet() {}

    /**
     * @Synopsis  处理请求
     *
     * @Param[in] HTTP请求
     * @Param[in] HTTP响应
     * @Param[in] session HTTP连接
     *
     * @Returns   
     */
    virtual int32_t handle(fang::http::HttpRequest::ptr resquest
                    , fang::http::HttpResponse::ptr response
                    , fang::http::HttpSession::ptr session) = 0;

    /**
     * @Synopsis  获取名称
     */
    const std::string& getName() const { return m_name; }

protected:
    std::string m_name;
};

// 函数式Servlet
class FunctionServlet : public Servlet {
 public:
     typedef std::shared_ptr<FunctionServlet> ptr;
     
     // 函数回调类型
     typedef std::function<int32_t (fang::http::HttpRequest::ptr resquest
                    , fang::http::HttpResponse::ptr response
                    , fang::http::HttpSession::ptr session)> callback;

     FunctionServlet(callback cb);
     virtual int32_t handle(fang::http::HttpRequest::ptr resquest
                    , fang::http::HttpResponse::ptr response
                    , fang::http::HttpSession::ptr session) override;

 private:
     callback m_cb;
};


class IServletCreator {
public:
    typedef std::shared_ptr<IServletCreator> ptr;
    virtual ~IServletCreator() {}

    virtual Servlet::ptr get() const = 0;

    virtual std::string getName() const = 0;
};

class HoldServletCreator : public IServletCreator {
public:
    typedef std::shared_ptr<HoldServletCreator> ptr;
    HoldServletCreator(Servlet::ptr slt)
        : m_servlet(slt) {}
    
    Servlet::ptr get() const override { return m_servlet; }

    std::string getName() const override { return m_servlet->getName(); }

private:
    Servlet::ptr m_servlet;
};


template <typename T>
class ServletCreator : public IServletCreator {
public:
    typedef std::shared_ptr<ServletCreator> ptr;

    ServletCreator() {}

    Servlet::ptr get() const override { return Servlet::ptr(new T); }

    std::string getName() const override { return TypeToName<T>(); }
};

/**
 * @Synopsis  Servlet分发器
 */
class ServletDispatch : public Servlet {
public:
    typedef std::shared_ptr<ServletDispatch> ptr;
    typedef fang::RWMutex RWMutexType;

    ServletDispatch();
    
    virtual int32_t handle(fang::http::HttpRequest::ptr resquest
                    , fang::http::HttpResponse::ptr response
                    , fang::http::HttpSession::ptr session) override;

    // 添加servlet
    void addServlet(const std::string& uri, Servlet::ptr slt);

    void addServlet(const std::string& uri, FunctionServlet::callback cb);

    void addGlobServlet(const std::string& uri, Servlet::ptr slt);

    void addGlobServlet(const std::string& uri, FunctionServlet::callback cb);

    void addServletCreator(const std::string& uri, IServletCreator::ptr creator);
    void addGlobServletCreator(const std::string& uri, IServletCreator::ptr creator);

    template<typename T>
    void addServletCreator(const std::string& uri) {
        addServletCreator(uri, std::make_shared<ServletCreator<T> >());
    }

    template<typename T>
    void addGlobServletCreator(const std::string& uri) {
        addGlobServletCreator(uri, std::make_shared<ServletCreator<T> >());
    }

    void delServlet(const std::string& uri);
    void delGlobServlet(const std::string& uri);

    Servlet::ptr getDefault() const { return m_default; }

    void setDefault(Servlet::ptr v) { m_default = v; }

    Servlet::ptr getServlet(const std::string& uri);
    Servlet::ptr getGlobServlet(const std::string& uri);

    // 通过uri获取servlet 精准---模糊---默认
    Servlet::ptr getMatchedServlet(const std::string& uri);

    void listAllServletCreator(std::map<std::string, IServletCreator::ptr>& infos);
    void listAllGlobServletCreator(std::map<std::string, IServletCreator::ptr>& infos);

private:
    RWMutexType m_mutex;
    // 精准匹配 uri(fang/xxx) -----> servlet
    std::unordered_map<std::string, IServletCreator::ptr> m_datas;

    // 模糊匹配 uri(fang/*) -------> servlet
    std::vector<std::pair<std::string, IServletCreator::ptr> > m_globs;

    // 默认servlet 所有路径没有匹配到时使用
    Servlet::ptr m_default;
    
};

// 默认返回404
class NotFoundServlet : public Servlet {
public:
    typedef std::shared_ptr<NotFoundServlet> ptr;

    NotFoundServlet(const std::string& name);
    virtual int32_t handle(fang::http::HttpRequest::ptr resquest
                    , fang::http::HttpResponse::ptr response
                    , fang::http::HttpSession::ptr session) override;

private:
    std::string m_name;
    std::string m_content;
};


}
}
