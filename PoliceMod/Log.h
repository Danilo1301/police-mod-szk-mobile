#pragma once

#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

class LogFile {
public:
    LogFile(const std::string& filename) {
        this->filename = filename;

        stream.open(filename, std::ios::app);
        if (!stream.is_open()) {
            std::cerr << "Erro: não foi possível abrir " << filename << "\n";
        }
    }

    ~LogFile() {
        if (stream.is_open()) {
            stream.close();
        }
    }

    void clearLog() {
        std::ofstream ofs(filename, std::ios::trunc);
        // O std::ofstream fecha automaticamente quando sai do escopo
    }

    template <typename T>
    LogFile& operator<<(const T& value) {
        if (stream.is_open()) {
            // Se for começo de linha, adiciona timestamp
            if (atLineStart) {
                stream << "[" << currentTime() << "] ";
                atLineStart = false;
            }

            stream << value;

            // Se o valor for std::endl, marca que próxima escrita é no início de linha
            // (necessário para manipuladores)
        }
        return *this;
    }

    LogFile& operator<<(std::ostream& (*manip)(std::ostream&)) {
        if (stream.is_open()) {
            manip(stream);
            if (manip == static_cast<std::ostream& (*)(std::ostream&)>(std::endl)) {
                atLineStart = true;
            }
        }
        return *this;
    }

    size_t getSize() {
        stream.flush(); // garante que todos os dados foram gravados
        return std::filesystem::file_size(filename);
    }

    void truncate() {
        stream.close();
        std::ofstream ofs(filename, std::ios::trunc);
        ofs.close();
        stream.open(filename, std::ios::app);
    }

private:
    std::ofstream stream;
    bool atLineStart = true;
    std::string filename;

    std::string currentTime() {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm localTime;

        localtime_r(&t, &localTime);

        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << localTime.tm_hour << ":"
            << std::setw(2) << std::setfill('0') << localTime.tm_min << ":"
            << std::setw(2) << std::setfill('0') << localTime.tm_sec;
        return oss.str();
    }
};

class Log {
public:
    static std::unique_ptr<LogFile> file;
    static std::unique_ptr<LogFile> internal;

    static void Init(const std::string& basePath) {
        file  = std::make_unique<LogFile>(basePath + "/policeMod.log");
        internal = std::make_unique<LogFile>(basePath + "/internal.log");
    }

    static void Close() {
        file.reset();
        internal.reset();
    }
};

// Função base para concatenar os argumentos
template<typename T>
static void appendToStream(std::ostringstream& oss, const T& value) {
    oss << value;
}

template<typename T, typename... Args>
static void appendToStream(std::ostringstream& oss, const T& value, const Args&... args) {
    oss << value;
    appendToStream(oss, args...);
}

// Função helper para logar no arquivo principal
template<typename... Args>
static void logToFile(const Args&... args) {
    std::ostringstream oss;
    appendToStream(oss, args...);

    if (Log::file) { 
        *Log::file << oss.str() << std::endl;
    }

    if (Log::internal && Log::internal->getSize() > 5 * 1024 * 1024) {
        Log::internal->truncate();
    }

    if (Log::internal) { 
        *Log::internal << oss.str() << std::endl;
    }
}

// Função helper para logar no log interno
template<typename... Args>
static void logInternal(const Args&... args) {
    if (Log::internal) {
        std::ostringstream oss;
        appendToStream(oss, args...);
        *Log::internal << oss.str() << std::endl;
    }
}