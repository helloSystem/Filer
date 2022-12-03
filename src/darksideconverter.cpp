#include "darksideconverter.h"

QStringList DarkSideConverter::qStringListFromFmPathList(FmPathList* srcFiles) {
  QStringList paths;
  for (GList* l = fm_path_list_peek_head_link(srcFiles); l; l = l->next) {
    FmPath* path = FM_PATH(l->data);
    QString sourcePathStr = QString(fm_path_to_str(path));
    paths.append(sourcePathStr);
  }
  return paths;
}
