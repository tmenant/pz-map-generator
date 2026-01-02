#include <cpptrace/from_current.hpp>

#include "cli/cmd_atlas_graph.h"
#include "cli/cmd_map_viewer.h"
#include "cli/cmd_tiles_browser.h"
#include "cli/command_manager.h"
#include <CLI/CLI.hpp>


int main(int argc, char **argv)
{
    CPPTRACE_TRY
    {
        CLI::App app;

        app.description("PZ Map Generator");
        app.name("pz-map-gen.exe");
        app.allow_extras(true);

        CommandManager manager;

        manager.add<CmdAtlasGraph>();
        manager.add<CmdMapViewer>();
        manager.add<CmdTilesBrowser>();

        manager.registerAll(app);

        CLI11_PARSE(app, argc, argv);
        return 0;
    }
    CPPTRACE_CATCH(const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        cpptrace::from_current_exception().print();
    }
}