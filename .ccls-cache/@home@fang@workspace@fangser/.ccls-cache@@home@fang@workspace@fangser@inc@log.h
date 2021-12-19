
/*
 * @Descripttion:
 * @version:
 * @Author: Fang
 * @email: 2192294938@qq.com
 * @Date: 2021-11-22 13:16:34
 * @LastEditors: Fang
 * @LastEditTime: 2021-11-30 10:25:03
 */
/*
 * @Descripttion:
 * @version:
 * @Author: Fang
 * @email: 2192294938@qq.com
 * @Date: 2021-11-22 13:16:34
 * @LastEditors: Fang
 * @LastEditTime: 2021-11-28 11:19:11
 */

#ifndef __FANG_LOG_H__
#define __FANG_LOG_H__

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/time.h>
#include <map>
#include "helpc.h"
#include "thread.h"

#define OUTIO std::cout

#define FANG_LOG_ROOT() fang::LoggerManager::GetInstance()->getRoot()

#define FANG_LOG_NAME(name) fang::LoggerManager::GetInstance()->getLogger(name)

#define FANG_LOG_LEVEL(logger, level)    \
    if ((logger)->getLevel() <= (level)) \
    fang::LogEventWarp(fang::LogEvent::ptr(new fang::LogEvent((logger), (level), __FILE__, __LINE__, 0, fang::GetThreadId(), fang::GetFiberId(), time(0), "UNKNOW"))).getSS()

#define FANG_LOG_DEBUG(logger) FANG_LOG_LEVEL(logger, fang::LogLevel::DEBUG_LOG_TYPE)
#define FANG_LOG_INFO(logger) FANG_LOG_LEVEL(logger, fang::LogLevel::INFO_LOG_TYPE)
#define FANG_LOG_WARN(logger) FANG_LOG_LEVEL(logger, fang::LogLevel::WARN_LOG_TYPE)
#define FANG_LOG_ERROR(logger) FANG_LOG_LEVEL(logger, fang::LogLevel::ERROR_LOG_TYPE)
#define FANG_LOG_FATAL(logger) FANG_LOG_LEVEL(logger, fang::LogLevel::FATAL_LOG_TYPE)

namespace fang
{

    class Logger;
    //日志级别
    class LogLevel
    {
    public:
        enum Level
        {
            UNDEFINE = 0,
            DEBUG_LOG_TYPE = 1,
            INFO_LOG_TYPE = 2,
            WARN_LOG_TYPE = 3,
            ERROR_LOG_TYPE = 4,
            FATAL_LOG_TYPE = 5
        };

        static const std::string ToString(LogLevel::Level);
        static LogLevel::Level FromString(const std::string &);
    };

    //日志事件
    class LogEvent
    {
    public:
        typedef std::shared_ptr<LogEvent> ptr;

        LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
                 const char *filename, int32_t line, uint32_t elapse,
                 uint32_t thread_id, uint32_t fiber_id, time_t time,
                 const std::string &thread_name);

    public:
        LogLevel::Level getLevel(void) const { return m_level; }
        uint32_t getElapse(void) const { return m_elapse; }
        std::shared_ptr<Logger> getLogger(void) { return m_logger; }
        uint32_t getThreadId(void) const { return m_threadId; }
        uint32_t getFiberId(void) const { return m_fiberId; }
        const std::string &getThreadName(void) { return m_threadName; }
        time_t getTime(void) const { return m_time; }
        const char *getFileName(void) { return m_filename; }
        int32_t getLine(void) const { return m_line; }
        std::string getContent(void) { return m_ss.str(); }
        std::stringstream &getSS(void) { return m_ss; }

    private:
        //文件名
        const char *m_filename;
        //行号
        int32_t m_line;
        //线程id
        uint32_t m_threadId;
        //协程id
        uint32_t m_fiberId;
        //线程名字
        std::string m_threadName;
        //当前时间戳
        time_t m_time;
        /// 程序启动开始到现在的毫秒数
        uint32_t m_elapse;
        //日志级别
        LogLevel::Level m_level;
        //事件输出流
        std::stringstream m_ss;
        //日子器
        std::shared_ptr<Logger> m_logger;
    };
    class LogEventWarp
    {
    public:
        LogEventWarp(LogEvent::ptr event)
            : m_event(event) {}
        ~LogEventWarp();
        std::stringstream &getSS(void) const { return m_event->getSS(); }

    private:
        LogEvent::ptr m_event;
    };

    //日志输出格式器
    class LogFormatter
    {
    public:
        typedef std::shared_ptr<LogFormatter> logFormatter_share_ptr;
        LogFormatter(const std::string &pattern = "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n");

        std::string format(LogEvent::ptr);
        std::ostream &format(std::ostream &, LogEvent::ptr);
        void parse();

    public:
        class FormatItem
        {
        public:
            typedef std::shared_ptr<FormatItem> formatItem_share_ptr;
            virtual ~FormatItem() {}
            virtual void format(std::ostream &os, LogEvent::ptr event) = 0;
        };

    private:
        std::string m_pattern;
        std::vector<FormatItem::formatItem_share_ptr> m_formatItems;
        bool m_error = false;
    };

    //日志输出地
    class LogAppender
    {
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        LogAppender(LogFormatter::logFormatter_share_ptr formatter = LogFormatter::logFormatter_share_ptr(new LogFormatter()))
            : m_formatter(formatter) {}
        virtual ~LogAppender() {}

        virtual void printLog(LogEvent::ptr) = 0;

        void setFormatter(LogFormatter::logFormatter_share_ptr formatter)
        {
            Mutex::Lock lock(m_mutex);
            m_formatter = formatter;
        }

        LogFormatter::logFormatter_share_ptr getFormatter(void)
        {
            Mutex::Lock lock(m_mutex);
            return m_formatter;
        }

    protected:
        LogFormatter::logFormatter_share_ptr m_formatter;
        Mutex m_mutex;
    };

    //日志器
    class Logger
    {
    public:
        typedef std::shared_ptr<Logger> ptr;
        Logger(const std::string &name = "root", LogLevel::Level level = LogLevel::Level::DEBUG_LOG_TYPE);

    public:
        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);

        void addAppender(LogAppender::ptr appender);
        void delAppender(LogAppender::ptr appender);

        const std::string &getName(void) const { return m_name; }
        LogLevel::Level getLevel(void) const { return m_level; }
        void setLevel(LogLevel::Level level) { m_level = level; }

        void log(LogLevel::Level level, LogEvent::ptr event);

    private:
        //日志器名字
        std::string m_name;
        //日志器级别
        LogLevel::Level m_level;
        Mutex m_mutex;
        // Appender集合
        std::list<LogAppender::ptr> m_appenders;
    };

    class ConsoleAppender : public LogAppender
    {
    public:
        void printLog(LogEvent::ptr) override;
    };

    class FileAppender : public LogAppender
    {
    public:
        FileAppender(const std::string &filepath)
            : m_filepath(filepath) {}
        void printLog(LogEvent::ptr) override;

    protected:
        bool reopen();

    private:
        std::string m_filepath;
        std::ofstream m_filestream;
    };

    class LoggerManager
    {
    public:
        static LoggerManager *GetInstance()
        {
            static LoggerManager LoggerMar;
            return &LoggerMar;
        }
        Logger::ptr getLogger(const std::string &name)
        {
            Mutex::Lock lock(m_mutex);
            auto it = m_loggers.find(name);
            if (it != m_loggers.end())
                return it->second;
            Logger::ptr logger(new Logger(name));
            logger->addAppender(LogAppender::ptr(new ConsoleAppender));
            m_loggers[name] = logger;
            return logger;
        }
        Logger::ptr getRoot() { return m_root; };

    protected:
        LoggerManager()
        {
            m_root.reset(new Logger);
            m_root->addAppender(LogAppender::ptr(new ConsoleAppender));
            m_loggers[m_root->getName()] = m_root;
        }

    private:
        Mutex m_mutex;
        Logger::ptr m_root;
        std::map<std::string, Logger::ptr> m_loggers;
    };
}

#endif
