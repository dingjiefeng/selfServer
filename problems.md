

# 遇到的问题以及解决

## clion编译的过程中提示“对‘pthread_create’未定义的引用”
出现的原因是因为pthread不是Linux系统默认的库，在CMakeList.txt最后加上：

    find_package(Threads REQUIRED)
    target_link_libraries(项目名称 Threads::Threads)
## git push的时候,报错"push failed could not read from remote repository"
先是在clion里面提交不过,然后尝试在命令行里面手动提交,还是不过,提示说可能没有权限访问远程服务器;
所以我怀疑是RSA出了问题,于是删除了原有的.ssh目录下的文件,重新生成了公私钥,上传到github上,但是还是不行;我就怀疑是不是因为网络的原因;最后发现确实是虚拟机网络出了问题,很尴尬.

