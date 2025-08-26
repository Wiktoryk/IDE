#include <QString>
#include <vector>

struct DummyBuffer {
    QString data;
    void insert(int pos, const QString& s) { data.insert(pos, s); }
    void erase(int pos, int len) { data.remove(pos, len); }
};
