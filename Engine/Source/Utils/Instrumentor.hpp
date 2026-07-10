#pragma once

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <iostream>

#include <thread>

namespace aio
{
    struct ProfileResult
    {
        std::string Name;
        long long Start, End;
        uint32_t ThreadID;
    };

    struct InstrumentationSession
    {
        std::string Name;
    };

    static std::string CleanFunctionSignature(const std::string& sig)
    {
        std::string cleaned = sig;
        const char* toRemove[] = { "__cdecl ", "__stdcall ", "__thiscall ", "__vectorcall " };
        for (auto r : toRemove) {
            size_t pos;
            while ((pos = cleaned.find(r)) != std::string::npos)
                cleaned.erase(pos, strlen(r));
        }
        return cleaned;
    }

    class Instrumentor
    {
    private:
        InstrumentationSession* m_CurrentSession;
        std::ofstream m_OutputStream;
        int m_ProfileCount;
    public:
        Instrumentor()
            : m_CurrentSession(nullptr), m_ProfileCount(0)
        {
        }

        void BeginSession(const std::string& name, const std::string& filepath = "results.json")
        {
            m_OutputStream.open(filepath);
            WriteHeader();
            m_CurrentSession = new InstrumentationSession{ name };
        }

        void EndSession()
        {
            WriteFooter();
            m_OutputStream.close();
            delete m_CurrentSession;
            m_CurrentSession = nullptr;
            m_ProfileCount = 0;
        }

        void WriteProfile(const ProfileResult& result)
        {
            if (m_ProfileCount++ > 0)
                m_OutputStream << ",";

            std::string name = result.Name;
            std::replace(name.begin(), name.end(), '"', '\'');

            m_OutputStream << "{";
            m_OutputStream << "\"cat\":\"function\",";
            m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
            m_OutputStream << "\"name\":\"" << name << "\",";
            m_OutputStream << "\"ph\":\"X\",";
            m_OutputStream << "\"pid\":0,";
            m_OutputStream << "\"tid\":" << result.ThreadID << ",";
            m_OutputStream << "\"ts\":" << result.Start;
            m_OutputStream << "}";

            m_OutputStream.flush();
        }

        void WriteHeader()
        {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
            m_OutputStream.flush();
        }

        void WriteFooter()
        {
            m_OutputStream << "]}";
            m_OutputStream.flush();
        }

        static Instrumentor& Get()
        {
            static Instrumentor instance;
            return instance;
        }
    };

    class InstrumentationTimer
    {
    public:
        InstrumentationTimer(const std::string& name)
            : m_Name(name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::high_resolution_clock::now();
        }

        ~InstrumentationTimer()
        {
            if (!m_Stopped)
                Stop();
        }

        void Stop()
        {
            auto endTimepoint = std::chrono::high_resolution_clock::now();

            long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
            long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

            uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
            Instrumentor::Get().WriteProfile({ m_Name, start, end, threadID });

            m_Stopped = true;
        }
    private:
        std::string m_Name;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
        bool m_Stopped;
    };
}

#if defined(_MSC_VER)
#define FUNCTION_SIG __FUNCSIG__
#else
#define FUNCTION_SIG __PRETTY_FUNCTION__
#endif

#ifdef AIO_DEBUG
#define PROFILING 1
#endif 

#if PROFILING == 1
#define CLEAN_FUNC_SIG()                                            ::aio::CleanFunctionSignature(FUNCTION_SIG)
#define AIO_BEGIN_SESSION(name, filepath)                           ::aio::Instrumentor::Get().BeginSession(name, filepath)
#define AIO_END_SESSION()                                           ::aio::Instrumentor::Get().EndSession()
#define AIO_PROFILE_SCOPE(name)                                     ::aio::InstrumentationTimer timer##__LINE__(name)
#define AIO_PROFILE_FUNCTION() AIO_PROFILE_SCOPE(CLEAN_FUNC_SIG())
#else
#define CLEAN_FUNC_SIG()
#define AIO_BEGIN_SESSION(name, filepath)
#define AIO_END_SESSION()
#define AIO_PROFILE_SCOPE(name)
#define AIO_PROFILE_FUNCTION()
#endif

