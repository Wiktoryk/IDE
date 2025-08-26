#include <git2.h>
#include <QtCore/QtCore>

// Minimal check to ensure libgit2 is linked correctly
bool git_available()
{
    return (git_libgit2_version(NULL, NULL, NULL), true);
}
