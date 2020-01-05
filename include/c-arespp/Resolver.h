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
#include <string>
#include <vector>

#include <mutex>
#include <queue>
#include <utility>

namespace CARESPP
{
    // Resolver is a wrapper for C-ARES, which handles static initialization and deinitialization.
    // All queries are processed asynchronously, and call a callback when finished, on the same thread
    // that runs Resolver::Run(). WSA MUST BE INITIALIZED OUTSIDE BEFORE USAGE
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

        // Asynchronously resolves a hostname with callback which will be called after the resolution
        void AsyncResolve(std::string hostName, ResolveCallback_t resolveCallback) noexcept;

        // Runs the resolver until all queries have been resolved. Calls all
        // callbacks when the transfer either completes or fails
        void Run() noexcept;
		// Runs all queued asynchronous operations, adding operations from a mutex-protected 
		// queue if they appear before the transfor or mid-transfer. Returns when all are complete 
		// and the queue is empty
		// MidResolveInsertionQueue_t example: std::queue<std::pair<std::string, ResolveCallback_t>>
		// A queue of void returning pairs of std::string hostName/ResolveCallback_t resolveCallback, 
		// with front() and pop()
		// Mutex_t example: std::mutex
		// A mutex with lock() and unlock()
		template<typename MidResolveInsertionQueue_t, typename Mutex_t>
		void Run(MidResolveInsertionQueue_t& midResolveInsertionQueue, Mutex_t& mutex)
		{
			// this implementation is copied and modified from ahost
			int nfds;
			fd_set read_fds, write_fds;
			timeval tv, * tvp;
			while (true)
			{
				// empty out the insertion queue
				{
					std::lock_guard insGuard(mutex);
					while (midResolveInsertionQueue.empty() == false)
					{
						const typename MidResolveInsertionQueue_t::value_type& front = midResolveInsertionQueue.front();
						AsyncResolve(std::move(front.first),
							std::move(front.second));
						midResolveInsertionQueue.pop();
					}
				}

				int res;
				FD_ZERO(&read_fds);
				FD_ZERO(&write_fds);
				nfds = ares_fds(m_ares, &read_fds, &write_fds);
				if (nfds == 0)
					break;
				tvp = ares_timeout(m_ares, NULL, &tv);
				res = select(nfds, &read_fds, &write_fds, NULL, tvp);
				if (res == -1)
					break;
				ares_process(m_ares, &read_fds, &write_fds);
			}
		}
    private:
        static void HandleResolve(void* arg, int status, int timeouts, hostent* hostent) noexcept;

        ares_channel m_ares;

        static std::atomic_size_t s_refCount;
    };
}

#endif