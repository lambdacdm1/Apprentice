-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibsse-ng")
add_requires("nlohmann_json")

-- set project
set_project("Apprentice")
set_version("0.1.0")
set_license("GPL-3.0")

-- set defaults
set_languages("c++23")
set_warnings("allextra")

-- set policies
set_policy("package.requires_lock", true)

-- add rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- targets
target("Apprentice")
    -- add dependencies to target
    add_deps("commonlibsse-ng")
    add_packages("nlohmann_json")

    -- add commonlibsse-ng plugin
    add_rules("commonlibsse-ng.plugin", {
        name = "apprentice",
        author = "lambdacdm",
        description = "SKSE plugin for Apprentice - A Trait Overhaul"
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("include/**.h")
    add_includedirs("include")
    set_pcxxheader("include/pch.h")
