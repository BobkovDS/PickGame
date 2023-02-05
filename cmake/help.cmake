function(make_sources _out_list)
    message(LOG " Using make_sources")
	# Добавим исходники
    set(src_list)
    file(GLOB_RECURSE src_list RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp")
    # stdafx.cpp идёт первым, чтобы cotire смог с его помощью PCH скомпилить
    #list(INSERT src_list 0 stdafx.cpp)
    list(REMOVE_DUPLICATES src_list)

    # Организуем их в дерево
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${src_list})

    # Добавим заголовочники
    set(hdr_list)
    file(GLOB_RECURSE hdr_list RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h" "*.hpp")
    list(REMOVE_DUPLICATES hdr_list)

    # Их тоже организуем в дерево
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Header Files" FILES ${hdr_list})

    # Объединим списки
    list(APPEND src_list ${hdr_list})

    set(${_out_list} "${src_list}" PARENT_SCOPE)
endfunction()