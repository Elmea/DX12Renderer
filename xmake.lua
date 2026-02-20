add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")

set_languages("cxx20")

add_requires("glfw")
add_requires("stb")

add_packages("glfw")
add_packages("stb")

add_links("d3d12")
add_links("dxgi")
add_links("dxguid")
add_links("d3dcompiler")

target("DX12Renderer")
    set_kind("binary")
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_headerfiles("src/**.hpp")
    add_includedirs("src")

    add_headerfiles("ThirdParty/ElmeasMaths/RedfoxMaths.hpp")

    -- Third Party
    add_headerfiles("ThirdParty/(**.h)")
    add_headerfiles("ThirdParty/(**.hpp)")
    add_files("ThirdParty/(**.c)")
    add_files("ThirdParty/(**.cpp)")

    add_includedirs("ThirdParty", {public = true})
target_end()
