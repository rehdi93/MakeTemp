#include <random>
#include <fstream>
#include <string>

#include "fmt/format.h"
#include "makeTemp.h"

using namespace std;
namespace fs = std::filesystem;

// helper methods

static auto random_name(const int size) -> string
{
    static const char data[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
        ;

    auto eng = minstd_rand(random_device{}());
    auto dist = uniform_int_distribution<>(0, std::size(data) - 2);
    
    auto fn = string(size, '*');
    for (auto& c : fn)
    {
        c = data[dist(eng)];
    }

    return fn;
}

extern
makeTempErr_category& MakeTempErr_category()
{
    static makeTempErr_category c;
    return c;
}

fs::path temp_filename(string_view template_, fs::path dir, const int rndSize, error_code& ec)
{
    ec.clear();

    if (!dir.is_absolute())
    {
        dir = fs::absolute(dir);
    }

    if (rndSize < 3 || rndSize > 255)
    {
        ec = make_error_code(makeTempErr::rand_chars_out_of_range);
        return dir;
    }

	auto name = random_name(rndSize);
    
    try
    {
        auto fn = fmt::format(template_, name);
        dir /= fn;
    }
    catch(const fmt::format_error& )
    {
        ec = make_error_code(makeTempErr::invalid_template); 
    }
    
    return dir;
}

error_code create_temp(const fs::path& p, bool isDir)
{
    error_code ec;

    if (fs::exists(p, ec))
    {
        return make_error_code(makeTempErr::file_dir_exists);
    }

    if (ec) return ec;

    if (!fs::exists(p.parent_path(), ec))
    {
        return ec ? ec : make_error_code(makeTempErr::baseDir_not_found);
    }

    if (!isDir) {
        fstream f;
        f.open(p, ios::out);
        if (f.bad()) { 
            ec = make_error_code(errc::io_error); 
        } 
        f.close(); 
    } 
    else {
        fs::create_directory(p, ec);
    }
    
    return ec;
}
