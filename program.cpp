// makeTemp entry point


#include "makeTemp.h"
#include "fmt/ostream.h"
#include "clara.hpp"
#include "config.h"

using namespace std;
using namespace clara;
namespace fs = std::filesystem;

struct makeTempOptions
{
    bool dry_run = false, showHelp = false, createDir = false;
    string name_template = MAKETEMP_DEF_TEMPLATE;
    fs::path base_dir = fs::temp_directory_path();

    Parser* cli = nullptr;
    
} options;

void print_help()
{
    // printing 'cli' prints Clara's generated help
    fmt::print("Creates a temporary file or directory\n{}\n", *options.cli);
    fmt::print("Name template:\n{}\n\n", 
        TextFlow::Column(
        "Template for the new file/dir name, optionally it may contain a replacement field '{n#}' "
        "where 'n#' is length of the generated name. " 
        "If no numbers are specified in the brackets, 11 is assumed.\n"
        "Only ONE replacement field is allowed in the template. "
        "If nothing is specified, '" MAKETEMP_DEF_TEMPLATE "' is used.\n"
        "If no replacement field is present, the template will be the file/dir name. However if a file/dir "
        "of the same name exists in 'base dir', " MAKETEMP_EXE " will fail."
    ).indent(2));

    fmt::print("Exemple:\n{}\n{}\n\n", 
        TextFlow::Column(
            MAKETEMP_EXE " -b /my/dir {5}"
            ).indent(2),
        TextFlow::Column(
            "# exemple output: /my/dir/abcde"
            ).indent(3)
    );
    
    fmt::print("Credits:\n{}\n\n",
        TextFlow::Column(MAKETEMP_EXE " v" MAKETEMP_VERSION " by Pedro Rodrigues").indent(2)
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
    auto path = temp_filename(options.name_template, options.base_dir, ec);
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
