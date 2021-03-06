#pragma once

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <exception>
#include <stdexcept>

template <typename Traits> class Dictionary;

struct KeyTraits {
  typedef std::string key_t;
  typedef std::string value_t;
  typedef std::pair<key_t, value_t> item_t;
  static const char* begin_text() { return ""; }
  static const char* separator_text() { return " = "; }
  static const char* end_text() { return "\n"; }
};

typedef Dictionary<KeyTraits> INISection;

struct SectionTraits {
  typedef std::string key_t;
  typedef INISection value_t;
  typedef std::pair<key_t, value_t> item_t;
  static const char* begin_text() { return "["; }
  static const char* separator_text() { return "]\n"; }
  static const char* end_text() { return "\n"; }
};

template <typename Traits>
class Dictionary {
  public:
  typedef typename Traits::key_t key_t;
  typedef typename Traits::value_t value_t;
  typedef std::map<key_t, value_t> storage_t;
  typedef typename storage_t::iterator iterator;
  typedef typename storage_t::const_iterator const_iterator;

  private:
  static value_t empty;
  storage_t data;
    
  public:
  const value_t& operator[](const key_t& key) const {
    const_iterator it = data.find(key);
    const value_t& result = (it == data.end()) ? empty : it->second;
    return result;
  }

  value_t& operator[](const key_t& key) {
    value_t& result = data[key];
    return data[key];
  }

  friend std::ostream& operator<<(std::ostream& os, const Dictionary<Traits>& d) {
    for (const_iterator it = d.data.begin(); it != d.data.end(); ++it) {
      os << Traits::begin_text() << it->first
         << Traits::separator_text() << it->second
         << Traits::end_text();
    }
    return os;
  }
};

// There's got to be a less verbose way to do this initialization :(
template<typename Traits>
typename Dictionary<Traits>::value_t Dictionary<Traits>::empty = typename Dictionary<Traits>::value_t();

typedef Dictionary<SectionTraits> INIBase;

class INIFile : public INIBase {
  std::string filename;

  static void trim(std::string& to_trim) {
    int lstrip = to_trim.find_first_not_of(" \t\n");
    int rstrip = to_trim.find_last_not_of(" \t\n");
    if (lstrip == std::string::npos || rstrip == std::string::npos) {
      to_trim = "";
    } else {
      to_trim = to_trim.substr(lstrip, rstrip + 1);
    }
  }
  
  public:
  bool writeback;
  
  INIFile(const std::string& filename, bool writeback = true, bool exists = true):
    filename(filename), writeback(writeback), INIBase(exists ? createBase(filename) : INIBase()) {}

  static INIBase createBase(const std::string& filename) {
    // We construct with exists = false if we want to make a new INI file.
    INIBase tmp;

    std::string current_line;
    std::string current_section = "";
    std::ifstream input(filename.c_str());
    if (!input.is_open()) {
      throw std::runtime_error("INI file not found");
    }

    while (std::getline(input, current_line)) {
      // Trim whitespace from line
      trim(current_line);
      // Skip empty lines
      if (current_line.empty()) { continue; }
      int last_index = current_line.length() - 1;
      // Make sure this is clearly either a section title or key=value pair.
      if (current_line[0] == '[' && current_line[last_index] == ']') {
        if (current_line.find_first_of("[]", 1) != last_index) {
          throw std::runtime_error("found brackets in mid-line");
        }

        current_section = current_line.substr(1, last_index - 1);
        trim(current_section);
        if (current_section == "") {
          throw std::runtime_error("empty section name");
        }
      } else {
        // This should be a key=value pair. Make sure some section is "open".
        if (current_section == "") {
          throw std::runtime_error("keys before the first section");
        }
        // Find the '=' and make sure there's only one.
        int pos = current_line.find('=');
        if (pos == std::string::npos) {
          throw std::runtime_error("no = found in expected key=value pair");
        }
        if (pos != current_line.rfind('=')) {
          throw std::runtime_error(">1 =s found in expected key=value pair");
        }
        // Split up key and value and do the insertion
        std::string key(current_line, 0, pos); trim(key);
        std::string value(current_line, pos + 1); trim(value);
        tmp[current_section][key] = value;
      }
    }
    return tmp;
  }

  ~INIFile() {
    if (writeback) {
      write_to_file();
    }
  }

  void write_to_file() {
    std::ofstream output(filename.c_str());
    output << *this;
  }
};