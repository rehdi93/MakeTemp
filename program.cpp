// makeTemp entry point


#include "makeTemp.h"
#include "fmt/ostream.h"
#include "clara.hpp"
#include "config.h"

using namespace clara;
namespace fs = std::filesystem;

struct makeTempOptions
{
    bool dry_run = false, showHelp = false, createDir = false;
    std::string name_template = MAKETEMP_DEF_TEMPLATE;
    fs::path base_dir = fs::temp_directory_path();

    Parser* cli = nullptr;
    
} options;

void print_help()
{
    using clara::TextFlow::Column;
    
    // printing 'cli' prints Clara's generated help
    fmt::print("Creates a temporary file or directory and print its name.\n{}\n", *options.cli);
    fmt::print("Name template:\n{}\n\n", 
        Column(
        "A template must contain EXACTLY ONE replacement field '{n#}' or '{}', which will be replaced by a randomly generated string, "
        "where 'n#' is the length of the string, or 11 if no number is specified.\n"
        "If no template is provided, '" MAKETEMP_DEF_TEMPLATE "' is used."
        ).indent(2)
    );

    fmt::print("Exemple:\n{}\n{}\n\n", 
        Column(
            MAKETEMP_EXE " -b /my/dir {5}"
        ).indent(2),
        Column(
            "# exemple output: /my/dir/abcde"
        ).indent(2)
    );
    
    fmt::print("Credits:\n{}\n\n",
        Column(MAKETEMP_EXE " v" MAKETEMP_VERSION " by Pedro Rodrigues").indent(2)
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
        ("Base directory where the file/dir will be created, defaults to your system's TMP folder")
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

    std::error_code ec;
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
