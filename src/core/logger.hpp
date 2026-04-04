#ifndef SOCCER_CORE_LOG_H
#define SOCCER_CORE_LOG_H 1

#include <quill/SimpleSetup.h>

inline quill::Logger* SOCCER_LOGGER = quill::simple_logger();

#define SOCCER_LOG_INFO(...) QUILL_LOG_INFO(SOCCER_LOGGER, __VA_ARGS__)
#define SOCCER_LOG_DEBUG(...) QUILL_LOG_DEBUG(SOCCER_LOGGER, __VA_ARGS__)

#endif