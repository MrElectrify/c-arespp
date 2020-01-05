#include <c-arespp/Resolver.h>

#include <chrono>
#include <cstdio>
#include <sstream>

// C-ARES
#include <ares_setup.h>

using CARESPP::Resolver;

int main()
{
#ifdef USE_WINSOCK
	WORD wVersionRequested = MAKEWORD(USE_WINSOCK, USE_WINSOCK);
	WSADATA wsaData;
	WSAStartup(wVersionRequested, &wsaData);
#endif

	Resolver resolver;

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

	resolver.AsyncResolve("en.cppreference.com",
		[](int status, const std::vector<std::string>& IPs)
		{
			if (status != ARES_SUCCESS)
			{
				printf("Failed to resolve en.cppreference.com: %s\n", ares_strerror(status));
				return;
			}

			std::ostringstream ss;
			ss << "IPs for en.cppreference.com:\n";
			for (const std::string& IP : IPs)
				ss << IP << '\n';

			printf(ss.str().c_str());
		});

	resolver.AsyncResolve("www.google.com",
		[](int status, const std::vector<std::string>& IPs)
		{
			if (status != ARES_SUCCESS)
			{
				printf("Failed to resolve www.google.com: %s\n", ares_strerror(status));
				return;
			}

			std::ostringstream ss;
			ss << "IPs for www.google.com:\n";
			for (const std::string& IP : IPs)
				ss << IP << '\n';

			printf(ss.str().c_str());
		});

	resolver.AsyncResolve("www.github.com",
		[](int status, const std::vector<std::string>& IPs)
		{
			if (status != ARES_SUCCESS)
			{
				printf("Failed to resolve www.github.com: %s\n", ares_strerror(status));
				return;
			}

			std::ostringstream ss;
			ss << "IPs for www.github.com:\n";
			for (const std::string& IP : IPs)
				ss << IP << '\n';

			printf(ss.str().c_str());
		});

	resolver.AsyncResolve("www.timeanddate.com",
		[](int status, const std::vector<std::string>& IPs)
		{
			if (status != ARES_SUCCESS)
			{
				printf("Failed to resolve www.timeanddate.com: %s\n", ares_strerror(status));
				return;
			}

			std::ostringstream ss;
			ss << "IPs for www.timeanddate.com:\n";
			for (const std::string& IP : IPs)
				ss << IP << '\n';

			printf(ss.str().c_str());
		});

	resolver.Run();

	std::chrono::system_clock::time_point end = std::chrono::system_clock::now();

	printf("Resolution took %d ms\n", std::chrono::duration_cast<std::chrono::milliseconds>(end  - start));

#ifdef USE_WINSOCK
	WSACleanup();
#endif

	return 0;
}