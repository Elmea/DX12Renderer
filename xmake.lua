add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")

add_requires("glfw")
add_requires("stb")

add_packages("glfw")
add_packages("stb")

target("DX12Renderer")
    set_kind("binary")
    add_files("src/*.cpp")
    add_headerfiles("src/*.h")
    add_headerfiles("src/*.hpp")

    -- Third Party
    add_headerfiles("ThirdParty/(**.h)")
    add_headerfiles("ThirdParty/(**.hpp)")
    add_headerfiles("ThirdParty/(**.c)")
    add_files("ThirdParty/(**.cpp)")

    add_includedirs("ThirdParty", {public = true})
target_end()
