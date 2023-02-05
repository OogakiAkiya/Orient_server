# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.13)

# プロジェクト名と使用する言語を設定
project(Orient_Server)

#Libraryパス一覧取得
file(GLOB LIBRARY_PAS_LIST RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Orient_server/src/Library/*")


#subdirectoryの追加
foreach(ITEM ${LIBRARY_PAS_LIST})
   add_subdirectory(${ITEM})
endforeach()

#ライブラリのディレクトリ名取得
file(GLOB LIBRARY_LIST RELATIVE "${CMAKE_SOURCE_DIR}/Orient_server/src/Library/" "Orient_server/src/Library/*")

#boostのパッケージ検索
set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
find_package(Boost 1.81.0 COMPONENTS system) 

if(Boost_FOUND)
   #boostインクルード
   include_directories(${Boost_INCLUDE_DIRS}) 
   
   #実行ファイルの設定追加
   add_executable(Orient_server.out ${PROJECT_SOURCE_DIR}/Orient_server/Orient_server.cpp)
   
   #ライブラリのリンク設定
   target_link_libraries(
      Orient_server.out
      ${LIBRARY_LIST}
      ${Boost_LIBRARIES}
   )
endif()
