// makeTemp entry point

#define CLARA_CONFIG_CONSOLE_WIDTH 90

#include "makeTemp.h"
#include "fmt/ostream.h"
#include "clara.hpp"

#define XMKTEMP_VER             "1.0.0"
#define XMKTEMP_AUTHOR          "Pedro Rodrigues"
#define XMKTEMP_DEF_TEMPLATE    "{}.tmp"

using namespace std;
using namespace clara;
namespace fs = std::filesystem;


struct makeTempOptions
{
    bool dry_run = false, showHelp = false, createDir = false;
    int name_size = 11;
    string name_template = XMKTEMP_DEF_TEMPLATE;
    fs::path base_dir = fs::temp_directory_path();

    Parser* cli = nullptr;
    
} options = {};

void print_help()
{
    // printing 'cli' prints Clara's generated help
    fmt::print("Creates a temporary file or directory\n{}\n", *options.cli);
    fmt::print("Name template:\n{}\n\n", 
        TextFlow::Column(
        "Template for the new file/dir name, optionally it may contain a replacement field ('{}') "
        "that will be replaced by 'n#' of random characters, any other characters are taken verbatim.\n"
        "Only ONE replacement field is allowed in the template. "
        "If nothing is specified, '" XMKTEMP_DEF_TEMPLATE "' is used.\n"
        "If no replacement field is present, the template will be the file/dir name. However if a file/dir "
        "of the same name exists in 'base dir', makeTemp will fail."
    ).indent(2));
    
    fmt::print("Credits:\n{}\n\n",
        TextFlow::Column("makeTemp v" XMKTEMP_VER " by " XMKTEMP_AUTHOR).indent(2)
    );
}


int main(int argc, char *argv[]) 
{
    auto cli = Help(options.showHelp)
    | Opt(options.dry_run)
        ["-u"]["--dry-run"]
        ("Don't create anything, just print the genereted name")
    | Opt(options.createDir)
        ["-d"]["--directory"]
        ("Create a directory instead of a file")
    | Opt(options.base_dir, "base dir")
        ["-b"]["--base-dir"]
        ("Base directory where the file/dir will be created, defaults to your system's "
        "TMP folder")
    | Arg(options.name_template, "name template")
    | Opt(options.name_size, "n# chars")
        ["-l"]["--rnd-lenght"]
        ("Number of random chars to write when a replacement field is found (min 3, max 255), 11 by default")
    ;

    options.cli = &cli;

    auto result = cli.parse(Args(argc, argv));
    if(!result) 
    {
        fmt::print(stderr, "Error in command line: {}\n", result.errorMessage());
        return 1;
    }

    if (options.showHelp)
    {
        print_help();
        return 0;
    }

    error_code ec;
    auto path = temp_filename(options.name_template, options.base_dir, options.name_size, ec);
    if (ec)
    {
        fmt::print(stderr, "Error: {}\n", ec.message());
        return 1;
    }

    if (!options.dry_run) 
    {
        ec = create_temp(path, options.createDir);
        if (ec)
        {
            fmt::print(stderr, "Error: {}\n", ec.message());
            return 1;
        }
    }

    // all went well, print path
    fmt::print(path.string() + '\n');

    return 0;
}
