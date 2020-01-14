# c-arespp
Designed with a similar interface to curlpp-async, a C++ wrapper for c-ares domain name resolution

## CARESPP::Resolver
`Resolver` is a wrapper around `ares_channel`, that allows for asynchronous callback-based resolving of DNS names, in the proactor design.

**Windows implication**: `WSAStartup` must occur before the resolver is created, or else ares will not function.

### Functions
`Resolver::Resolver(void)`: Creates a Resolver and throws `std::runtime_error` in the event that ares fails to start. Also performs static initialization of the c-ares library if this is the first instance.

`void Resolver::AsyncResolve(std::string hostName, ResolveCallback_t resolveCallback)`: Starts an asynchronous resolve operation. This function is thread-safe and can be called from any thread.

`ResolveCallback_t` is defined as `std::function<void(int status, const std::vector<std::string>& IPs)>`

`void Resolver::Run()`: Runs the resolver until all queued operations have completed.
