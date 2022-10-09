# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.16)

# プロジェクト名と使用する言語を設定
project(Orient_Server)

#subdirectoryの追加
add_subdirectory(Orient_server/src/Library/OpenSocket)
add_subdirectory(Orient_server/src/Library/FileController)
add_subdirectory(Orient_server/src/Library/Timestamp)

#実行ファイルの追加
add_executable(Orient_server.out ${PROJECT_SOURCE_DIR}/Orient_server/Orient_server.cpp)

#ライブラリのリンク
target_link_libraries(
    Orient_server.out 
    OpenSocket 
    FileController
    Timestamp
)
