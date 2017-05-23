#ifndef ENCODING_H_
#define ENCODING_H_

#include <stdint.h>
#include <stddef.h>
#include <mutex>
#include <iostream>
#include <codecvt>
#include <string>
#include <vector>
#include <limits>
#include <locale>

#if defined(_MSC_VER)
using encodinglib_wchar_t = int16_t;
#else
using encodinglib_wchar_t = char16_t;
#endif

/**
* @enum CharEncodingId
* @brief  List of character encoding ids.
*/
enum class CharEncodingId {
  UTF8,
  UTF16,
  UCS2,
  WIN1252,
  UNSPECIFIED
};

/**
* @class EncodingLib_Conv_UTF8N_UTF16W
* @brief  Thread-Safe Singleton for ut16 wide string to utf8 narrow string
*   and vice versa.
*/
class EncodingLib_Conv_UTF8N_UTF16W {
public:
  /** @brief Destructor */
  ~EncodingLib_Conv_UTF8N_UTF16W() {};

  /** @brief Get the static singleton instance */
  static EncodingLib_Conv_UTF8N_UTF16W & Instance() {
    static EncodingLib_Conv_UTF8N_UTF16W conv;
    return conv;
  }

  /**
  * @brief Convert from utf16 wide string to utf8 narrow string.
  * @param utf16_wstr  Input utf16 wide string.
  * @param ok  Optional ok flag. Set to true if the conversion was
  *   successful, and to false otherwise.
  * @returns Converted utf8 narrow string.
  */
  static std::string ToUTF8N(const std::u16string &utf16_wstr,
                             bool *ok = nullptr) {
    return Instance().InstanceToUTF8N(utf16_wstr, ok);
  }

  /**
  * @brief Convert from utf16 wide char sequence to utf8 narrow string.
  * @param utf16_wseq  Input utf16 wide char sequence.
  * @param num_welements  Number of wchar_t elements in utf16_wseq.
  * @param ok  Optional ok flag. Set to true if the conversion was
  *   successful, and to false otherwise.
  * @returns Converted utf8 narrow string.
  */
  static std::string ToUTF8N(const char16_t *utf16_wseq,
                             size_t num_welements,
                             bool *ok = nullptr) {
    return Instance().InstanceToUTF8N(utf16_wseq, num_welements, ok);
  }

  /**
  * @brief Convert from utf8 narrow string to utf16 wide string.
  * @param utf8_nstr  Input utf8 narrow string.
  * @param ok  Optional ok flag. Set to true if the conversion was
  *   successful, and to false otherwise.
  * @returns Converted utf16 wide string.
  */
  static std::u16string ToUTF16W(const std::string &utf8_nstr,
                                 bool *ok = nullptr) {
    return Instance().InstanceToUTF16W(utf8_nstr, ok);
  }

  /** @brief Deleted method (ilegal for singleton) */
  EncodingLib_Conv_UTF8N_UTF16W(const EncodingLib_Conv_UTF8N_UTF16W&) = delete;

  /** @brief Deleted method (ilegal for singleton) */
  void operator=(const EncodingLib_Conv_UTF8N_UTF16W&) = delete;

protected:
  /** @brief Protected constructor (singleton pattern) */
  EncodingLib_Conv_UTF8N_UTF16W() {};

private:
  std::string InstanceToUTF8N(const std::u16string &utf16_wstr,
                              bool *ok = nullptr) {
    if (ok != nullptr) *ok = true;
    try {
      std::lock_guard<std::mutex> lock(m_conv_lock);
      auto p = reinterpret_cast<const encodinglib_wchar_t *>(utf16_wstr.c_str());
      return m_utf8str_utf16wstr.to_bytes(p, p + utf16_wstr.size());
    } catch (...) {
      if (ok != nullptr) *ok = false;
      return{};
    }
  }

  std::string InstanceToUTF8N(const char16_t *utf16_wseq,
                              size_t num_welements,
                              bool *ok = nullptr) {
    if (ok != nullptr) *ok = true;
    try {
      std::lock_guard<std::mutex> lock(m_conv_lock);
      auto p = reinterpret_cast<const encodinglib_wchar_t *>(utf16_wseq);
      return m_utf8str_utf16wstr.to_bytes(p, p + num_welements);
    } catch (...) {
      if (ok != nullptr) *ok = false;
      return{};
    }
  }

  std::u16string InstanceToUTF16W(const std::string &utf8_nstr,
                                  bool *ok = nullptr) {
    if (ok != nullptr) *ok = true;
    try {
      std::lock_guard<std::mutex> lock(m_conv_lock);

      auto r = m_utf8str_utf16wstr.from_bytes(utf8_nstr);
      auto p = reinterpret_cast<const encodinglib_wchar_t *>(r.c_str());
      return  std::u16string(p, p + r.size());
    } catch (...) {
      if (ok != nullptr) *ok = false;
      return{};
    }
  }

  /** @brief Allow thread safe access to the underlying converter object */
  std::mutex m_conv_lock;

  /** @brief Convert from utf8 string to utf16 wstring and vice versa */
  std::wstring_convert<std::codecvt_utf8_utf16<encodinglib_wchar_t>, encodinglib_wchar_t>
    m_utf8str_utf16wstr;
};

#endif
