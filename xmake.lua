add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")

target("DX12Renderer")
    set_kind("binary")
    add_files("src/*.cpp")
    add_files("src/*.h")
    add_files("src/*.hpp")

