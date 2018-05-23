

# 遇到的问题以及解决

## clion编译的过程中提示“对‘pthread_create’未定义的引用”
出现的原因是因为pthread不是Linux系统默认的库，在CMakeList.txt最后加上：

    find_package(Threads REQUIRED)
    target_link_libraries(项目名称 Threads::Threads)
