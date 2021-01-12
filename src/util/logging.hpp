#pragma once

#include <map>
#include <sstream>
#include <string>
#include <string_view>

namespace MjB {

/**************************************************************************/
/**
 * @name LoggerOutputHandlerBase
 * @brief Abstract base class for all logging output handlers
 *
 * Responsible for taking an 'entry' (line of text) and displaying/storing/etc
 *
 */
class LoggerOutputHandlerBase {
  public:
    /**************************************************************************/
    /**
     * @brief Process a new entry for output
     *
     * @param entry String view to new entry for logging
     *
     */
    virtual void processEntry(std::string_view const& entry) = 0;

    virtual ~LoggerOutputHandlerBase() {}
};

/**************************************************************************/
/**
 * @name STDOUTLogger
 * @brief Logger output that immediately directs all entries to STDOUT
 *
 * Note: appends a newline to each entry
 *
 */
class STDOUTLogger : public LoggerOutputHandlerBase {

  public:
    /**************************************************************************/
    /**
     * @brief Process a new entry for output
     *
     * @param entry String view to new entry for logging
     *
     */
    void processEntry(std::string_view const& entry) {
        std::cout << entry << "\n";
    }
};

class Logger {

    std::map<std::string const, std::unique_ptr<LoggerOutputHandlerBase>>
        handlers_;

    /**************************************************************************/
    /**
     * @brief Pass a new entry to all the registered handlers
     *
     * @param entry New entry to process
     *
     */
    void processEntry(std::string_view const& entry) {
        for (auto const& [name, handler] : handlers_) {
            handler->processEntry(entry);
        }
    }

  public:
    // Instace designed to be ephemeral and collect a new log entry during its
    // lifespan
    class LogInstance {
        std::stringstream
            outputStream_; /**< Store the entry as it's being created */
        Logger& owner_;    /**< The logger that created this  */

        friend class Logger;

        // private so only the logger can create one
        LogInstance(Logger& owner) : owner_(owner) {
            outputStream_ << "[log] ";
        }

      public:
        LogInstance(LogInstance const&) = delete;
        /**************************************************************************/
        /**
         * @brief Move constructor
         *
         */
        LogInstance(LogInstance&& other) :
            outputStream_(std::move(other.outputStream_)),
            owner_(other.owner_) {}

        /**************************************************************************/
        /**
         * @brief Call operator -- adds a new item to the log (assumed to be
         * streamable)
         *
         * @param item Item to add to current entry
         *
         */
        template <typename T>
        void operator()(T const& item) {
            outputStream_ << item;
        }

        /**************************************************************************/
        /**
         * @brief Destructor -- will submit the entry to the owning logger
         *
         */
        ~LogInstance() {
            std::cout << "deconstructing logger (string is: "
                      << outputStream_.str() << ")\n";
            owner_.processEntry(outputStream_.str());
        }
    };

    using OutputHandlerRef = std::unique_ptr<LoggerOutputHandlerBase>;
    using NamedHandler = std::pair<std::string, OutputHandlerRef>;

    /**************************************************************************/
    /**
     * @brief Default constructor
     *
     */
    Logger() {}

    /**************************************************************************/
    /**
     * @brief List constructor
     *
     * @param handlerList Initializer list of name/handler pairs to add to the
     *                    logger
     *
     */
    // Logger(std::initializer_list<NamedHandler> handlerList) {
    //     for (auto& [name, handler] : handlerList) {
    //         handlers_[name] = std::move(handler);
    //     }
    // }

    /**************************************************************************/
    /**
     * @brief Adds a new named output handler - will overwrite any output
     * handler with the same name
     *
     * @param name String to identify this output handler
     * @param handler Unique pointer to new output handler
     *
     */
    void addHandler(std::string_view const& name,
                    std::unique_ptr<LoggerOutputHandlerBase>&& handler) {
        handlers_[std::string(name)] = std::move(handler);
    }

    /**************************************************************************/
    /**
     * @brief Creates a new logging instance whose lifetime corresponds with
     *        a single log entry
     *
     * @return A 'log instance' structure (designed to be temporary)
     *
     */
    LogInstance log() { return LogInstance(*this); }
};

template <typename T>
Logger::LogInstance&& operator<<(Logger::LogInstance&& logger, T const& item) {
    logger(item);
    return std::move(logger);
}

Logger::LogInstance&& operator<<(Logger::LogInstance&& logger,
                                 std::string_view const& item) {
    logger(item);
    return std::move(logger);
}

} // namespace MjB
