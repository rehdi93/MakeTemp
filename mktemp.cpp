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

inline auto random_name(const int size) -> string
{
    static const char data[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
        ;

    auto eng = minstd_rand(random_device{}());
    auto dist = uniform_int_distribution<short>(0, sizeof data-2);
    
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

    auto fn = fmt::format(template_, random_name(11));

    return dir / fn;
}

error_code create_temp(const fs::path& p, bool isDir)
{
    error_code ec;

    if (fs::exists(p, ec))
    {
        if (ec) return ec;
        else return make_error_code(errc::file_exists);
    }

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
        ("Base directory where the file/dir will be created, defaults to your system's"
        "TMP folder")
    | Arg(templ, "name template")
        ("Template for the new file/dir name, if it contains '{}', a random sequence of characters "
        "will be placed at that point, any other characters are taken verbatim. "
        "If nothing is specified, '" XMKTEMP_DEF_TEMPLATE "' is used")
    ;

    auto result = cli.parse(Args(argc, argv));
    if(!result) 
    {
        fmt::print(stderr, "Error in command line: {}\n", result.errorMessage());
        return 1;
    }

    if (showHelp)
    {
        fmt::print("Creates a temporary file or directory\n\n{}\nXmakeTemp. Ver{}\nMade by {}", 
                cli, XMKTEMP_VER, XMKTEMP_AUTHOR);
        
        return 0;
    }

    auto path = make_temp_name(templ, baseDir);

    if (!dry_run) 
    {
        auto ec = create_temp(path, createDir);
        if (ec)
        {
            fmt::print(stderr, "Error: {} ({})\n", ec.message(), ec);
            return 1;
        }
    }

    puts(path.string().c_str());

    return 0;
}
