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

bool validateTemplate(string_view templ)
{
    // validate replacement fields.
    size_t startIdx = templ.find('{');

    if (startIdx == string::npos)
    {
        // no replacement fields found
        // this is ok, the template will just be used as the filename
        return true;
    }

    size_t endIdx = templ.find('}', startIdx);
    bool match = endIdx - startIdx == 1;
    
    startIdx = templ.find('{', endIdx);

    // only one '{}' is allowed
    return match && startIdx == string::npos;
}

auto make_temp_name(string_view template_, fs::path dir, const int rndSize, error_code& ec)
{
    ec.clear();

    if (!dir.is_absolute())
    {
        dir = fs::absolute(dir);
    }

    if (!validateTemplate(template_))
    {
        ec = make_error_code(errc::invalid_argument);
        return dir;
    }

    if (rndSize < 3 || rndSize > 255)
    {
        ec = make_error_code(errc::argument_out_of_domain);
        return dir;
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
    int name_size = 11;

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
        ("Number of random chars to write (min 3, max 255), 11 by default")
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
            "Template for the new file/dir name, if it contains a replacement field ('{}'), a random sequence of characters "
            "will be replaced there, any other characters are taken verbatim. "
            "If nothing is specified, '" XMKTEMP_DEF_TEMPLATE "' is used"
        ));
        return 0;
    }

    error_code ec;
    auto path = make_temp_name(templ, baseDir, name_size, ec);
    if (ec)
    {
        fmt::print(stderr, "Error: {} ({})\n", ec.message(), ec);
        return 1;
    }

    if (!dry_run) 
    {
        ec = create_temp(path, createDir);
        if (ec)
        {
            fmt::print(stderr, "Error: {} ({})\n", ec.message(), ec);
            return 1;
        }
    }

    fmt::print(path.string());

    return 0;
}
