import os
import filecmp
import hashlib
import sys

def compare_directories(dir1, dir2):
    """
    递归比较两个目录下的所有文件和目录
    """
    dcmp = filecmp.dircmp(dir1, dir2)
    
    # 比较文件
    for name in dcmp.common_files:
        path1 = os.path.join(dir1, name)
        path2 = os.path.join(dir2, name)
        if not compare_files(path1, path2):
            print(f"文件不一致: {path1} 和 {path2}")
    
    # 比较子目录
    for name in dcmp.common_dirs:
        path1 = os.path.join(dir1, name)
        path2 = os.path.join(dir2, name)
        compare_directories(path1, path2)
    
    # 打印只在左边目录存在的文件或目录
    for name in dcmp.left_only:
        print(f"只在 {dir1} 中存在: {name}")
    
    # 打印只在右边目录存在的文件或目录
    for name in dcmp.right_only:
        print(f"只在 {dir2} 中存在: {name}")

def compare_files(file1, file2):
    """
    比较两个文件是否一致
    """
    size1 = os.path.getsize(file1)
    size2 = os.path.getsize(file2)
    
    if size1 != size2:
        return False
    
    if size1 > 5 * 1024 * 1024:  # 大于5M
        return True
    
    # 小于5M，比较内容
    return get_file_hash(file1) == get_file_hash(file2)

def get_file_hash(file_path):
    """
    计算文件的MD5哈希值
    """
    hash_md5 = hashlib.md5()
    with open(file_path, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("使用方法: python script.py <目录1> <目录2>")
        sys.exit(1)
    
    dir1 = sys.argv[1]
    dir2 = sys.argv[2]
    
    if not os.path.isdir(dir1) or not os.path.isdir(dir2):
        print("错误: 请提供有效的目录路径")
        sys.exit(1)
    
    print(f"比较目录 '{dir1}' 和 '{dir2}':")
    compare_directories(dir1, dir2)