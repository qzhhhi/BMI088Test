---@diagnostic disable: undefined-global
function main (target)
    import("core.project.config")
    import("core.project.depend")
    import("core.project.project")
    import("core.base.task")
    import("core.tool.compiler")
    -- print(target)

    -- local abc = compiler:load("cxx", target)
    -- print(abc)

    local dependfile = path.join(config.buildir(), ".gens", "rules", "plugin.compile_commands.autoupdate") .. ".d"
    depend.on_changed(function ()
        target:add("cxxflags", "-isystem/usr/share/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/bin/../lib/gcc/arm-none-eabi/13.2.1/../../../../arm-none-eabi/include/c++/13.2.1")
        target:add("cxxflags", "-isystem/usr/share/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/bin/../lib/gcc/arm-none-eabi/13.2.1/../../../../arm-none-eabi/include/c++/13.2.1/arm-none-eabi")
        target:add("cxxflags", "-isystem/usr/share/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/bin/../lib/gcc/arm-none-eabi/13.2.1/../../../../arm-none-eabi/include/c++/13.2.1/backward")
        target:add("cxflags", "-isystem/usr/share/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/bin/../lib/gcc/arm-none-eabi/13.2.1/include")
        target:add("cxflags", "-isystem/usr/share/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/bin/../lib/gcc/arm-none-eabi/13.2.1/include-fixed")
        target:add("cxflags", "-isystem/usr/share/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/bin/../lib/gcc/arm-none-eabi/13.2.1/../../../../arm-none-eabi/include")

        local filename = "compile_commands.json"
        local filepath = outputdir and path.join(outputdir, filename) or filename
        task.run("project", {kind = "compile_commands", outputdir = ".", lsp = "clangd"})
        print("compile_commands.json updated!")
    end, {dependfile = dependfile,
            files = project.allfiles(),
            values = target:sourcefiles()})
end