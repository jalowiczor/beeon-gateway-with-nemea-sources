#include <string>
#include <sstream>

template <typename T>
static inline std::string impl_to_string(const T &n)
{
	std::ostringstream s;
	s << n;
	return s.str();
}

std::string std::to_string(int v)
{
	return impl_to_string(v);
}

std::string std::to_string(long v)
{
	return impl_to_string(v);
}

std::string std::to_string(long long v)
{
	return impl_to_string(v);
}

std::string std::to_string(unsigned int v)
{
	return impl_to_string(v);
}

std::string std::to_string(unsigned long v)
{
	return impl_to_string(v);
}

std::string std::to_string(unsigned long long v)
{
	return impl_to_string(v);
}

std::string std::to_string(float v)
{
	return impl_to_string(v);
}

std::string std::to_string(double v)
{
	return impl_to_string(v);
}

std::string std::to_string(long double v)
{
	return impl_to_string(v);
}
