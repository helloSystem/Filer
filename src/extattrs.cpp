#include "extattrs.h"

#include <sys/param.h> // for checking BSD definition
#if defined(BSD)
#include <sys/extattr.h>
#else
#include <sys/types.h>
#include <sys/xattr.h>
#endif

#include <QDebug>
#include <QProcess>
#include <QStandardPaths>

#define XATTR_NAMESPACE "user"

using namespace Fm;

namespace Fm {

static const int ATTR_VAL_SIZE = 2048; // probono: Can we do without a predetermined size?
// If this size is too small, then reading extattr fails, leading to strange unexpected errors.
// 256 was not enough to read, e.g., 'can-open' containing many MIME types

/*
 * get the attibute value from the extended attribute for the path as int
 */
int getAttributeValueInt(const QString& path, const QString& attribute, bool& ok) {
  int value = 0;

  // get the value from the extended attribute for the path
  char data[ATTR_VAL_SIZE];
#if defined(BSD)
  ssize_t bytesRetrieved = extattr_get_file(path.toLatin1().data(), EXTATTR_NAMESPACE_USER,
                                                    attribute.toLatin1().data(), data, ATTR_VAL_SIZE);
#else
  QString namespacedAttr;
  namespacedAttr.append(XATTR_NAMESPACE).append(".").append(attribute);
  ssize_t bytesRetrieved = getxattr(path.toLatin1().data(),
                                            namespacedAttr.toLatin1().data(), data, ATTR_VAL_SIZE);
#endif
  // check if we got the attribute value
  if (bytesRetrieved <= 0)
    ok = false;
  else {
    // convert the value to int via QString
    QString strValue(data);
    bool intOK;
    int val = strValue.toInt(&intOK);
    if (intOK) {
      ok = true;
      value = val;
    }
  }
  return value;
}

/*
 * set the attibute value in the extended attribute for the path as int
 */
bool setAttributeValueInt(const QString& path, const QString& attribute, int value) {
    // set the value from the extended attribute for the path
    const QString data = QString::number(value);
    return setAttributeValueQString(path, attribute, data);
}

/*
 * get the attibute value from the extended attribute for the path as QString
 */
QString getAttributeValueQString(const QString& path, const QString& attribute, bool& ok) {
  // get the value from the extended attribute for the path
  char data[ATTR_VAL_SIZE];
#if defined(BSD)
  ssize_t bytesRetrieved = extattr_get_file(path.toLatin1().data(), EXTATTR_NAMESPACE_USER,
                                                    attribute.toLatin1().data(), data, ATTR_VAL_SIZE);
#else
  QString namespacedAttr;
  namespacedAttr.append(XATTR_NAMESPACE).append(".").append(attribute);
  ssize_t bytesRetrieved = getxattr(path.toLatin1().data(),
                                            namespacedAttr.toLatin1().data(), data, ATTR_VAL_SIZE);
#endif
  // check if we got the attribute value
  if (bytesRetrieved < 0) // If this is 0, then the value is empty but the extattr is set. If this is < 0, extattr is not set
    ok = false;
  else {
    // convert the value to QString
    data[bytesRetrieved] = 0;
    QString strValue;
    strValue = QString::fromStdString(data);
    strValue = strValue.trimmed();
    ok = true;
    return strValue;
  }
  return nullptr;
}

/*
 * set the attibute value in the extended attribute for the path as QString
 */
bool setAttributeValueQString(const QString& path, const QString& attribute, const QString& value) {
  // set the value from the extended attribute for the path

    QString candidateProgram = QStandardPaths::findExecutable("setextattr"); // FreeBSD
    if(candidateProgram.isEmpty())
            QStandardPaths::findExecutable("setxattr"); // Linux
    if(candidateProgram.isEmpty()) {
        qCritical() << "Did not find setextattr nor setxattr, cannot set extended attribute";
        return false;
    }
    QProcess p;
    p.setProgram(QStandardPaths::findExecutable(candidateProgram));
    p.setArguments({"user", attribute, value, path});
    p.start();
    p.waitForFinished();
    if(p.exitCode() != 0) {
        qCritical() << "Failed to run command:" << p.program() << p.arguments();
        return false;
    }
    return true;

//The following does not work on read-only files, e.g., at /usr
//#if defined(BSD)
//  ssize_t bytesSet = extattr_set_file(path.toLatin1().data(), EXTATTR_NAMESPACE_USER,
//                                      attribute.toLatin1().data(), value.toLatin1().data(),
//                                      value.length() + 1); // include \0 termination char
//  // check if we set the attribute value
//  return (bytesSet > 0);
//#else
//  QString namespacedAttr;
//  namespacedAttr.append(XATTR_NAMESPACE).append(".").append(attribute);
//  int success = setxattr(path.toLatin1().data(),
//                                      namespacedAttr.toLatin1().data(), value.toLatin1().data(),
//                                      value.length() + 1, 0); // include \0 termination char
//  // check if we set the attribute value
//  return (success == 0);
//#endif

}

}
