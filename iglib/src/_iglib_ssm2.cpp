#include "pch.h"
#include "_iglib_ssm2.h"
typedef const char *const c_cstr;

constexpr uint64_t POW10TABLE[ 20 ]
{
  1,
  10,
  100,
  1000,
  10000,
  100000,
  1000000,
  10000000,
  100000000,
  1000000000,
  10000000000,
  100000000000,
  1000000000000,
  10000000000000,
  100000000000000,
  1000000000000000,
  10000000000000000,
  100000000000000000,
  1000000000000000000,
  10000000000000000000,
};

constexpr FORCEINLINE bool is_capitalized(const char c)
{
  return 'A' <= c && c <= 'Z';
}

constexpr FORCEINLINE bool is_digit(const char c)
{
  return '0' <= c && c <= '9';
}

constexpr FORCEINLINE bool is_letter(const char c)
{
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

constexpr FORCEINLINE bool is_whitespace(const char c)
{
  return c == '\n' || c == ' ' || c == '\t';
}

template <typename _T>
constexpr FORCEINLINE _T parse_integral(c_cstr s, const size_t n)
{
  _T i;
  for (size_t j{}; j < n; j++)
    i += _T(s[ j ] - '0') * POW10TABLE[ j ];
  return i;
}

class StringReader final
{
public:
  StringReader(c_cstr p_cstr, const size_t l)
    : m_length{ l }, m_cstr{ p_cstr }, m_index{} {}

  __forceinline _NODISCARD char read() noexcept
  {
    return m_cstr[ m_index++ ];
  }

  __forceinline _NODISCARD std::string &&read(size_t n)
  {
    if (!n)
      return std::string();

    if (m_index + n >= m_length)
      n = (m_length - m_index) - 1;

    std::string s{ m_cstr + m_index, n };
    m_index += n;
    return std::move(s);
  }

  __forceinline _NODISCARD std::string &&read(const size_t i, size_t n)
  {
    if (!n)
      return std::string();

    if (i >= m_length)
      return std::string();

    if (i + n >= m_length)
      n = (m_length - i) - 1;
    return std::string{ m_cstr + i, n };
  }

  __forceinline _NODISCARD bool empty() const noexcept
  {
    return m_index >= m_length;
  }

  __forceinline operator bool() const noexcept
  {
    return m_index < m_length;
  }

  __forceinline void advance() noexcept
  {
    m_index++;
  }

  __forceinline void retreat() noexcept
  {
    m_index--;
  }

  __forceinline _NODISCARD bool march() noexcept
  {
    return ++m_index < m_length;
  }

  __forceinline _NODISCARD size_t get_index() const noexcept
  {
    return m_index;
  }

  __forceinline _NODISCARD c_cstr begin() const noexcept
  {
    return m_cstr + m_index;
  }

  __forceinline _NODISCARD c_cstr end() const noexcept
  {
    return m_cstr + m_length;
  }

  __forceinline _NODISCARD c_cstr data() const noexcept
  {
    return m_cstr;
  }

private:
  size_t m_index;
  const size_t m_length;
  c_cstr m_cstr;
  std::stack<size_t, std::vector<size_t>> m_indcies_stack;
};

FORCEINLINE std::vector<std::string> tokenize(const std::string &src)
{
  c_cstr cstr = src.c_str();
  const size_t len = src.length();
  std::vector<std::string> strs(1024);

  for (StringReader reader{ cstr, len }; reader; reader.advance())
  {
    const char c = reader.read();
    const size_t anchor = reader.get_index();

    if (is_letter(c))
    {
      while (reader.march())
      {
        const char c2 = reader.read();
        if (is_whitespace(c2))
          break;
      }
      const size_t l = reader.get_index() - anchor;
      strs.push_back(reader.read(anchor, l));
      reader.retreat();
    }
    else if (is_digit(c) || c == '.')
    {
      bool dotet = (c == '.');
      while (reader.march())
      {
        const char c2 = reader.read();
        if (!is_digit(c2))
          break;
      }
      const size_t l = reader.get_index() - anchor;
      strs.push_back(reader.read(anchor, l));
      reader.retreat();
    }

  }
}

namespace ig::ssm
{
	ScalerSceneModel2D::ScalerSceneModel2D(const std::string &src)
	{
	}

	ScalerSceneModel2D::ScalerSceneModel2D(std::ifstream &file)
	{
	}
}
