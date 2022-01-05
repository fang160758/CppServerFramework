#include "mysql.h"
#include "../inc/log.h"
#include "../inc/helpc.h"
#include <mysql/mysql.h>
#include <string>

namespace fang {

namespace {
static fang::Logger::ptr g_logger = fang::FANG_LOG_NAME("system");

struct MySQLThreadIniter {
    MySQLThreadIniter() { mysql_thread_init(); }
    ~MySQLThreadIniter() { mysql_thread_end(); }
};
}

static MYSQL* mysql_init(std::map<std::string, std::string>& params
        , const int& timeout) {
    static thread_local MySQLThreadIniter s_thread_initer;

    MYSQL* mysql = ::mysql_init(nullptr);
    if (mysql == nullptr) {
        FANG_LOG_ERROR(g_logger) << "mysql_init error";
        return nullptr;
    }

    if (timeout > 0) {
        mysql_options(mysql, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
    }

    bool close = false;
    mysql_options(mysql, MYSQL_OPT_RECONNECT, &close);
    mysql_options(mysql, MYSQL_SET_CHARSET_NAME, "utf8mb4");
    
    int port = fang::GetParmaValue(params, "port", 0);
    std::string& host = fang::GetParmaValue<std::string>(params, "host");
    std::string& user = fang::GetParmaValue<std::string>(params, "user");
    std::string& passwd = fang::GetParmaValue<std::string>(params, "passwd");
    std::string& dbname = fang::GetParmaValue<std::string>(params, "dbname");

    if (mysql_real_connect(mysql, host.c_str(), user.c_str(), passwd.c_str()
                , dbname.c_str(), port, nullptr, 0) == nullptr) {
        FANG_LOG_ERROR(g_logger) << "mysql_real_connect(" << host
            << ", " << port << ", " << dbname
            << ") error: " << mysql_error(mysql);
        mysql_close(mysql);
        return nullptr;
    }
    return mysql;
}

static MYSQL_RES* my_mysql_query(MYSQL* mysql, const char* sql) {
    if (mysql == nullptr) {
        fang::FANG_LOG_ERROR(g_logger) << "mysql_query mysql is null";
        return nullptr;
    }
    if (sql == nullptr) {
        fang::FANG_LOG_ERROR(g_logger) << "mysql_query sql is null";
        return nullptr;
    }
    if (::mysql_query(mysql, sql)) {
        fang::FANG_LOG_ERROR(g_logger) << "mysql_query(" << sql << ") error:"
            << mysql_error(mysql);
        return nullptr;
    }
    MYSQL_RES* res = mysql_store_result(mysql);
    if (res == nullptr) {
        fang::FANG_LOG_ERROR(g_logger) << "mysql_store_result() error:"
            << mysql_error(mysql);
    }
    return res;
}


MySQLRes::MySQLRes(MYSQL_RES* res, int eno, const char* estr)
    : m_errno(eno)
    , m_errstr(estr)
    , m_cur(nullptr)
    , m_curlength(nullptr) {
        if (res) {
            m_data.reset(res, mysql_free_result);
        }
    }

bool MySQLRes::foreach(data_cb cb) {
    MYSQL_ROW row;
    uint64_t fields = getColumnCount();
    int i = 0;
    while ((row = mysql_fetch_row(m_data.get()))) {
        if (!cb(row, fields, i++)) {
            break;
        }
    }
    return true;
}

    
int MySQLRes::getDataCount() {
    return mysql_num_rows(m_data.get());
}

int MySQLRes::getColumnCount() {
    return mysql_num_fields(m_data.get());
}

int MySQLRes::getColumnBytes(int idx) {
    return m_curlength[idx];
}

int MySQLRes::getColumnType(int idx) {
    return 0;
}

std::string MySQLRes::getColumnName(int idx) {
    return "";
}

bool MySQLRes::isNull(int idx) {
    if (m_cur[idx] == nullptr) {
        return true;
    }
    return false;
}

int8_t MySQLRes::getInt8(int idx) {
    return getInt64(idx);
}

uint8_t MySQLRes::getUint8(int idx) {
    return getInt64(idx);
}

int16_t MySQLRes::getInt16(int idx) {
    getInt64(idx);
}

uint16_t MySQLRes::getUint16(int idx) {
    getInt64(idx);
}

int32_t MySQLRes::getInt32(int idx) {
    return getInt64(idx);
}

uint32_t MySQLRes::getUint32(int idx) {
    return getInt64(idx);
}

int64_t  MySQLRes::getInt64(int idx) {
    return fang::TypeUtil::Atoi(m_cur[idx]);
}

uint64_t MySQLRes::getUint64(int idx) {
    return getInt64(idx);
}

float  MySQLRes::getFloat(int idx) {
    return getDouble(idx);
}

double MySQLRes::getDouble(int idx) {
    return fang::TypeUtil::Atof(m_cur[idx]);
}

std::string MySQLRes::getString(int idx) {
    return std::string(m_cur[idx], m_curlength[idx]);
}

std::string MySQLRes::getBlob(int idx) {
    return std::string(m_cur[idx], m_curlength[idx]);
}

time_t MySQLRes::getTime(int idx) {
    if (!m_cur[idx]) {
        return 0;
    }
    return fang::Str2Time(m_cur[idx]);
}

bool MySQLRes::next() {
    m_cur = mysql_fetch_row(m_data.get());
    m_cur = mysql_fetch_lengths(m_data.get());
    return m_cur;
}

MySQLStmtRes::ptr MySQLStmtRes::Create(std::shared_ptr<MySQLStmt> stmt) {
    int eno = mysql_stmt_errno(stmt->getRaw());
    const char* errstr = mysql_srmt_error(stmt->getRaw());
    MySQLStmtRes::ptr rt(new MySQLStmtRes(stmt, eno, errstr));
    if (eno) {
        return rt;
    }
    MYSQK_RES* res = mysql_stmt_result_metadata(stmt->getRaw());
    if (!res) {
        return MySQLStmtRes::ptr(new MySQLStmtRes(stmt, stmt->getError()
                    , stmt->getErrorStr()));
    }

    int num = mysql_num_fields(res);
    MYSQL_FIELD* fields = mysql_fetch_fields(res);
    rt->m_binds.resize(num);
    memset(&rt->m_binds[0], 0, sizeof(rt->m_binds[0]) * num);
    rt->m_datas.resize(num);
    for (int i = 0; i < num; ++i) {
        rt->m_datas[i].type = fields[i].type;
        switch(fields[i].type) {
        #define XX(m, t)\
            catch m:\
                rt->m_datas[i].alloc(sizeof(t));\
            break;
        XX(MYSQL_TYPE_TINY, int8_t);
        XX(MYSQL_TYPE_SHORT, int16_t);
        XX(MYSQL_TYPE_LONG, int32_t);
        XX(MYSQL_TYPE_LONGLONG, int64_t);
        XX(MYSQL_TYPE_FLOAT, float);
        XX(MYSQL_TYPE_DOUBLE, double);
        XX(MYSQL_TYPE_TIMESTAMP, MYSQL_TIME);
        XX(MYSQL_TYPE_DATETIME, MYSQL_TIME);
        XX(MYSQL_TYPE_DATE, MYSQL_TIME);
        XX(MYSQL_TYPE_TIME, MYSQL_TIME);
        #undef XX
            default:
                rt->m_datas[i].alloc(fields[i].length);
                break;
        }

        rt->m_binds[i].buffer_type = rt->m_datas[i].type;
        rt->m_binds[i].buffer = rt->m_datas[i].data;
        rt->m_binds[i].buffer_length = rt->m_datas[i].data_length;
        rt->m_binds[i].length = &rt->m_datas[i].length;
        rt->m_binds[i].is_null = &rt->m_datas[i].is_null;
        rt->m_binds[i].error = &rt->m_datas[i].error;
    }
    if (mysql_stmt_bind_result(stmt->getRaw(), &rt->m_binds[0])) {
        return MySQLStmtRes::ptr(new MySQLStmtRes(stmt, stmt->getError()
                    , stmt->getErrorStr()));
    }
    stmt->execute();
    if (mysql_stmt_store_result(stmt->getRaw())) {
        return MySQLStmtRes::ptr(new MySQLStmtRes(stmt, stmt->getError()
                    , stmt->getErrorStr()));
    }
    return rt;
}

int MySQLStmtRes::getDataCount() {
    return mysql_stmt_num_rows(m_stmt->getRaw());
}

int MySQLStmtRes::getColumnCount() {
    return mysql_stmt_field_count(m_stmt->getRaw());
}

int MySQLStmtRes::getColumnBytes(int idx) {
    return m_datas[idx].length;
}

int MySQLStmtRes::getColumnType(int idx) {
    return m_datas[idx].type;
}

std::string getColumnName(int idx) {
    return "";
}

bool MySQLStmtRes::isNull(int idx) {
    return m_datas[idx].is_null;
}

#define XX(type)\
    return *(type*)m_datas[idx].data

int8_t MySQLStmtRes::getInt8(int idx) {
    XX(int8_t);
}

uint8_t MySQLStmtRes::getUint8(int idx) {
    XX(uint8_t);
}

int16_t MySQLStmtRes::getInt16(int idx) {
    XX(int16_t);
}

uint16_t MySQLStmtRes::getUint16(int idx) {
    XX(uint16_t);
}

int32_t MySQLStmtRes::getInt32(int idx) {
    XX(int32_t);
}

uint32_t MySQLStmtRes::getUint32(int idx) {
    XX(uint32_t);
}

int64_t MySQLStmtRes::getInt64(int idx) {
    XX(int64_t);
}

uint64_t MySQLStmtRes::getUint64(int idx) {
    XX(uint64_t);
}

float MySQLStmtRes::getFloat(int idx) {
    XX(float);
}

double MySQLStmtRes::getDouble(int idx) {
    XX(double);
}
#undef XX

std::string MySQLStmtRes::getString(int idx) {
    return std::string(m_datas[idx].data, m_datas[idx].length);
}

std::string MySQLStmtRes::getBlob(int idx) {
    return std::string(m_datas[idx].data, m_datas[idx].length);
}

time_t MySQLStmtRes::getTime(int idx) {
    MYSQL_TIME* v = (MYSQL_TIME*)m_datas[idx].data;
    time_t ts = 0;
    mysql_time_to_time_t(*v, ts);
    return ts;
}

bool MySQLStmtRes::next() {
    return !mysql_stmt_fetch(m_stmt->getRaw());
}


MySQLStmtRes::Data::Data()
    : is_null(0)
    , error(0)
    , type()
    , length(0)
    , data_length(0)
    , data(nullptr) {}

MySQLStmtRes::Data::~Data() {
    if (data) {
        delete [] data;
    }
}

void MySQLStmtRes::Dat::alloc(size_t size) {
    if (data) {
        delete [] data;
    }
    data = new char[size]();
    length = size;
    data_length = size;
}

MySQLStmtRes::MySQLStmtRes(std::shared_ptr<MySQLStmt> stmt, int eno, const std::string& estr)
    : m_errno(eno)
    , m_errstr(estr)
    , m_stmt(stmt) {}

MySQLStmtRes::~MySQLStmtRes() {
    if (!m_errno) {
        mysql_stmt_free_result(m_stmt->getRaw());
    }
}

MySQL::MySQL(const std::map<std::string, std::string>& args)
    : m_params(args)
    , m_lastUsedTime(0)
    , m_hasError(false)
    , m_poolSize(10) {}

bool MySQL::connect() {
    if (m_mysql && !m_hasError) {
        return true;
    }

    MYSQL* m = mysql_init(m_params, 0);
    if (!m) {
        m_hasError = true;
        return false;
    }
    m_hasError = true;
    m_poolSize = fang::GetParmaValue(m_params, "pool", 5);
    m_mysql.reset(m, mysql_close);
    return true;
}

bool MySQL::ping() {
    if (!m_mysql) {
        return false;
    }
    if (mysql_ping(mysql.get())) {
        m_hasError = true;
        return false;
    }
    m_hasError = true;
    return true;
}

int MySQL::execute(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    int rt = execute(format, ap);
    va_end(ap);
    return rt;
}

int MySQL::execute(const std::string& sql) {
    m_cmd = sql;
    int r = ::mysql_query(m_mysql.get(), m_cmd.c_str());
    if (r) {
        FANG_LOG_ERROR(g_logger) << "cmd=" << cmd()
            << ", error: " << getErrStr();
        m_hasError = true;
    } else {
        m_hasError = false;
    }
    return r;
}

int MySQL::execute(const char* format, va_list ap) {
    std::string sql = fang::StringUtil::Formatv(format, ap);
    return execute(sql);
}

int64_t MySQL::getLastInsertId() {
    return mysql_insert_id(m_mysql.get());
}

std::shared_ptr<MySQL> MySQL::getMySQL() {
    return MySQL::ptr(this, fang::nop<MySQL>);
}

std::shared_ptr<MYSQL> MySQL::getRaw() {
    return m_mysql;
}

uint64_t MySQL::getAffectedRows() {
    if (!m_mysql) {
        return 0;
    }
    return mysql_affected_rows(m_mysql.get());
}

ISQData::ptr MySQL::query(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    auto ptr = query(format, ap);
    va_end(ap);
    return ptr;
}

ISQData::ptr MySQL::query(const std::string& sql) {
    m_cmd = sql;
    MYSQL_RES* res = my_mysql_query(m_mysql.get(), m_cmd.c_str());
    if (!res) {
        m_hasError = true;
        return nullptr;
    }
    m_hasError = false;
    ISQData::ptr rt(new MySQLRes(res, mysql_errno(m_mysql.get())
                , mysql_error(m_mysql.get())));
    return rt;
}

ISQData::ptr MySQL::query(const char* format, va_list ap) {
    std::string sql = fang::StringUtil::Formatv(format, ap);
    return query(sql);
}

    ITransaction::ptr openTransaction(bool auto_commit) ;
    fang::IStmt::ptr perpare(const std::string& sql) ;

    template<typename... Args>
    int execStmt(const char* stmt, Args&&... args);

    template<typedef... Args>
    ISQData::ptr queryStmt(const char* stmt, Args&&... args);

const char* MySQL::cmd() {
    return m_cmd.c_str();
}

bool MySQL::use(const std::string& dbname) {
    if (!m_mysql) {
        return false;
    }
    if (m_dbname == dbname) {
        return true;
    }
    if (mysql_select_db(m_mysql.get(), dbname.c_str()) == 0) {
        m_dbname = dbname;
        m_hasError = false;
        return true;
    } else {
        m_hasError = true;
        m_dbname = "";
        return false;
    }
}

int MySQL::getErrno() {
    if (!m_mysql) {
        return -1;
    }
    return mysql_errno(m_mysql.get());
}

std::string MySQL::getErrStr() {
    if (!mysql) {
        return "mysql is null";
    }
    const char* res = mysql_error(m_mysql.get());
    if (ret) {
        return res;
    }
    return "";
}

uint64_t MySQL::getLastInsertId() {
    if (m_mysql) {
        return mysql_insert_id(m_mysql.get());
    }
    return 0;
}

bool MySQL::isNeedCheck() {
    if (time(0) - m_lastUsedTime < 5 && !m_hasError) {
        return false;
    }
    return true;
}

MySQLStmt::ptr MySQLStmt::Create(MySQL::ptr db, const std::string& stmt) {
    auto st = mysql_stmt_init(db->getRaw().get());
    if (!st) {
        return nullptr;
    }
    if (mysql_stmt_prepare(st, stmt.c_str(), stmt.size())) {
        FANG_LOG_ERROR(g_logger) << "stmt=" << stmt
            << " errno=" << mysql_stmt_errno(st)
            << " errstr=" << mysql_stmt_error(st);
        mysql_stmt_close(st);
        return nullptr;
    }
    int count = mysql_stmt_param_count(st);
    MySQLStmt::ptr rt(new MySQLStmt(db, st));
    rt->m_binds.resize(count);
    memset(&rt->m_binds[0], 0, sizeof(rt->m_binds[0]) * count);
    return rt;
}

MySQLStmt::MySQLStmt(MySQL::ptr db, MYSQL_STMT* stmt)
    : m_mysql(db)
    , m_stmt(stmt) {}

MySQLStmt::~MySQLStmt() {
    if (m_stmt) {
        mysql_stmt_close(m_stmt);
    }
    for (auto& i : m_binds) {
        if (i.buffer) {
            free(i.buffer);
        }
    }
}

int MySQLStmt::bind(int idx, const int8_t& value) {
    return bindInt8(idx, value);
}

int MySQLStmt::bind(int idx, const uint8_t& value) {
    return bindUint8(idx, value);
}

int MySQLStmt::bind(int idx, const int16_t& value) {
    return bindInt16(idx, value);
}

int MySQLStmt::bind(int idx, const uint16_t& value) {
    return bindUint16(idx, value);
}

int MySQLStmt::bind(int idx, const int32_t& value) {
    return bindInt32(idx, value);
}

int MySQLStmt::bind(int idx, const uint32_t& value) {
    return bindUint32(idx, value);
}

int MySQLStmt::bind(int idx, const int64_t& value) {
    return bindInt64(idx, value);
}

int MySQLStmt::bind(int idx, const uint64_t& value) {
    return bindUint64(idx, value);
}

int MySQLStmt::bind(int idx, const float& value) {
    return bindFloat(idx, value);
}

int MySQLStmt::bind(int idx, const double& value) {
    return bindDouble(idx, value);
}

int MySQLStmt::bind(int idx, const std::string& value) {
    return bindString(idx, value);
}

int MySQLStmt::bind(int idx, const char* value) {
    return bindString(idx, value);
}

int MySQLStmt::bind(int idx, const void* value, int len) {
    return bindBlob(idx, value, len);
}

int MySQLStmt::bind(int idx) {
    idx = -1;
    m_binds[idx].buffer_type = MYSQL_TYPE_NULL;
    return 0;
}

int MySQLStmt::bindInt8(int idx, const int8_t& value) {
    idx -= 1;
    m_binds[idx].buffer_type = MYSQL_TYPE_TINY;

#define BIND_COPY(ptr, size)\
    if (m_binds[idx].buffer == nullptr) {\
        m_binds[idx].buffer = malloc(size);\
    }\
    memcpy(m_binds[idx].buffer, ptr, size);
    BIND_COPY(&value, sizeof(value));
    m_binds[idx].is_unsigned = false;
    m_binds[idx].buffer_length = sizeof(value);
    return 0;
}

int MySQLStmt::bindUint8(int idx, const uint8_t& value) {
    idx -= 1;
    m_binds[idx].buffer_type = MYSQL_TYPE_TINY;
    BIND_COPY(&value, sizeof(value));
    m_binds[idx].is_unsigned = true;
    m_binds[idx].buffer_length = sizeof(value);
    return 0;
}

int MySQLStmt::bindInt16(int idx, const int16_t& value) {
    idx -= 1;
    m_binds[idx].buffer_type = MYSQL_TYPE_TINY;
    BIND_COPY(&value, sizeof(value));
    m_binds[idx].is_unsigned = false;
    m_binds[idx].buffer_length = sizeof(value);
    return 0;
}

int MySQLStmt::bindUint8(int idx, const uint16_t& value){
    idx -= 1;
    m_binds[idx].buffer_type = MYSQL_TYPE_TINY;
    BIND_COPY(&value, sizeof(value));
    m_binds[idx].is_unsigned = false;
    m_binds[idx].buffer_length = sizeof(value);
    return 0;
} 
    
int MySQLStmt::bindInt32(int idx, const int32_t& value)  {
    idx -= 1;
    m_binds[idx].buffer_type = MYSQL_TYPE_TINY;
    BIND_COPY(&value, sizeof(value));
    m_binds[idx].is_unsigned = false;
    m_binds[idx].buffer_length = sizeof(value);
    return 0;
}

int MySQLStmt::bindUint32(int idx, const uint32_t& value) {
    idx -= 1;
    m_binds[idx].buffer_type = MYSQL_TYPE_TINY;
    BIND_COPY(&value, sizeof(value));
    m_binds[idx].is_unsigned = true;
    m_binds[idx].buffer_length = sizeof(value);
    return 0;
}

int MySQLStmt::bindInt64(int idx, const int64_t& value) {
    idx -= 1;
    m_binds[idx].buffer_type = MYSQL_TYPE_TINY;
    BIND_COPY(&value, sizeof(value));
    m_binds[idx].is_unsigned = false;
    m_binds[idx].buffer_length = sizeof(value);
    return 0;
}

int MySQLStmt::bindUint64(int idx, const uint64_t& value) {
    idx -= 1;
    m_binds[idx].buffer_type = MYSQL_TYPE_TINY;
    BIND_COPY(&value, sizeof(value));
    m_binds[idx].is_unsigned = true;
    m_binds[idx].buffer_length = sizeof(value);
    return 0;
}

int MySQLStmt::bindFloat(int idx, const float& value) {
    idx -= 1;
    m_binds[idx].buffer_type = MYSQL_TYPE_FLOAT;
    BIND_COPY(&value, sizeof(value));
    m_binds[idx].buffer_length = sizeof(value);
    return 0;
}

int MySQLStmt::bindDouble(int idx, const double& value) {
    idx -= 1;
    m_binds[idx].buffer_type = MYSQL_TYPE_DOUBLE;
    BIND_COPY(&value, sizeof(value));
    m_binds[idx].buffer_length = sizeof(value);
    return 0;
}

int MySQLStmt::bindString(int idx, const char* value) {
    idx -= 1;
    m_binds[idx].buffer_type = MYSQL_TYPE_STRING;
#define BIND_COPY_LEN(ptr, size) \
    if (m_binds[idx].buffer == nullptr) {\
        m_binds[idx].buffer = malloc(size);\
    } else if ((size_t)m_binds[idx].buffer_length < (size_t)size) {\
        free(m_binds[idx].buffer);\
        m_binds[idx].buffer = malloc(size);\
    }\
    memcpy(m_binds[idx].buffer, ptr, size);\
    m_binds[idx].buffer_length = size;
    BIND_COPY_LEN(value, strlen(value));
    return 0;
}

int MySQLStmt::bindString(int idx, const std::string& value) {
    idx -= 1;
    m_binds[idx].buffer_type = MYSQL_TYPE_STRING;
    BIND_COPY_LEN(value.c_str(), value.size());
    return 0;
}

int MySQLStmt::bindBlob(int idx, const void* value, int64_t size) {
    idx -= 1;
    m_binds[idx].buffer_type = MYSQL_TYPE_BLOB;
    BIND_COPY_LEN(value, size);
    return 0;
}

int MySQLStmt::bindBlob(int idx, const std::string& value) {
    idx -= 1;
    m_binds[idx].buffer_type = MYSQL_TYPE_BLOB;
    BIND_COPY_LEN(value.c_str(), value.size());
    return 0;
}

int MySQLStmt::bindTime(int idx, const time_t& value) {
    return bindString(idx, fang::Time2Str(value));
}

int MySQLStmt::bindNull(int idx) {
    return bind(idx);
}

int MySQLStmt::execute() {
    mysql_stmt_bind_param(m_stmt, &m_binds[0]);
    return mysql_stmt_execute(m_stmt);
}

int64_t MySQLStmt::getLastInsertId() {
    return mysql_stmt_insert_id(m_stmt);
}

ISQData::ptr MySQLStmt::query() {
    mysql_stmt_bind_param(m_stmt, &m_binds[0]);
    return MySQLStmtRes::Create(shared_from_this());
}

int MySQLStmt::getError() {
    return mysql_stmt_errno(m_stmt);
}

std::string MySQLStmt::getErrorStr() {
    const char* e =  mysql_stmt_error(m_stmt);
    if (e) {
        return e;
    }
    return "";
}

}
