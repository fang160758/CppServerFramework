#ifndef __FANG_DB_MYSQL_H__
#define __FANG_DB_MYSQL_H__

#include <memory>
#include <functional>
#include <mysql/mysql.h>
#include <string>
#include <type_traits>
#include <vector>
#include "db.h"

namespace fang {
struct MySQLTime {
    MySQLTime(time_t t) : ts(t) {}
    time_t ts;
};

bool mysql_time_to_time_t(const MYSQL_TIME& mt, time_t& ts);
bool time_t_to_mysql_time(const time_t& ts, MYSQL_TIME& mt);


class MySQLRes : public ISQData {
public:
    typedef std::shared_ptr<MySQLRes> ptr;
    typedef std::function<bool(MYSQL_ROW row
            , int field_count, int row_no)> data_cb;

    MySQLRes(MYSQL_RES* res, int eno, const char* estr);

    MYSQL_RES* get() const { return m_data.get(); }

    int getErrno() const { return m_errno; }
    const std::string& getErrStr() const { return m_errstr; }

    bool foreach(data_cb cb);

    
    virtual int getDataCount() override;
    virtual int getColumnCount() override;
    virtual int getColumnBytes(int idx) override;
    virtual int getColumnType(int idx) override;
    virtual std::string getColumnName(int idx) override;

    virtual bool isNull(int idx) override;
    virtual int8_t getInt8(int idx) override;
    virtual uint8_t getUint8(int idx) override;
    virtual int16_t getInt16(int idx) override;
    virtual uint16_t getUint16(int idx) override;
    virtual int32_t getInt32(int idx) override;
    virtual uint32_t getUint32(int idx) override;
    virtual int64_t getInt64(int idx) override;
    virtual uint64_t getUint64(int idx) override;
    virtual float getFloat(int idx) override;
    virtual double getDouble(int idx) override;
    virtual std::string getString(int idx) override;
    virtual std::string getBlob(int idx) override;
    virtual time_t getTime(int idx) override;
    virtual bool next() override;

private:
    int m_errno;
    std::string m_errstr;
    MYSQL_ROW m_cur;
    unsigned long* m_curlength;
    std::shared_ptr<MYSQL_RES> m_data;
};

class MySQLStmtRes : public ISQData {
public:
    typedef shared_ptr<MySQLStmtRes> ptr;
    static MySQLStmtRes::ptr Create(std::shared_ptr<MySQLStmt> stmt);
    ~MySQLStmtRes();

    int getErrno() const { return m_errno; }
    const std::string& getErrStr() const { return m_errstr; }

    virtual int getDataCount() override;
    virtual int getColumnCount() override;
    virtual int getColumnBytes(int idx) override;
    virtual int getColumnType(int idx) override;
    virtual std::string getColumnName(int idx) override;

    virtual bool isNull(int idx) override;
    virtual int8_t getInt8(int idx) override;
    virtual uint8_t getUint8(int idx) override;
    virtual int16_t getInt16(int idx) override;
    virtual uint16_t getUint16(int idx) override;
    virtual int32_t getInt32(int idx) override;
    virtual uint32_t getUint32(int idx) override;
    virtual int64_t getInt64(int idx) override;
    virtual uint64_t getUint64(int idx) override;
    virtual float getFloat(int idx) override;
    virtual double getDouble(int idx) override;
    virtual std::string getString(int idx) override;
    virtual std::string getBlob(int idx) override;
    virtual time_t getTime(int idx) override;
    virtual bool next() override;

private:
    MySQLStmtRes(std::shared_ptr<MySQLStmt> stmt, int eno, const std::string& estr);

    struct Data {
        Data();
        ~Data();
        void alloc(size_t size);
        my_bool is_null;
        my_bool error;
        enum_field_types type;
        unsigned long length;
        int32_t data_length;
        char* data;
    };

private:
    int m_errno;
    std::string m_errstr;
    std::shared_ptr<MySQLStmt> m_stmt;
    std::vector<MYSQL_BIND> m_binds;
    std::vector<Data> m_datas;
};

class MySQLStmt : public IStmt
                  , public std::enable_shared_from_this<MySQLStmt> {

public:
    typedef std::shared_ptr<MySQLStmt> ptr;
    static MySQLStmt::ptr Create(MySQL::ptr db, const std::string& stmt);
    ~MySQLStmt();

    int bind(int idx, const int8_t& value);
    int bind(int idx, const uint8_t& value);
    int bind(int idx, const int16_t& value);
    int bind(int idx, const uint16_t& value);
    int bind(int idx, const int32_t& value);
    int bind(int idx, const uint32_t& value);
    int bind(int idx, const int64_t& value);
    int bind(int idx, const uint64_t& value);
    int bind(int idx, const float& value);
    int bind(int idx, const double& value);
    int bind(int idx, const std::string& value);
    int bind(int idx, const char* value);
    int bind(int idx, const void* value, int len);

    int bind(int idx);

    virtual int bindInt8(int idx, const int8_t& value) override;
    virtual int bindUint8(int idx, const uint8_t& value) override;
    virtual int bindInt16(int idx, const int16_t& value) override;
    virtual int bindUint16(int idx, const uint16_t& value) override;
    virtual int bindInt32(int idx, const int32_t& value) override;
    virtual int bindUint32(int idx, const uint32_t& value) override;
    virtual int bindInt64(int idx, const int64_t& value) override;
    virtual int bindUint64(int idx, const uint64_t& value) override;
    virtual int bindFloat(int idx, const float& value) override;
    virtual int bindDouble(int idx, const double& value) override;
    virtual int bindString(int idx, const char* value) override;
    virtual int bindString(int idx, const std::string& value) override;
    virtual int bindBlob(int idx, const void* value, int64_t size) override;
    virtual int bindBlob(int idx, const std::string& value) override;
    virtual int bindTime(int idx, const time_t& value) override;
    virtual int bindNull(int idx) override;

    virtual int execute() override;
    virtual int64_t getLastInsertId() override;
    virtual ISQData::ptr query() override;

    virtual int getError() override;
    virtual std::string getErrorStr() override;

private:
    MySQLStmt(MySQL::ptr db, MYSQL_STMT* stmt);

private:
    MySQL::ptr m_mysql;
    MYSQL_STMT* m_stmt;
    std::vector<MYSQL_BIND> m_binds; 
};






class MySQL : public IDB
              , public std::enable_shared_from_this<MySQL> {

public:
    typedef std::shared_ptr<MySQL> ptr;

    MySQL(const std::map<std::string, std::string>& args);

    bool connect();
    bool ping();

    virtual int execute(const char* format, ...) override;
    virtual int execute(const std::string& sql) override;
    int execute(const char* format, va_list ap);

    int64_t getLastInsertId() override;
    std::shared_ptr<MySQL> getMySQL();
    std::shared_ptr<MYSQL> getRaw();

    uint64_t getAffectedRows();

    ISQData::ptr query(const char* format, ...) override;
    ISQData::ptr query(const std::string& sql) override;
    ISQData::ptr query(const char* format, va_list ap);

    ITransaction::ptr openTransaction(bool auto_commit) override;
    fang::IStmt::ptr perpare(const std::string& sql) override;

    template<typename... Args>
    int execStmt(const char* stmt, Args&&... args);

    template<typedef... Args>
    ISQData::ptr queryStmt(const char* stmt, Args&&... args);

    const char* cmd();
    bool use(const std::string& dbname);

    int getErrno() override;
    std::string getErrStr() override;
    uint64_t getLastInsertId();

private:
    bool isNeedCheck();

private:
    std::map<std::string, std::string> m_params;
    std::shared_ptr<MYSQL> m_mysql;

    std::string m_cmd;
    std::string m_dbname;

    uint64_t m_lastUsedTime;
    bool m_hasError;
    int32_t m_poolSize;
};

}


#endif
