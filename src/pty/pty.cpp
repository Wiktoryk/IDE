#include <QtCore/QtCore>

// PTY stub; platform-specific implementations will live here (openpty/ConPTY).
bool pty_is_supported()
{
#if defined(_WIN32)
    return true; // ConPTY on Win10+ (to be implemented)
#else
    return true; // openpty on Unix (to be implemented)
#endif
}
