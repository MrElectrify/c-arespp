#include <c-arespp/Resolver.h>

// C-ARES
#include <ares_setup.h>

// STL
#include <stdexcept>

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

Resolver::~Resolver()
{
	if (m_ares != nullptr)
		ares_destroy(m_ares);

	if (--s_refCount == 0)
	{
		// static deinit
		ares_library_cleanup();
	}
}