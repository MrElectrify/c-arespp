#ifndef LIBCURLPP_ASYNC_WEBCLIENT_H_
#define LIBCURLPP_ASYNC_WEBCLIENT_H_

/*
 *  CURLPP-Async WebClient
 *  12/29/19 20:09
 */

// ARES
#define CARES_STATICLIB
#include <ares.h>

// STL
#include <atomic>
#include <functional>
#include <string>
#include <vector>

namespace CARESPP
{
    // Resolver is a wrapper for C-ARES, which handles static initialization and deinitialization.
    // All queries are processed asynchronously, and call a callback when finished, on the same thread
    // that runs Resolver::Run().
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

        // Cancels any async operations and calls the handler with
        ~Resolver() noexcept;

        // Starts an asynchronous resolve operation

        // Runs the resolver until all queries have been resolved. Calls all
        // callbacks when the transfer either completes or fails
        void Run();
    private:
        ares_channel m_ares;

        static std::atomic_size_t s_refCount;
    };
}

#endif