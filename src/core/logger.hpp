#ifndef BIRBS_CORE_LOG_H
#define BIRBS_CORE_LOG_H 1

#include <quill/SimpleSetup.h>

inline quill::Logger* BIRBS_LOGGER = quill::simple_logger();

#define VOX_LOG_INFO(x) QUILL_LOG_INFO(BIRBS_LOGGER, x)
#define VOX_LOG_DEBUG(x) QUILL_LOG_DEBUG(BIRBS_LOGGER, x)

#endif