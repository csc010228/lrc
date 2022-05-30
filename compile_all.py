# -*- coding: utf-8 -*-  
import os

#测试文件夹
TEST_DIR='../compiler2022-master/test/function'

#lrc编译器可执行文件名
LRC_EXE_FILE='./lrc'

#源文件后缀
SOURCE_FILE_SUFFIX='.sy'

#需要删除的文件后缀
DELETE_FILE_SUFFIX='.pre'

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
    return_value=os.system(LRC_EXE_FILE+' -S -o '+target_file+'.s '+source_file)
    if return_value!=0:
        return False
    else:
        return True

#编译目录file_dir下的所有sy文件
def complier_dir(dir_name):
    error_num=0
    success_num=0
    files_name_with_suffix_sy=files_name_with_suffix(dir_name,SOURCE_FILE_SUFFIX)
    for file in files_name_with_suffix_sy:
        is_success=complie_file(file,os.path.splitext(file)[0])
        if is_success:
            print(file+"\t\t\t\t\t\tsuccess!")
            success_num=success_num+1
        else:
            error_num=error_num+1
            print(file+"\t\t\t\t\t\terror!")
    return success_num,error_num

#删除某一个目录下的所有特定后缀的文件
def delete_specific_suffix_file(dir_name,suffix):
    files_name_with_suffix_sy=files_name_with_suffix(dir_name,suffix)
    for file in files_name_with_suffix_sy:
        os.remove(file)


if __name__ == '__main__':
    success_num,error_num=complier_dir(TEST_DIR)
    print(str(success_num)+" success,"+str(error_num)+" error")
    delete_specific_suffix_file(TEST_DIR,DELETE_FILE_SUFFIX)