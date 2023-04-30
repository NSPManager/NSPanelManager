#ifndef MqttLog_H
#define MqttLog_H

#include <Arduino.h>
#include <string>
#include <PubSubClient.h>
#include <MqttManager.hpp>

#define LOG_SHORT_FILENAME                                   \
    (strrchr(__FILE__, '/')    ? strrchr(__FILE__, '/') + 1  \
     : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 \
                               : __FILE__)

#define LOG_ERROR(...)                                                    \
    MqttLog::instance->logToMqtt(MqttLogLevel::Error, LOG_SHORT_FILENAME, \
                                 __LINE__, __func__, __VA_ARGS__)
#define LOG_WARNING(...)                                                    \
    MqttLog::instance->logToMqtt(MqttLogLevel::Warning, LOG_SHORT_FILENAME, \
                                 __LINE__, __func__, __VA_ARGS__)
#define LOG_INFO(...)                                                    \
    MqttLog::instance->logToMqtt(MqttLogLevel::Info, LOG_SHORT_FILENAME, \
                                 __LINE__, __func__, __VA_ARGS__)
#define LOG_DEBUG(...)                                                    \
    MqttLog::instance->logToMqtt(MqttLogLevel::Debug, LOG_SHORT_FILENAME, \
                                 __LINE__, __func__, __VA_ARGS__)
#define LOG_TRACE(...)                                                    \
    MqttLog::instance->logToMqtt(MqttLogLevel::Trace, LOG_SHORT_FILENAME, \
                                 __LINE__, __func__, __VA_ARGS__)

enum MqttLogLevel
{
    None,
    Error,
    Warning,
    Info,
    Debug,
    Trace
};

class MqttLog
{
public:
    inline static MqttLog *instance;
    /// @brief Will initialize the library
    void init(std::string *mqttLogTopic);
    /// @brief Set the highest log level to log
    /// @param logLevel The highest log level to send to mqtt server
    void setLogLevel(const MqttLogLevel logLevel);

    template <typename... Args>
    void logToMqtt(const MqttLogLevel logLevel, const char *filename, int lineNumber, const char *functionName, Args &&...args)
    {
        if (logLevel > this->_logLevel || this->_mqttLogTopic->empty())
        {
            return;
        }

        xSemaphoreTake(this->_messageBuilderMutex, portMAX_DELAY);
        this->buildMessageFromArgs(args...); // Build all parts into one message string

        std::string mqttLogMessage;
        switch (logLevel)
        {
        case MqttLogLevel::Error:
            mqttLogMessage = "ERROR";
            break;
        case MqttLogLevel::Warning:
            mqttLogMessage = "WARNING";
            break;
        case MqttLogLevel::Info:
            mqttLogMessage = "INFO";
            break;
        case MqttLogLevel::Debug:
            mqttLogMessage = "DEBUG";
            break;
        case MqttLogLevel::Trace:
            mqttLogMessage = "TRACE";
            break;

        default:
            mqttLogMessage = "-UKN-";
            break;
        }

        // Build log message
        mqttLogMessage.append(": ");
        mqttLogMessage.append(filename);
        mqttLogMessage.append("");
        mqttLogMessage.append(":");
        mqttLogMessage.append(std::to_string(lineNumber));
        mqttLogMessage.append(" f(.");
        mqttLogMessage.append(functionName);
        mqttLogMessage.append(") ");
        mqttLogMessage.append(this->_messageBuild.c_str());

        // Reset message holder before giving up the mutex
        this->_messageBuild.clear();
        xSemaphoreGive(this->_messageBuilderMutex);
        // Queue log message for sending
        MqttManager::publish(this->_mqttLogTopic->c_str(), mqttLogMessage);
    }

    template <typename LogMessageT, typename... Args>
    void buildMessageFromArgs(LogMessageT logMessage, Args... args)
    {
        this->_messageBuild.concat(logMessage);

        if (sizeof...(args) > 0)
        {
            this->buildMessageFromArgs(args...);
        }
    }

    template <typename LogMessageT>
    void buildMessageFromArgs(LogMessageT logMessage)
    {
        this->_messageBuild.concat(logMessage);
    }

private:
    std::string *_mqttLogTopic;
    MqttLogLevel _logLevel{MqttLogLevel::Info};
    String _messageBuild;
    SemaphoreHandle_t _messageBuilderMutex = NULL; // Lock usage of message building argument to one call at the time.
};

#endif
