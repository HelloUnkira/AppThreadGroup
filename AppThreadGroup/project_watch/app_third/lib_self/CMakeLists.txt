# 本文件夹下引用该账户的另一仓库中的部分代码
# 子仓库导入:StudyRecord

# 收集源,生成库,指定库的链接语言,添加库依赖
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/StudyRecord/KnowledgeSet/A_Component/C_FunctionLibrary/A_Module/Calculator/Calculator.c
                  ${CMAKE_CURRENT_SOURCE_DIR}/StudyRecord/KnowledgeSet/A_Component/C_FunctionLibrary/G_MiniGame/Game2048/Game2048.c
                  ${CMAKE_CURRENT_SOURCE_DIR}/StudyRecord/KnowledgeSet/A_Component/C_FunctionLibrary/G_MiniGame/Tetris/Tetris.c
                  ${CMAKE_CURRENT_SOURCE_DIR}/StudyRecord/KnowledgeSet/A_Component/C_FunctionLibrary/G_MiniGame/Tetris/TetrisGraph.c
                  ${CMAKE_CURRENT_SOURCE_DIR}/StudyRecord/KnowledgeSet/A_Component/C_FunctionLibrary/G_MiniGame/Snake/DoubleList.c
                  ${CMAKE_CURRENT_SOURCE_DIR}/StudyRecord/KnowledgeSet/A_Component/C_FunctionLibrary/G_MiniGame/Snake/Snake.c
                  ${CMAKE_CURRENT_SOURCE_DIR}/StudyRecord/KnowledgeSet/A_Component/C_FunctionLibrary/G_MiniGame/Maze/Maze.c)
target_sources(app_third PRIVATE ${SOURCES})
