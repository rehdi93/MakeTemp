// makeTemp entry point

#include "makeTemp.h"
#include "fmt/ostream.h"
#include "clara.hpp"

#define XMKTEMP_VER             "1.0.0"
#define XMKTEMP_AUTHOR          "Pedro Rodrigues"
#define XMKTEMP_DEF_TEMPLATE    "{}.tmp"

using namespace std;
using namespace clara;
namespace fs = std::filesystem;


auto help_column(const string& text)
{
    auto w = CLARA_CONFIG_CONSOLE_WIDTH;
    return TextFlow::Column(text).width(w).indent(2);
}


int main(int argc, char *argv[]) 
{
    bool dry_run = false, showHelp = false, createDir = false;
    string templ = XMKTEMP_DEF_TEMPLATE;
    fs::path baseDir = fs::temp_directory_path();
    int name_size = 11;

    auto cli = Help(showHelp)
    | Opt(dry_run)
        ["-u"]["--dry-run"]
        ("Don't create anything, just print the name")
    | Opt(createDir)
        ["-d"]["--directory"]
        ("Create a directory instead of a file")
    | Opt(baseDir, "base dir")
        ["-b"]["--base-dir"]
        ("Base directory where the file/dir will be created, defaults to your system's "
        "TMP folder")
    | Arg(templ, "name template")
    | Opt(name_size, "n# chars")
        ["-l"]["--rnd-lenght"]
        ("Number of random chars to write when a replacement field is found (min 3, max 255), 11 by default")
    ;

    auto result = cli.parse(Args(argc, argv));
    if(!result) 
    {
        fmt::print(stderr, "Error in command line: {}\n", result.errorMessage());
        return 1;
    }

    if (showHelp)
    {
        fmt::print("Creates a temporary file or directory\n{}\n\n", cli);
        fmt::print("Name template:\n{}\n\n", 
            help_column(
            "Template for the new file/dir name, optionally it may contain a replacement field ('{}') "
            "that will be replaced by '-l num#' of random characters, any other characters are taken verbatim.\n"
            "Only ONE replacement field is allowed in the template. "
            "If nothing is specified, '" XMKTEMP_DEF_TEMPLATE "' is used.\n"
            "If no replacement field is present, the template will be the file/dir name. However if a file/dir "
            "of the same name exists in 'base dir', makeTemp will fail w/o touching the file system."
        ));
        return 0;
    }

    error_code ec;
    auto path = make_temp_name(templ, baseDir, name_size, ec);
    if (ec)
    {
        fmt::print(stderr, "Error: {}\n", ec.message());
        return 1;
    }

    if (!dry_run) 
    {
        ec = create_temp(path, createDir);
        if (ec)
        {
            fmt::print(stderr, "Error: {}\n", ec.message());
            return 1;
        }
    }

    fmt::print(path.string());

    return 0;
}
