
#ifndef GLOGCONFIG_H_
#define GLOGCONFIG_H_
#include <atomic>
#include <iostream>
#include <string.h>
#include <libconfig.h++>
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include "Utils.h"

/**
* @class GLogConfig
* @brief Class to manage the configuration of glog library.
*   Do not instantiate this class. Instead use the static members.
*   Call InitGLog in your main function to perform a simple glog
*   initialization which logs to stderr. Later, call SetupGLog with a
*   libconfig configuration file to make a complete setup.
*/
class GLogConfig {
public:
  GLogConfig() = delete;

  /**
  * @brief Setup glog from a config file.
  */
  static bool SetupGLog(libconfig::Setting &config) {
    try {
      // Read from config
      bool read_ok = true;
      int setting_FLAGS_v, setting_FLAGS_stderrthreshold,
        setting_FLAGS_minloglevel;
      std::string logging_name, path_glog_info, path_glog_warning,
        path_glog_error;

      read_ok &= ReadSettingSafe(config, "logging_name",
                                 logging_name);
      read_ok &= ReadSettingSafe(config, "path_glog_info",
                                 path_glog_info);
      read_ok &= ReadSettingSafe(config, "path_glog_warning",
                                 path_glog_warning);
      read_ok &= ReadSettingSafe(config, "path_glog_error",
                                 path_glog_error);
      read_ok &= ReadSettingSafe(config, "FLAGS_v",
                                 setting_FLAGS_v);
      read_ok &= ReadSettingSafe(config, "FLAGS_stderrthreshold",
                                 setting_FLAGS_stderrthreshold);
      read_ok &= ReadSettingSafe(config, "FLAGS_minloglevel",
                                 setting_FLAGS_minloglevel);

      if (!read_ok) {
        std::cerr << "Missing configuration properties" << std::endl;
        return false;
      }

      // Setup glog
      //fLB::FLAGS_colorlogtostderr = true;
      fLB::FLAGS_alsologtostderr = true;
      fLI::FLAGS_stderrthreshold = setting_FLAGS_stderrthreshold;
      fLI::FLAGS_minloglevel = setting_FLAGS_minloglevel;
      fLI::FLAGS_v = setting_FLAGS_v;

      if (!path_glog_info.empty())
        google::SetLogDestination(google::GLOG_INFO,
                                  path_glog_info.c_str());
      if (!path_glog_warning.empty())
        google::SetLogDestination(google::GLOG_WARNING,
                                  path_glog_warning.c_str());
      if (!path_glog_error.empty())
        google::SetLogDestination(google::GLOG_ERROR,
                                  path_glog_error.c_str());

    //  char * logging_name_leak = strdup(logging_name.c_str());
      char * logger_name = new char[50];
      *strcpy(logger_name, logging_name.c_str());
      google::InitGoogleLogging(logger_name);
      GlogIsInit::Set(true);
    } catch (libconfig::ParseException e) {
      std::cerr << "ParseException: " << e.what() << std::endl;
      return false;
    } catch (libconfig::FileIOException e) {
      std::cerr << "FileIOException: " << e.what() << std::endl;
      return false;
    } catch (libconfig::ConfigException e) {
      std::cerr << "ConfigException: " << e.what() << std::endl;
      return false;
    } catch (...) {
      std::cerr << "Unexpected Exception" << std::endl;
      return false;
    }
    return true;
  }
private:
  template <typename TType>
  static bool ReadSettingSafe(libconfig::Setting &setting,
                              const char *name,
                              TType &target) throw() {
    if (!setting.exists(name) || !setting.lookupValue(name, target)) {
      std::cerr << "Config file error. Missing property '"
        << name << "' in " << setting.getPath() << std::endl;
      return false;
    }
    return true;
  }
};

#endif //GLOGCONFIG_H_