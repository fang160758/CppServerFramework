#ifndef __FANG_TCP_SERVER_
#define __FANG_TCP_SERVER_
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include "address.h"
#include "socket.h"
#include "iomanager.h"
#include "singleton.h"
#include "config.h"


namespace fang {


struct TcpServerConf {

    typedef std::shared_ptr<TcpServerConf> ptr;

    std::vector<std::string> address;
    int keepalive = 0;
    int timeout = 1000 * 2 * 60;
    int ssl = 0;

    std::string id;
    std::string type = "http";
    std::string name;
    std::string cerf_file;
    std::string key_file;
    std::string accept_worker;
    std::string io_worker;
    std::string process_worker;
    std::map<std::string, std::string> args;

    bool isValid() const {
        return !address.empty();
    }

    bool operator==(const TcpServerConf& oth) const {
        return address == oth.address
            && keepalive == oth.keepalive
            && timeout == oth.timeout
            && name == oth.name
            && ssl == oth.ssl
            && cerf_file == oth.cerf_file
            && key_file == oth.key_file
            && accept_worker = oth.accept_worker
            && io_worker == oth.io_worker
            && process_worker == oth.process_worker
            && args = oth.args
            && id = oth.id
            && type = oth.type;
    }

    };

    template<>
    class LexicalCast<std::string, TcpServerConf> {
    public:
        TcpServerConf operator()(const std::string& v) {
            YAML::Node node = YAML::Load(v);
            TcpServerConf conf;
            conf.id = node["id"].as<std::string>(conf.id);
            conf.type = node["type"].as<std::string>(conf.type);
            conf.keepalive = node["keepalive"].as<int>(conf.keepalive);
            conf.timeout = node["timeout"].as<int>(conf.timeout);
            conf.name = node["name"].as<std::string>(conf.name);
            conf.ssl = node["ssl"].as<int>(conf.ssl);
            conf.cerf_file = node["cerf_file"].as<std::string>(conf.cerf_file);
            conf.key_file = node["key_file"].as<std::string>(conf.key_file);
            conf.accept_worker = node["accept_worker"].as<std::string>(conf.accept_worker);
            conf.process_worker = node["process_worker"].as<std::string>(conf.accept_worker);
            conf.io_worker = node["io_worker"].as<std::string>(conf.io_worker);
            conf.args = LexicalCast<std::string
                , std::map<std::string, std::string> >()(node["args"].as<std::string>(""));
            if (node["address"].IsDefined()) {
                for (size_t i = 0; i < node["address"].size(); ++i) {
                    conf.address.push_back(node["address"][i].as<std::string>());
                }
            }
            return conf;
        }
    };


    template<>
    class LexicalCast<TcpServerConf, std::string> {
    public:
        std::string operator()(const TcpServerConf& conf) {
            YAML::Node node;
            node["id"] = conf.id;
            node["type"] = conf.type;
            node["name"] = conf.name;
            node["keepalive"] = conf.keepalive;
            node["timeout"] = conf.timeout;
            node["ssl"] = conf.ssl;
            node["cert_file"] = conf.cert_file;
            node["key_file"] = conf.key_file;
            node["accept_worker"] = conf.accept_worker;
            node["io_worker"] = conf.io_worker;
            node["process_worker"] = conf.process_worker;
            node["args"] = YAML::Load(LexicalCast<std::map<std::string, std::string>
                    , std::string>()(conf.args));
            for (auto& i : conf.address) {
                node["address"].push_back(i);
            }
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
};

class TcpServer : std::enable_shared_from_this<TcpServer>
                , Noncopyable {

public:
    std::shared_ptr<TcpServer> ptr;

    TcpServer(fang::IOManager* worker = fang::IoManager::GetThis()
            , fang::IOManager* io_worker = fang::IoManager::GetThis()
            , fang::IOManager* accept_worker = fang::IOManager::GetThis());

    ~TcpServer();

    /**
     * @Synopsis  ????????????
     *
     * @Param[in] addr  ??????
     * @Param[in] ssl   ????????????ssl
     *
     * @Returns   ??????????????????
     */
    virtual bool bind(fang::Address::ptr addr, bool ssl = false);

    /**
     * @Synopsis  ??????????????????, ?????????????????????????????????????????????????????????false
     *
     * @Param[in] ????????????
     * @Param[in] ???????????????????????????
     * @Param[in] ssl ????????????ssl
     *
     * @Returns   ??????????????????
     */
    virtual bool bind(const std::vector<fang::Address::ptr>& addrs
            , std::vector<Address::ptr>& fails
            , bool ssl = false);

    /**
     * @Synopsis  
     *
     * @Param[in] cerf_file
     * @Param[in] key_file
     *
     * @Returns   
     */
    bool loadCertificates(const std::string& cerf_file, const std::string& key_file);


    /**
     * @Synopsis  ?????????????????????bind ???????????????
     */
    virtual bool start();

    /**
     * @Synopsis  ????????????
     */
    virtual void stop();

    /**
     * @Synopsis  ????????????????????????
     */
    uint64_t getRecvTimeout() const { return m_recvTimeout; }

    /**
     * @Synopsis  ??????????????????
     */
    std::string getName() const { return m_name; }

    /**
     * @Synopsis  ????????????????????????
     */
    void setRecvTimeout(uint64_t v) { m_recvTimeout = v; }

    /**
     * @Synopsis  ??????????????????
     */
    virtual void setName(const std::string& v) { m_name = v; }

    /**
     * @Synopsis  ??????????????????
     */
    bool isStop() const { return m_isStop; };

    /**
     * @Synopsis  ?????????
     */
    virtual std::string toString(const std::string& prefix = "");
    
    /**
     * @Synopsis  ??????????????????socket
     */
    std::vector<Socket::ptr> getSocks const { return m_socks; }

protected:

    /**
     * @Synopsis  ??????????????????socket???
     */
    virtual void handleClient(Socket::ptr client);

    /**
     * @Synopsis  ????????????socket??????
     */
    virtual void [<0;70;26M]startAccept(Socket::ptr sock);

protected:
    std::vector<Socket::ptr> m_socks;   // ??????socket??????
    IOManager* m_worker;                // socket???????????????
    IOManager* m_ioWorker;
    IOManager* m_acceptWorker;          // ???socket????????????????????????
    uint64_t m_recvTimeout;             // ??????????????????
    std::string m_name;                 // ???????????????
    std::string m_type = "tcp";         // ???????????????
    bool m_isStop;                      // ??????????????????
    bool m_ssl = false;

    TcpServerConf::ptr m_conf;
}
}






#endif
