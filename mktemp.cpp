#include <filesystem>
#include <string>
#include <string_view>
#include <random>
#include <fstream>
#include <system_error>

#include "fmt/format.h"
#include "fmt/ostream.h"
#include "clara.hpp"

using namespace std;
using namespace clara;
namespace fs = std::filesystem;

#define XMKTEMP_VER     "1.0.0"
#define XMKTEMP_AUTHOR  "Pedro Rodrigues"
#define XMKTEMP_DEF_TEMPLATE "{}.tmp"

auto random_name(const int size) -> string
{
    static const char data[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
        ;

	auto rd = random_device{};
    auto eng = minstd_rand(rd());
    auto dist = uniform_int_distribution<>(0, sizeof data-2);
    
    auto fn = string(size, '*');
    for(auto& c : fn)
    {
        c = data[dist(eng)];
    }

    return fn;
}

auto make_temp_name(string_view template_, fs::path dir, const int rndSize = 11)
{
    if (!dir.is_absolute())
    {
        dir = fs::absolute(dir);
    }

    if (template_.find("{}") == string::npos)
    {
        // template lacks format pos. just use it as the name
        return dir / template_;
    }

	auto name = random_name(rndSize);
    auto fn = fmt::format(template_, name);

    return dir / fn;
}

error_code create_temp(const fs::path& p, bool isDir)
{
    error_code ec;

    if (fs::exists(p, ec))
    {
        return make_error_code(errc::file_exists);
    }

    if (ec) return ec;

    if (!isDir) {
        fstream f;
        f.open(p, ios::out);
        if (f.good()) { f.close(); }
        else { ec = make_error_code(errc::io_error); }
    }
    else {
        fs::create_directory(p, ec);
    }
    
    return ec;    
}

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
    uint8_t name_size = 11;

    auto cli = Help(showHelp)
    | Opt(dry_run)
        ["-u"]["--dry-run"]
        ("Don't create anything, just print the name")
    | Opt(createDir)
        ["-d"]["--directory"]
        ("Create a directory instead of a file")
    | Opt(baseDir, "path/to/dir")
        ["-b"]["--base-dir"]
        ("Base directory where the file/dir will be created, defaults to your system's "
        "TMP folder")
    | Arg(templ, "name template")
    | Opt(name_size, "n# chars")
        ["--rnd-chars"]
        ("Number of random chars to write (255 max), 11 by default")
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
            "Template for the new file/dir name, if it contains '{}', a random sequence of characters "
            "will be placed at that point, any other characters are taken verbatim. "
            "If nothing is specified, '" XMKTEMP_DEF_TEMPLATE "' is used"
        ));
        return 0;
    }

    auto path = make_temp_name(templ, baseDir, name_size);

    if (!dry_run) 
    {
        auto ec = create_temp(path, createDir);
        if (ec)
        {
            fmt::print(stderr, "Error: {} ({})\n", ec.message(), ec);
            return 1;
        }
    }

    fmt::print(path.string());

    return 0;
}
