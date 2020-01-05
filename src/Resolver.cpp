#include <c-arespp/Resolver.h>

// STL
#include <stdexcept>

// C-ARES
#include <ares_setup.h>

#ifndef _WIN32
#include <netdb.h>
#endif

using CARESPP::Resolver;

std::atomic_size_t Resolver::s_refCount = 0;

Resolver::Resolver()
{
	if (s_refCount++ == 0)
	{
		int status = ares_library_init(ARES_LIB_INIT_ALL);
		if (status != ARES_SUCCESS)
			throw std::runtime_error("Library failed to init: " + std::string(ares_strerror(status)));
	}

	int status = ares_init(&m_ares);
	if (status != ARES_SUCCESS)
		throw std::runtime_error("ARES failed to init: " + std::string(ares_strerror(status)));
}

Resolver::Resolver(Resolver&& other) noexcept
	: m_ares(std::exchange(other.m_ares, nullptr))
{
	++s_refCount;
}

Resolver& Resolver::operator=(Resolver&& other) noexcept
{
	if (m_ares != nullptr)
		ares_destroy(m_ares);

	m_ares = other.m_ares;
	other.m_ares = nullptr;

	return *this;
}

Resolver::~Resolver() noexcept
{
	if (m_ares != nullptr)
		ares_destroy(m_ares);

	if (--s_refCount == 0)
	{
		// static deinit
		ares_library_cleanup();
	}
}

void Resolver::AsyncResolve(std::string hostName, ResolveCallback_t resolveCallback) noexcept
{
	struct in_addr addr4;
	
	std::pair<std::string, ResolveCallback_t>* pPair = 
		new std::pair(std::make_pair(std::move(hostName), std::move(resolveCallback)));

	// try to convert it in case it is an IP
	if (ares_inet_pton(AF_INET, pPair->first.c_str(), &addr4) == 1)
	{
		ares_gethostbyaddr(m_ares, &addr4, sizeof(addr4), AF_INET, &Resolver::HandleResolve, pPair);
	}
	else
	{
		ares_gethostbyname(m_ares, pPair->first.c_str(), AF_INET, &Resolver::HandleResolve, pPair);
	}
}

// this implementation is copied and modified from ahost
void Resolver::Run() noexcept
{
	int nfds;
	fd_set read_fds, write_fds;
	timeval tv, *tvp;
	while (true)
	{
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

void Resolver::HandleResolve(void* arg, int status, int timeouts, hostent* hostent) noexcept
{
	std::pair<std::string, ResolveCallback_t>* pPair = 
		reinterpret_cast<std::pair<std::string, ResolveCallback_t>*>(arg);

	std::vector<std::string> IPs;

	if (status == ARES_SUCCESS)
	{
		// add all of the IPs that were found
		for (char** ppAddr = hostent->h_addr_list;
			*ppAddr != nullptr;
			++ppAddr)
		{
			char addrBuf[46] = "??";
			if (ares_inet_ntop(hostent->h_addrtype, *ppAddr, addrBuf, sizeof(addrBuf)) == nullptr)
				continue;

			IPs.emplace_back(addrBuf);
		}
	}

	pPair->second(status, IPs);
	delete pPair;
}