#include <QtCore/QtCore>

// Very small placeholder for an rg runner; real impl will use QProcess to stream stdout.
QString ripgrepBinaryGuess()
{
#ifdef _WIN32
    return QStringLiteral("rg.exe");
#else
    return QStringLiteral("rg");
#endif
}
