#include "trash.h"
#include "fileoperation.h"

void Fm::Trash::emptyTrash(){
    FmPathList* files = fm_path_list_new();
    fm_path_list_push_tail(files, fm_path_get_trash());
    Fm::FileOperation::deleteFiles(files);
    fm_path_list_unref(files);
}
