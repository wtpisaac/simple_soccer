#ifndef BIRBS_CORE_LOG_H
#define BIRBS_CORE_LOG_H 1

#include <quill/SimpleSetup.h>

inline quill::Logger* BIRBS_LOGGER = quill::simple_logger();

#define BIRBS_LOG_INFO(...) QUILL_LOG_INFO(BIRBS_LOGGER, __VA_ARGS__)
#define BIRBS_LOG_DEBUG(...) QUILL_LOG_DEBUG(BIRBS_LOGGER, __VA_ARGS__)

#endif