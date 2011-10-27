#ifndef _LOG_H_
#define _LOG_H_

#include <string>

enum LogLevel {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
};

class Logger {
public:
    Logger();
    Logger(LogLevel level);
    virtual ~Logger();
public:
    bool log(const std::string& msg, LogLevel level) {
        if(level >= this->level) {
            logImpl(msg);
        }
    }

    void setLevel(LogLevel level) {
        this->level = level;
    }
    
    LogLevel getLevel() const {
        return level;
    }
    
protected:
    virtual bool logImpl(const std::string msg) = 0;
private:
    LogLevel level;
};


class FileLogger : public Logger {
public:
    FileLogger();
    FileLogger(const std::string& filename);
    virtual ~FileLogger();
private:
    FileLogger(const FileLogger& rhs);
    const FileLogger& operator=(const FileLogger& rhs);
public:
    bool open();
    void close();
    virtual bool logImpl(const std::string& msg);
};


class RollingFileLogger : public Logger {
public:
    RollingFileLogger(const std::string& dir, const std::string& basename);
    virtual ~RollingFileLogger();
private:
    RollingFileLogger(const RollingFileLogger& rhs);
    const RollingFileLogger& operator=(const RollingFileLogger& rhs);
public:
    bool open();
    void close();

    virtual bool logImpl(const std::string& msg);
private:
    std::string dir;
    std::string basename;
    FileLogger* fileLogger;
};

#endif /* _LOG_H_ */
