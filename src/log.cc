/*
 * @Descripttion:
 * @version:
 * @Author: Fang
 * @email: 2192294938@qq.com
 * @Date: 2021-11-22 13:58:51
 * @LastEditors: Fang
 * @LastEditTime: 2021-11-29 15:17:40
 */
#include "log.h"
#include "helpc.h"
#include <map>
#include <functional>
namespace fang
{

    const std::string LogLevel::ToString(LogLevel::Level level)
    {
        switch (level)
        {
#define DEF_XX(type)     \
    case LogLevel::type: \
        return #type
            DEF_XX(DEBUG_LOG_TYPE);
            DEF_XX(INFO_LOG_TYPE);
            DEF_XX(WARN_LOG_TYPE);
            DEF_XX(ERROR_LOG_TYPE);
            DEF_XX(FATAL_LOG_TYPE);
#undef DEF_XX
        default:
            return "UNDEFINE";
        }
        return "UNDEFINE";
    }

    LogLevel::Level LogLevel::FromString(const std::string &str)
    {
#define DEF_XX(level, v) \
    if (str == #v)       \
    return LogLevel::level
        DEF_XX(DEBUG_LOG_TYPE, DEBUG_LOG_TYPE);
        DEF_XX(INFO_LOG_TYPE, INFO_LOG_TYPE);
        DEF_XX(WARN_LOG_TYPE, WARN_LOG_TYPE);
        DEF_XX(ERROR_LOG_TYPE, ERROR_LOG_TYPE);
        DEF_XX(FATAL_LOG_TYPE, FATAL_LOG_TYPE);
#undef DEF_XX
        return LogLevel::UNDEFINE;
    }

    LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
                       const char *filename, int32_t line, uint32_t elapse,
                       uint32_t thread_id, uint32_t fiber_id, time_t time,
                       const std::string &thread_name)
        : m_filename(filename),
          m_line(line),
          m_threadId(thread_id),
          m_fiberId(fiber_id),
          m_threadName(thread_name),
          m_time(time),
          m_elapse(elapse),
          m_level(level), 
          m_logger(logger) {}

    void Logger::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            Mutex::Lock lock(m_mutex);
            for (auto it = m_appenders.begin(); it != m_appenders.end(); it++)
                (*it)->printLog(event);
        }
    }
    LogEventWarp::~LogEventWarp() { m_event->getLogger()->log(m_event->getLevel(), m_event); }
    void Logger::debug(LogEvent::ptr event)
    {
        log(LogLevel::Level::DEBUG_LOG_TYPE, event);
    }

    void Logger::info(LogEvent::ptr event)
    {
        log(LogLevel::Level::INFO_LOG_TYPE, event);
    }

    void Logger::warn(LogEvent::ptr event)
    {
        log(LogLevel::Level::WARN_LOG_TYPE, event);
    }

    void Logger::error(LogEvent::ptr event)
    {
        log(LogLevel::Level::ERROR_LOG_TYPE, event);
    }

    void Logger::fatal(LogEvent::ptr event)
    {
        log(LogLevel::Level::FATAL_LOG_TYPE, event);
    }

    void Logger::addAppender(LogAppender::ptr appender)
    {
        Mutex::Lock lock(m_mutex);
        m_appenders.push_back(appender);
    }

    void Logger::delAppender(LogAppender::ptr appender)
    {
        Mutex::Lock lock(m_mutex);
        for (auto it = m_appenders.begin(); it != m_appenders.end(); it++)
            if (*it == appender)
            {
                m_appenders.erase(it);
                break;
            }
    }

    void ConsoleAppender::printLog(LogEvent::ptr event)
    {
        Mutex::Lock lock(m_mutex);
        OUTIO << m_formatter->format(event);
    }

    void FileAppender::printLog(LogEvent::ptr event)
    {
        reopen();
        Mutex::Lock lock(m_mutex);
        if (!m_formatter->format(m_filestream, event))
            std::cout << "error" << std::endl;
    }

    bool FileAppender::reopen()
    {
        Mutex::Lock lock(m_mutex);
        if (m_filestream)
            m_filestream.close();
        return Helpc::OpenFileWirte(m_filestream, m_filepath, std::ios_base::app);
    }

    std::string LogFormatter::format(LogEvent::ptr event)
    {
        std::stringstream ss;
        for (auto &it : m_formatItems)
            it->format(ss, event);
        return ss.str();
    }

    std::ostream &LogFormatter::format(std::ostream &ofs, LogEvent::ptr event)
    {
        for (auto &it : m_formatItems)
            it->format(ofs, event);
        return ofs;
    }

    class MessageFormatItem : public LogFormatter::FormatItem
    {
    public:
        MessageFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, LogEvent::ptr event) override
        {
            os << event->getContent();
        }
    };

    class LevelFormatItem : public LogFormatter::FormatItem
    {
    public:
        LevelFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, LogEvent::ptr event) override
        {
            os << LogLevel::ToString(event->getLevel());
        }
    };

    class ElapseFormatItem : public LogFormatter::FormatItem
    {
    public:
        ElapseFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, LogEvent::ptr event) override
        {
            os << event->getElapse();
        }
    };

    class NameFormatItem : public LogFormatter::FormatItem
    {
    public:
        NameFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, LogEvent::ptr event) override
        {
            os << event->getLogger()->getName();
        }
    };

    class ThreadIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        ThreadIdFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, LogEvent::ptr event) override
        {
            os << event->getThreadId();
        }
    };

    class FiberIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        FiberIdFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, LogEvent::ptr event) override
        {
            os << event->getFiberId();
        }
    };

    class ThreadNameFormatItem : public LogFormatter::FormatItem
    {
    public:
        ThreadNameFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, LogEvent::ptr event) override
        {
            os << event->getThreadName();
        }
    };

    class DateTimeFormatItem : public LogFormatter::FormatItem
    {
    public:
        DateTimeFormatItem(const std::string &format = "%Y-%m-%d %H:%M:%S")
            : m_format(format)
        {
            if (m_format.empty())
            {
                m_format = "%Y-%m-%d %H:%M:%S";
            }
        }

        void format(std::ostream &os, LogEvent::ptr event) override
        {
            struct tm tm;
            time_t time = event->getTime();
            localtime_r(&time, &tm);
            char buf[64];
            strftime(buf, sizeof(buf), m_format.c_str(), &tm);
            os << buf;
        }

    private:
        std::string m_format;
    };

    class FilenameFormatItem : public LogFormatter::FormatItem
    {
    public:
        FilenameFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, LogEvent::ptr event) override
        {
            os << event->getFileName();
        }
    };

    class LineFormatItem : public LogFormatter::FormatItem
    {
    public:
        LineFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, LogEvent::ptr event) override
        {
            os << event->getLine();
        }
    };

    class NewLineFormatItem : public LogFormatter::FormatItem
    {
    public:
        NewLineFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, LogEvent::ptr event) override
        {
            os << std::endl;
        }
    };

    class StringFormatItem : public LogFormatter::FormatItem
    {
    public:
        StringFormatItem(const std::string &str)
            : m_string(str) {}
        void format(std::ostream &os, LogEvent::ptr event) override
        {
            os << m_string;
        }

    private:
        std::string m_string;
    };

    class TabFormatItem : public LogFormatter::FormatItem
    {
    public:
        TabFormatItem(const std::string &str = "") {}
        void format(std::ostream &os, LogEvent::ptr event) override
        {
            os << "\t";
        }

    private:
        std::string m_string;
    };

    LogFormatter::LogFormatter(const std::string &pattern)
        : m_pattern(pattern)
    {
        parse();
    }

    void LogFormatter::parse()
    {
        std::vector<std::tuple<std::string, std::string, int>> vec;
        std::string nstr;
        for (size_t i = 0; i < m_pattern.size(); ++i)
        {
            if (m_pattern[i] != '%')
            {
                nstr.append(1, m_pattern[i]);
                continue;
            }

            if ((i + 1) < m_pattern.size())
            {
                if (m_pattern[i + 1] == '%')
                {
                    nstr.append(1, '%');
                    continue;
                }
            }

            size_t n = i + 1;
            int fmt_status = 0;
            size_t fmt_begin = 0;

            std::string str;
            std::string fmt;
            while (n < m_pattern.size())
            {
                if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}'))
                {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    break;
                }
                if (fmt_status == 0)
                {
                    if (m_pattern[n] == '{')
                    {
                        str = m_pattern.substr(i + 1, n - i - 1);
                        // std::cout << "*" << str << std::endl;
                        fmt_status = 1; //解析格式
                        fmt_begin = n;
                        ++n;
                        continue;
                    }
                }
                else if (fmt_status == 1)
                {
                    if (m_pattern[n] == '}')
                    {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        // std::cout << "#" << fmt << std::endl;
                        fmt_status = 0;
                        ++n;
                        break;
                    }
                }
                ++n;
                if (n == m_pattern.size())
                {
                    if (str.empty())
                    {
                        str = m_pattern.substr(i + 1);
                    }
                }
            }

            if (fmt_status == 0)
            {
                if (!nstr.empty())
                {
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n - 1;
            }
            else if (fmt_status == 1)
            {
                std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
                m_error = true;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
        }

        if (!nstr.empty())
        {
            vec.push_back(std::make_tuple(nstr, "", 0));
        }

        static std::map<std::string, std::function<FormatItem::formatItem_share_ptr(const std::string &)>> s_format_items = {
#define DEF_XX(opt, C)                                           \
    {                                                            \
#opt, [](const std::string &fmt)                         \
        { return FormatItem::formatItem_share_ptr(new C(fmt)); } \
    }

            DEF_XX(m, MessageFormatItem),    // m:消息
            DEF_XX(p, LevelFormatItem),      // p:日志级别
            DEF_XX(r, ElapseFormatItem),     // r:累计毫秒数
            DEF_XX(c, NameFormatItem),       // c:日志名称
            DEF_XX(t, ThreadIdFormatItem),   // t:线程id
            DEF_XX(n, NewLineFormatItem),    // n:换行
            DEF_XX(d, DateTimeFormatItem),   // d:时间
            DEF_XX(f, FilenameFormatItem),   // f:文件名
            DEF_XX(l, LineFormatItem),       // l:行号
            DEF_XX(T, TabFormatItem),        // T:Tab
            DEF_XX(F, FiberIdFormatItem),    // F:协程id
            DEF_XX(N, ThreadNameFormatItem), // N:线程名称
#undef DEF_XX
        };
        for (auto &i : vec)
        {
            if (std::get<2>(i) == 0)
            {
                m_formatItems.push_back(FormatItem::formatItem_share_ptr(new StringFormatItem(std::get<0>(i))));
            }
            else
            {
                auto it = s_format_items.find(std::get<0>(i));
                if (it == s_format_items.end())
                {
                    m_formatItems.push_back(FormatItem::formatItem_share_ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                    m_error = true;
                }
                else
                {
                    m_formatItems.push_back(it->second(std::get<1>(i)));
                }
            }
        }
    }

    Logger::Logger(const std::string &name, LogLevel::Level level)
        : m_name(name),
          m_level(level) {}
}
