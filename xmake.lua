add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")

set_rundir("$(projectdir)")
set_languages("c++20")

target("CppSerializer")
    set_kind("binary")

    add_includedirs("include")
    add_files("example/**.cpp")
    add_headerfiles("include/**.h")
    add_headerfiles("include/**.inl")
target_end()