/*
 * ==============================================================================
 *
 *       Filename:  main.cc
 *        Created:  04/26/15 15:15:04
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * ==============================================================================
 */

#include <bits/stdc++.h>
#include <boost/lexical_cast.hpp>

const char* strnchr(const char* s, char c, size_t n) {
  return reinterpret_cast<const char*>(memchr(s, c, n));
}

namespace alpha {
class Slice {
 public:
  Slice()
    : buf_(nullptr), size_(0) {
  }
  Slice(const std::string& s)
    : buf_(s.data()), size_(s.size()) {
  }
  Slice(const char* s)
    : buf_(s), size_(strlen(s)) {
  }
  Slice(const char* s, const char* e)
    : buf_(s), size_(e - s) {
    assert (e >= s);
  }
  Slice(const char* s, size_t len)
    : buf_(s), size_(len) {
  }
  const char* data() const {
    return buf_;
  }
  size_t size() const {
    return size_;
  }
  std::string str() const {
    return std::string(buf_, size_);
  }
  bool empty() const {
    return buf_ == nullptr || size_ == 0;
  }
  const char* begin() const {
    return buf_;
  }
  const char* end() const {
    return buf_ + size_;
  }
  void clear() {
    buf_ = nullptr;
    size_ = 0;
  }
  void assign(const char* s, const char* e) {
    *this = alpha::Slice(s, e);
  }
  Slice subslice(size_t pos, size_t count) {
    assert (pos <= size_);
    size_t newsize = std::min(size_ - pos, count);
    return Slice(buf_ + pos, newsize);
  }
  void advance(size_t step) {
    assert (step <= size_);
    buf_ += step;
    size_ -= step;
  }
 private:
  const char* buf_;
  size_t size_;
};

bool operator<(const Slice& lhs, const Slice& rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                      rhs.begin(), rhs.end());
}

bool operator==(const Slice& lhs, const Slice& rhs) {
  return lhs.size() == rhs.size()
         && memcmp(lhs.data(), rhs.data(), lhs.size()) == 0;
}
}

class BadFormatException final : public std::invalid_argument {
 public:
  explicit BadFormatException(const std::string& msg)
    : std::invalid_argument(msg) {
  }
};

struct FormatArg {
  enum class NextKeyMode {
    kNone = 1,
    kInt = 2,
    kString = 3
  };
  enum class Align : uint8_t {
    kDefault,
    kLeft,
    kRight,
    kPadAfterSign,
    kCenter,
    kInvalid
  };
  enum class Sign : uint8_t {
    kDefault,
    kPlusOrMinus,
    kMinus,
    kSpaceOrMinus,
    kInvalid
  };
  static constexpr char kDefaultFill = '\0';
  static constexpr int kDefaultWidth = -1;
  static constexpr int kDefaultPrecision = -1;
  static constexpr char kDefaultPresentation = '\0';

  FormatArg(alpha::Slice s)
    : next_key_mode(NextKeyMode::kNone),
      fill(kDefaultFill),
      align(Align::kDefault),
      sign(Sign::kDefault) {
    if (!s.empty()) {
      InitSlow(s);
    }
  }

  void InitSlow(alpha::Slice str) {
    auto p = strnchr(str.begin(), ':', str.size());
    if (!p) {
      key = str;
      return;
    } else {
      key = alpha::Slice(str.begin(), p);
    }
    ++p;
    auto e = str.end();
    if (p == e) {
      return;
    }
    Align a;
    if (p + 1 != e && (a = FormatAlign(*(p + 1))) != Align::kInvalid) {
      fill = *p;
      align = a;
      p += 2;
    } else if ((a = FormatAlign(*p)) != Align::kInvalid) {
      align = a;
      ++p;
    }
    if (p == e) return;
    Sign s;
    if ((s = FormatSign(*p)) != Sign::kInvalid) {
      sign = s;
      ++p;
      if (p == e) return;
    }
    if (*p == '#') {
      base_prefix = true;
      if (++p == e) return;
    }

    if (*p == '0') {
      if (align != Align::kDefault) {
        throw BadFormatException("alignment specified twice");
      }
      fill = '0';
      align = Align::kPadAfterSign;
      if (++p == e) return;
    }
    if (*p >= '0' && *p <= '9') {
      auto b = p;
      do {
        ++p;
      } while (p != e && *p >= '0' and * p <= '9');
      alpha::Slice w(b, p);
      width = boost::lexical_cast<int>(w.str());
      if (p == e) return;
    }

    if (*p == ',') {
      thousands_separator = true;
      if (++p == e) return;
    }

    if (*p == '.') {
      auto b = ++p;
      while (p != e && isdigit(*p)) {
        ++p;
      }
      if (p != b) {
        precision = boost::lexical_cast<int>(alpha::Slice(b, p).str());
        if (p != e && *p == '.') {
          trailing_dot = true;
          ++p;
        }
      } else {
        trailing_dot = true;
      }
      if (p == e) return;
    }

    presentation = *p;
    if (++p == e) return;
    throw BadFormatException("extra characters in format string");
  }

  Align FormatAlign(char c) {
    if (c == '<') {
      return Align::kLeft;
    } else if (c == '>') {
      return Align::kRight;
    } else if (c == '=') {
      return Align::kPadAfterSign;
    } else if (c == '^') {
      return Align::kCenter;
    } else {
      return Align::kInvalid;
    }
  }
  Sign FormatSign(char c) {
    if (c == '+') {
      return Sign::kPlusOrMinus;
    } else if (c == '-') {
      return Sign::kMinus;
    } else if (c == ' ') {
      return Sign::kSpaceOrMinus;
    } else {
      return Sign::kInvalid;
    }
  }

  alpha::Slice SplitKey() {
    if (next_key_mode == NextKeyMode::kString) {
      next_key_mode = NextKeyMode::kNone;
      return next_key;
    }
    if (key.empty()) {
      return alpha::Slice();
    }

    auto b = key.begin();
    auto e = key.end();
    const char* p;
    if (e[-1] == ']') {
      --e;
      p = strnchr(b, '[', key.size());
      if (p == nullptr) {
        throw BadFormatException("unmatched `]'");
      }
    } else {
      p = strnchr(b, '.', key.size());
    }

    if (p) {
      key.assign(p + 1, e);
    } else {
      p = e;
      key.clear();
    }
    return alpha::Slice(b, p);
  }

  unsigned SplitIntKey() {
    if (next_key_mode == NextKeyMode::kInt) {
      next_key_mode = NextKeyMode::kNone;
      return next_int_key;
    }
    auto piece = SplitKey();
    size_t e;
    unsigned key = stoul(piece.str(), &e, 10);
    if (e != piece.size()) {
      throw BadFormatException("Key must be integer type");
    }
    return key;
  }

  void SetNextKey(alpha::Slice s) {
    assert (next_key_mode != NextKeyMode::kInt);
    next_key = s;
    next_key_mode = NextKeyMode::kString;
  }

  void SetNextIntKey(unsigned key) {
    next_int_key = key;
    next_key_mode = NextKeyMode::kInt;
  }

  unsigned next_int_key;
  alpha::Slice key;
  alpha::Slice next_key;
  NextKeyMode next_key_mode;
  char fill;
  Align align;
  Sign sign;
  bool base_prefix;
  int width = kDefaultWidth;
  bool thousands_separator = false;
  int precision = kDefaultPrecision;
  bool trailing_dot = false;
  char presentation = kDefaultPresentation;
};

template<typename T, class Enable = void>
struct IntKeyIndexable {
  static const bool value = false;
};

struct FormatTraitsBase {
  using enable = void;
};

template<typename T, class Enable = void>
struct IndexableTraits;

template<typename C>
struct IndexableTraitsSeq : FormatTraitsBase {
  using container_type = C;
  using value_type = typename C::value_type;
  static const value_type& at(const C& c, unsigned index) {
    return c.at(index);
  }
};

template<typename T, typename A>
struct IndexableTraits<std::vector<T, A>>
                                           : IndexableTraitsSeq<std::vector<T, A>> {
};

template<typename T, typename A>
struct IndexableTraits<std::deque<T, A>>
                                          : IndexableTraitsSeq<std::deque<T, A>> {
};

template<typename T, size_t N>
struct IndexableTraits<std::array<T, N>>
                                          : IndexableTraitsSeq<std::array<T, N>> {

};

template<typename C>
struct IndexableTraitsAssoc : FormatTraitsBase {
  using container_type = C;
  using value_type = typename C::value_type::second_type;
  static const value_type& at(const C& c, unsigned idx) {
    return c.at(static_cast<typename C::key_type>(idx));
  }
};

template<typename K, typename T, typename C, typename A>
struct IndexableTraits <
    std::map<K, T, C, A>,
    typename std::enable_if<std::is_integral<K>::value>::type
    > : IndexableTraitsAssoc<std::map<K, T, C, A>> {
};

namespace detail {
template<typename T>
struct KeyFromSlice;

template<>
struct KeyFromSlice<std::string> : public FormatTraitsBase {
  using key_type = std::string;
  static std::string convert(alpha::Slice s) {
    return s.str();
  }
};

template<>
struct KeyFromSlice<alpha::Slice> : public FormatTraitsBase {
  using key_type = alpha::Slice;
  static alpha::Slice convert(alpha::Slice s) {
    return s;
  }
};

template<typename T>
int UIntToDec(FormatArg& arg, char* &p, T val) {
  using UT = typename std::make_unsigned<T>::type;
  UT v = static_cast<UT>(val >= 0 ? val : -val);
  auto idx = 1;
  while (v != 0) {
    uint8_t mod = v % 10;
    *--p = '0' + mod;
    v /= 10;
    if (v != 0
        && arg.thousands_separator
        && idx != 0
        && idx % 3 == 0) {
      *--p = ',';
    }
    ++idx;
  }
  if (val == 0) {
    *--p = '0';
  }
  return 0;
}

template<typename T>
int UIntToHex(FormatArg& arg, char* &p, T val, const char* repr) {
  using UT = typename std::make_unsigned<T>::type;
  UT mask = static_cast<UT>(0xF);
  UT v = static_cast<UT>(val >= 0 ? val : -val);
  while (v != 0) {
    uint8_t piece = v & mask;
    *--p = repr[piece];
    v >>= 4;
  }
  if (val == 0) {
    *--p = '0';
  }
  if (arg.base_prefix) {
    *--p = 'x';
    *--p = '0';
    return 2;
  } else {
    return 0;
  }
}

template<typename T>
int UIntToOct(FormatArg& arg, char* &p, T val, const char* repr) {
  using UT = typename std::make_unsigned<T>::type;
  UT mask = static_cast<UT>(0x7);
  UT v = static_cast<UT>(val >= 0 ? val : -val);
  while (v != 0) {
    uint8_t piece = v & mask;
    *--p = repr[piece];
    v >>= 3;
  }
  if (val == 0) {
    *--p = '0';
  }
  if (arg.base_prefix) {
    *--p = '0';
    return 1;
  } else {
    return 0;
  }
}
template<typename T>
int UIntToBin(FormatArg& arg, char* &p, T val, const char* repr) {
  using UT = typename std::make_unsigned<T>::type;
  UT mask = static_cast<UT>(0x1);
  UT v = static_cast<UT>(val >= 0 ? val : -val);
  while (v != 0) {
    uint8_t piece = v & mask;
    *--p = repr[piece];
    v >>= 1;
  }
  if (val == 0) {
    *--p = '0';
  }
  if (arg.base_prefix) {
    *--p = repr[2];
    *--p = '0';
    return 2;
  } else {
    return 0;
  }
}

template<typename T>
int DoubleToString(FormatArg& arg, char* &p, double val) {
  char presentation = arg.presentation;
  if (presentation == FormatArg::kDefaultPresentation) {
    presentation = 'g';
  } else if (presentation == 'F') {
    presentation = 'f';
  }
  char format[20];
  if (arg.precision == FormatArg::kDefaultPrecision) {
    snprintf(format, sizeof(format), "%%%c", presentation);
  } else {
    snprintf(format, sizeof(format), "%%.%d%c", arg.precision, presentation);
  }
  char buf[256];
  auto n = snprintf(buf, sizeof(buf), format, val);
  p -= n;
  memcpy(p, buf, n);
  if (*p == '-') {
    return 1;
  }
  return 0;
}

template<typename Output>
void FormatString(FormatArg& arg, Output& output, alpha::Slice s) {
  char fill = arg.fill == FormatArg::kDefaultFill ? ' ' : arg.fill;
  int padding_size = arg.width - static_cast<int>(s.size());
  auto pad = [&fill, &output](int size) {
    assert (size >= 0);
    static const int kMaxPaddingSize = 128;
    char padding_buffer[kMaxPaddingSize];
    int realsize = std::min(size, kMaxPaddingSize);
    memset(padding_buffer, fill, realsize);
    output(alpha::Slice(padding_buffer, realsize));
  };
  switch (arg.align) {
    case FormatArg::Align::kDefault:
    case FormatArg::Align::kLeft:
      output(s);
      pad(std::max(0, padding_size));
      break;
    case FormatArg::Align::kRight:
    case FormatArg::Align::kPadAfterSign:
      pad(std::max(0, padding_size));
      output(s);
      break;
    case FormatArg::Align::kCenter:
      pad(std::max(0, padding_size / 2));
      output(s);
      pad(std::max(0, padding_size - padding_size / 2));
      break;
    default:
      assert (false);
  }
}

template<typename Output>
void FormatNumber(FormatArg& arg, Output& output, int prefix_len, alpha::Slice s) {
  assert (static_cast<int>(s.size()) >= prefix_len);
  if (arg.align == FormatArg::Align::kDefault) {
    arg.align = FormatArg::Align::kRight;
  } else if (prefix_len && arg.align == FormatArg::Align::kPadAfterSign) {
    output(s.subslice(0, prefix_len));
    s.advance(prefix_len);
    arg.width = std::max(arg.width - prefix_len, 0);
  }
  detail::FormatString(arg, output, s);
}

template<typename T, class Enable = void>
struct IsNegative;

template<typename T>
struct IsNegative < T, typename std::enable_if <
    (std::is_integral<T>::value && !std::is_same<T, bool>::value)
  || std::is_floating_point<T>::value
    >::type > {
  bool operator()(T val) {
    return std::is_signed<T>::value && val < 0;
  }
};
} //namespace detail

template<typename C>
struct KeyableTraitsAssoc : FormatTraitsBase {
  using key_type = typename C::key_type;
  using value_type = typename C::value_type::second_type;
  static const value_type& at(const C& c, alpha::Slice key) {
    return c.at(detail::KeyFromSlice<key_type>::convert(key));
  }
};

template<typename T, class Enable = void>
struct KeyableTraits;

template<typename K, typename T, typename C, typename A>
struct KeyableTraits <
    std::map<K, T, C, A>,
    typename detail::KeyFromSlice<K>::enable
    > : public KeyableTraitsAssoc<std::map<K, T, C, A>> {
};

template<typename T, class Enable = void>
struct IsSomeStringPrototype;

template<typename T>
struct IsSomeStringPrototype < T, typename std::enable_if <
    std::is_same<std::string, T>::value
    || std::is_same<alpha::Slice, T>::value
    || std::is_same<const char*, T>::value
    >::type > {
  using enable = void;
};

template<typename T, class Enable = void>
class FormatValue;

template<typename T>
class FormatValue<T, typename KeyableTraits<T>::enable> {
 public:
  FormatValue(const T& val)
    : val_ (val) {
  }

  template<typename Output>
  void Format(FormatArg& arg, Output& output) const {
    auto piece = arg.SplitKey();
    FormatValue<typename std::decay<
    typename KeyableTraits<T>::value_type
    >::type
    >(KeyableTraits<T>::at(val_, piece)).Format(arg, output);
  }

 private:
  const T& val_;
};

//string type
template<typename T>
class FormatValue<T, typename IsSomeStringPrototype<
  typename std::decay<T>::type
  >::enable> {
 public:
  FormatValue(const T& val)
    : val_(val) {
  }

  template<typename Output>
  void Format(FormatArg& arg, Output& output) const {
    alpha::Slice val = val_;
    if (arg.precision != FormatArg::kDefaultPrecision) {
      val = val.subslice(0, arg.precision);
    }
    detail::FormatString(arg, output, val);
  }

 private:
  alpha::Slice val_;
};

template<typename T>
class FormatValue<T, typename std::enable_if<
  std::is_same<
  typename std::decay<T>::type,
  double>::value
  >::type> {
 public:
  FormatValue(const T& val)
    : val_(val) {
  }

  template<typename Output>
  void Format(FormatArg& arg, Output& output) const {
    static const int kBufferSize = 1024;
    char buf[kBufferSize];
    char* p = buf + kBufferSize;
    auto prefix_len = detail::DoubleToString<T>(arg, p, val_);
    detail::FormatNumber(arg, output, prefix_len, alpha::Slice(p, buf + kBufferSize));
  }

 private:
  T val_;
};

template<typename T>
class FormatValue<T, typename std::enable_if<
  std::is_same<
  typename std::decay<T>::type,
  float>::value
  >::type> {
 public:
  FormatValue(const T& val)
    : val_(val) {
  }

  template<typename Output>
  void Format(FormatArg& arg, Output& output) const {
    FormatValue<double>(val_).Format(arg, output);
  }

 private:
  T val_;
};

//integer
template<typename T>
class FormatValue < T, typename std::enable_if <
  std::is_integral<T>::value
  && !std::is_same<T, bool>::value
  >::type
  > {
 public:
  FormatValue(const T& val)
    : val_(val) {
  }
  template<typename Output>
  void Format(FormatArg& arg, Output& output) const {
    const char upper_repr_hex[] = {
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
      'A', 'B', 'C', 'D', 'E', 'F'
    };
    const char lower_repr_hex[] = {
      '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
      'a', 'b', 'c', 'd', 'e', 'f'
    };
    const char repr_oct[] = {
      '0', '1', '2', '3', '4', '5', '6', '7'
    };
    const char lower_repr_bin[] = {
      '0', '1', 'b'
    };
    const char upper_repr_bin[] = {
      '0', '1', 'B'
    };
    /*
     * 1 for '+'/'-'
     * 2 for prefix
     * 64 for 64bits
     * total: 67bits
     */
    static const int kBufferSize = 67;
    char buf[kBufferSize];
    char* p = buf + kBufferSize;
    int prefix_len = 0;
    switch (arg.presentation) {
      case FormatArg::kDefaultPresentation:
      case 'd':
      case 'n':
        prefix_len = detail::UIntToDec(arg, p, val_);
        break;
      case 'x':
        prefix_len = detail::UIntToHex(arg, p, val_, lower_repr_hex);
        break;
      case 'X':
        prefix_len = detail::UIntToHex(arg, p, val_, upper_repr_hex);
        break;
      case 'o':
      case 'O':
        prefix_len = detail::UIntToOct(arg, p, val_, repr_oct);
        break;
      case 'b':
        prefix_len = detail::UIntToBin(arg, p, val_, lower_repr_bin);
        break;
      case 'B':
        prefix_len = detail::UIntToBin(arg, p, val_, upper_repr_bin);
        break;
      case 'c':
        *--p = static_cast<char>(val_);
        prefix_len = 0;
        break;
      default:
        assert (false);
    }
    switch (arg.sign) {
      case FormatArg::Sign::kDefault:
      case FormatArg::Sign::kMinus:
        if (detail::IsNegative<T>()(val_)) {
          *--p = '-';
          ++prefix_len;
        }
        break;
      case FormatArg::Sign::kPlusOrMinus:
        *--p = detail::IsNegative<T>()(val_) ? '-' : '+';
        ++prefix_len;
        break;
      case FormatArg::Sign::kSpaceOrMinus:
        *--p = detail::IsNegative<T>()(val_) ? '-' : ' ';
        ++prefix_len;
        break;
      default:
        assert (false);
    }
    //auto s = alpha::Slice(p, buf + kBufferSize).str();
    //std::cout << "`" << s << "' = " << s.size() << '\n';
    detail::FormatNumber(arg, output, prefix_len, alpha::Slice(p, buf + kBufferSize));
  }

 private:
  const T& val_;
};

//bool
template<typename T>
class FormatValue<T, typename std::enable_if<
  std::is_same<T, bool>::value
  >::type
  > {
 public:
  FormatValue(const T& val)
    : val_(val) {
  }

  template<typename Output>
  void Format(FormatArg& arg, Output& output) const {
    if (arg.presentation == FormatArg::kDefaultPresentation) {
      detail::FormatString(arg, output, val_ ? "true" : "false");
    } else {
      FormatValue<int>(val_).Format(arg, output);
    }
  }

 private:
  const T& val_;
};

//nullptr
template<typename T>
class FormatValue<T, typename std::enable_if<
  std::is_same<T, std::nullptr_t>::value
  >::type
  > {
 public:
  FormatValue(std::nullptr_t val)
    : val_(val) {
  }

  template<typename Output>
  void Format(FormatArg& arg, Output& output) const {
    output("(null)");
  }

 private:
  std::nullptr_t val_;
};

//tuple
template<typename... Args>
class FormatValue<std::tuple<Args...>> {
 public:
  using Tuple = std::tuple<Args...>;
  static const size_t kTupleSize = std::tuple_size<Tuple>::value;
  FormatValue(const Tuple& val)
    : val_(val) {
  }

  template<typename Output>
  void Format(FormatArg& arg, Output& output) const {
    FormatFrom<0>(arg.SplitIntKey(), arg, output);
  }

 private:
  template<size_t K, typename Output>
  typename std::enable_if<K == kTupleSize>::type
  FormatFrom(size_t index, FormatArg& arg, Output& output) const {
    throw BadFormatException("Format Tuple: index out of range");
  }

  template<size_t K, typename Output>
  typename std::enable_if < K < kTupleSize>::type
  FormatFrom(size_t index, FormatArg& arg, Output& output) const {
    if (index == K) {
      FormatValue<typename std::decay<
      typename std::tuple_element<K, Tuple>::type>::type
      >(std::get<K>(val_)).Format(arg, output);
    } else {
      FormatFrom < K + 1 > (index, arg, output);
    }
  }

 private:
  const Tuple& val_;
};

template<typename T>
class FormatValue<T, typename IndexableTraits<T>::enable> {
 public:
  FormatValue(const T& val)
    : val_(val) {
  }
  template<typename Output>
  void Format(FormatArg& arg, Output& output) const {
    FormatValue<typename std::decay<
    typename IndexableTraits<T>::value_type>::type>(
      IndexableTraits<T>::at(val_, arg.SplitIntKey())).Format(arg, output);
  }
 private:
  const T& val_;
};

template<bool ContainerMode, typename... Args>
class Formatter {
 private:
  using ValueTuple = std::tuple<FormatValue<
                     typename std::decay<Args>::type>...>;
  static const size_t ValueSize = std::tuple_size<ValueTuple>::value;

 public:
  Formatter(const char* str, Args&& ... args)
    : str_(str), values_(args...) {
    static_assert(!ContainerMode || ValueSize == 1,
                  "Expect only one argument in ContainerMode");
  }

  template<typename StringType>
  void AppendTo(StringType* to) const {
    auto output = [to](alpha::Slice s) {
      to->append(s.data(), s.size());
    };
    (*this)(output);
  }

  std::string str() const {
    std::string s;
    AppendTo(&s);
    return s;
  }

  template<typename Output>
  void operator()(Output& out) const {
    auto output = [&out](alpha::Slice str) {
      auto s = str.begin();
      auto e = str.end();

      while (s != e) {
        auto p = strnchr(s, '}', e - s);
        if (p && *(p + 1) != '}') {
          throw BadFormatException("single `}'");
        } else if (p) {
          out(alpha::Slice(s, p + 1));
          s = p + 2;
        } else {
          out(alpha::Slice(s, e));
          break;
        }
      }
    };
    alpha::Slice str(str_);
    auto s = str.begin();
    auto e = str.end();
    bool has_implicit_index = false;
    bool has_explicit_index = false;
    unsigned arg_index = 0;

    while (s != e) {
      auto p = strchr(s, '{');
      if (!p) {
        output(alpha::Slice(s, e));
        break;
      }
      output(alpha::Slice(s, p));
      auto q = p + 1;
      if (*q == '{') {
        output(alpha::Slice(p, 1));
        s = q + 1;
        continue;
      }

      q = strchr(p + 1, '}');
      if (!q) {
        throw BadFormatException("unmatched `}'");
      }
      FormatArg arg(alpha::Slice(p + 1, q));
      auto piece = arg.SplitKey();
      unsigned index = 0;
      if (ContainerMode) {
        if (piece.empty()) {
          arg.SetNextIntKey(arg_index++);
          has_implicit_index = true;
        } else {
          arg.SetNextKey(piece);
          has_explicit_index = true;
        }
      } else {
        if (piece.empty()) {
          index = arg_index++;
          has_implicit_index = true;
        } else {
          size_t e;
          index = std::stoul(piece.str(), &e, 10);
          if (e != piece.size()) {
            throw BadFormatException("Index must be integer");
          }
          has_explicit_index = true;
        }
      }
      if (has_explicit_index && has_implicit_index) {
        throw BadFormatException("Found both explicit and implicit index");
      }
      DoFormatFrom<0>(index, arg, output);
      s = q + 1;
    }
  }

  template<size_t K, typename Output>
  typename std::enable_if<K == ValueSize>::type
  DoFormatFrom(unsigned index, FormatArg& arg, Output& output) const {
    throw BadFormatException("Argument index out of range");
  }

  template<size_t K, typename Output>
  typename std::enable_if < K < ValueSize>::type
  DoFormatFrom(unsigned index, FormatArg& arg, Output& output) const {
    if (index == K) {
      std::get<K>(values_).Format(arg, output);
    } else {
      DoFormatFrom < K + 1 > (index, arg, output);
    }
  }
  const char* str_;
  ValueTuple values_;
};

template<bool ContainerMode, typename... Args>
std::ostream& operator<<(std::ostream& os,
                         const Formatter<ContainerMode, Args...>& formatter) {
  auto output = [&os](alpha::Slice s) {
    os.write(s.data(), s.size());
  };
  formatter(output);
  return os;
}

template<typename... Args>
Formatter<false, Args...> Format(const char* fmt, Args&& ... args) {
  return Formatter<false, Args...>(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
std::string sFormat(const char* fmt, Args&& ... args) {
  return Formatter<false, Args...>(fmt, std::forward<Args>(args)...).str();
}

template<typename C>
Formatter<true, C> vFormat(const char* fmt, C&& c) {
  return Formatter<true, C>(fmt, std::forward<C>(c));
}

template<typename C>
std::string svFormat(const char* fmt, C&& c) {
  return Formatter<true, C>(fmt, std::forward<C>(c)).str();
}

int main() {
#if 0
  {
    FormatArg arg("");
    assert (arg.SplitKey().empty());
  } {
    FormatArg arg(":X<4");
    assert (arg.SplitKey().empty());
  } {
    FormatArg arg("123");
    assert (arg.SplitKey() == "123");
  } {
    FormatArg arg("123:X<10");
    assert (arg.SplitKey() == "123");
  } {
    FormatArg arg("123[key]");
    assert (arg.SplitKey() == "123");
  } {
    FormatArg arg("123[key]:X<10");
    assert (arg.SplitKey() == "123");
  } {
    FormatArg arg("key");
    assert (arg.SplitKey() == "key");
  } {
    FormatArg arg("key:X<10");
    assert (arg.SplitKey() == "key");
  }
#endif
#if 0
  // Objects produced by Format() can be streamed without creating
  // an intermediary string; {} yields the next argument using default
  // formatting.
  std::cout << Format("The answers are {} and {}\n", 23, 42);
  // => "The answers are 23 and 42"
  // If you just want the string, though, you're covered.
  std::string result = sFormat("The answers are {} and {}", 23, 42);
  // => "The answers are 23 and 42"

  // To insert a literal '{' or '}', just double it.
  std::cout << Format("{} {{}} {{{}}}\n", 23, 42);
  // => "23 {} {42}"

  // Arguments can be referenced out of order, even multiple times
  std::cout << Format("The answers are {1}, {0}, and {1} again\n", 23, 42);
  // => "The answers are 42, 23, and 42 again"

  // It's perfectly fine to not reference all arguments
  std::cout << Format("The only answer is {1}\n", 23, 42);
  // => "The only answer is 42"

  // Values can be extracted from indexable containers
  // (random-access sequences and integral-keyed maps), and also from
  // string-keyed maps
  std::vector<int> v {23, 42};
  std::map<std::string, std::string> m { {"what", "answer"} };
  std::cout << Format("The only {1[what]} is {0[1]}\n", v, m);
  // => "The only answer is 42"

  // If you only have one container argument, vformat makes the syntax simpler
  std::map<std::string, std::string> m1 { {"what", "answer"}, {"value", "42"} };
  std::cout << vFormat("The only {what} is {value}\n", m1);
  // => "The only answer is 42"
  // same as
  std::cout << Format("The only {0[what]} is {0[value]}\n", m1);
  // => "The only answer is 42"
  // And if you just want the string,
  std::string result2 = svFormat("The only {what} is {value}\n", m1);
  // => "The only answer is 42"
  std::string result3 = sFormat("The only {0[what]} is {0[value]}\n", m1);
  std::cout <<  result3;
  // => "The only answer is 42"
  // {} works for vformat too
  std::vector<int> v2 {42, 23};
  std::cout << vFormat("{} {}\n", v2);
  // => "42 23"

  // format and vformat work with pairs and tuples
  std::tuple<int, std::string, int> t {42, "hello", 23};
  std::cout << vFormat("{0} {2} {1}\n", t);
  // => "42 23 hello"

  // Format supports width, alignment, arbitrary fill, and various
  // format specifiers, with meanings similar to printf
  // "X<10": fill with 'X', left-align ('<'), width 10
  std::cout << Format("{:X<10} {}\n", "hello", "world");
  // => "helloXXXXX world"
  // Format supports printf-style format specifiers
  std::cout << Format("{0:05d} decimal = {0:04x} hex\n", 42);
  // => "00042 decimal = 002a hex"

  // Formatter objects may be written to a string using folly::to or
  // folly::toAppend (see folly/Conv.h), or by calling their appendTo(),
  // str(), and fbstr() methods
  std::string s = Format("The only answer is {}\n", 42).str();
  std::cout << s;
  // => "The only answer is 42"
#endif
}
