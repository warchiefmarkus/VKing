#ifndef VK_JSON_H
#define VK_JSON_H
#include <QVariantMap>



namespace JSON {
   QVariant parse(const QByteArray &data);
   QByteArray generate(const QVariant &data, int indent = 0);
} //namespace JSON


#endif // VK_JSON_H

