#include "Log.h"

std::unique_ptr<LogFile> Log::file;
std::unique_ptr<LogFile> Log::internal;
bool Log::canLogDebug = false;