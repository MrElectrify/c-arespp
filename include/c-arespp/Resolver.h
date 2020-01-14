#ifndef LIBCARESPP_RESOLVER_H_
#define LIBCARESPP_RESOLVER_H_

/*
 *  C-ARESPP Resolver
 *  1/4/20 22:49
 */

// ARES
#define CARES_STATICLIB
#include <ares.h>

// STL
#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <utility>
#include <vector>


namespace CARESPP
{
    // Resolver is a wrapper for C-ARES, which handles static initialization and deinitialization.
    // All queries are processed asynchronously, and call a callback when finished, on the same thread
    // that runs Resolver::Run(). WSA MUST BE INITIALIZED OUTSIDE BEFORE USAGE.
    class Resolver
    {
    public:
        using ResolveCallback_t = std::function<void(int status, const std::vector<std::string>& IPs)>;

        // Creates a resolver. Throws std::runtime_error on error
        Resolver();

        Resolver(Resolver&& other) noexcept;
        Resolver& operator=(Resolver&& other) noexcept;

        Resolver(const Resolver& other) = delete;
        Resolver& operator=(const Resolver& other) = delete;

        // Cancels any async operations and calls the handler with failure
        ~Resolver() noexcept;

        // Asynchronously resolves a hostname with callback which will be called after the resolution.
        // This may be called from other threads than the thread running the loop
        void AsyncResolve(std::string hostName, ResolveCallback_t resolveCallback) noexcept;

        // Runs the resolver until all queries have been resolved. Calls all
        // callbacks when the transfer either completes or fails. Resolver must not
        // be moved, destructed, or go out of scope while Run() is in progress
        void Run() noexcept;
    private:
        static void HandleResolve(void* arg, int status, int timeouts, hostent* hostent) noexcept;

        ares_channel m_ares;
        std::mutex m_aresMutex;

        static std::atomic_size_t s_refCount;
    };
}

#endif