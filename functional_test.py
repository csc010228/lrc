# -*- coding: utf-8 -*-  
import os

#lrc编译器可执行文件名
LRC_EXE_FILE='./lrc'

#源文件后缀
SOURCE_FILE_SUFFIX='.sy'

#在目录file_dir下获取带有某一个特定后缀suffix的文件名
def files_name_with_suffix(file_dir,suffix):
    files_name=[]
    for root, dirs, files in os.walk(file_dir):
        for file in files:
            if os.path.splitext(file)[1] == suffix:
                files_name.append(os.path.join(root, file))
    return files_name

#编译某一个sy文件
def complie_file(source_file,target_file):
    return_value=os.system(LRC_EXE_FILE+' '+source_file+' '+target_file)
    if return_value!=0:
        return False
    else:
        return True

#编译目录file_dir下的所有sy文件
def complier_dir(dir_name):
    error_num=0
    files_name_with_suffix_sy=files_name_with_suffix(dir_name,SOURCE_FILE_SUFFIX)
    for file in files_name_with_suffix_sy:
        is_success=complie_file(file,os.path.splitext(file)[0])
        if is_success:
            print(file+"\t\t\t\t\t\tsuccess!")
        else:
            error_num=error_num+1
            print(file+"\t\t\t\t\t\terror!")
    return error_num


if __name__ == '__main__':
    print(str(complier_dir('../compiler2021-master/test/function/'))+" error")