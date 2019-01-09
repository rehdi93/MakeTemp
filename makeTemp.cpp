#include <random>
#include <fstream>
#include <string>
#include <cctype>

#include "fmt/format.h"
#include "makeTemp.h"

using std::string;
using std::string_view;
using std::error_code;
namespace fs = std::filesystem;


static auto random_name(const int size) -> string
{
    static const char data[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
        ;

    auto eng = std::minstd_rand(std::random_device{}());
    auto dist = std::uniform_int_distribution<>(0, (int)std::size(data) - 2);
    
    auto fn = string(size, '*');
    for (auto& c : fn)
    {
        c = data[dist(eng)];
    }

    return fn;
}


std::pair<int, string> parse_template(string tmplt)
{
    int ret = -1;

    auto it = std::find(tmplt.begin(), tmplt.end(), '{');
    auto ite = std::find(it, tmplt.end(), '}');

    if (it == tmplt.end() || ite == tmplt.end()) { // bad template
        return { ret, tmplt };
    } else if (it + 1 == ite) { // default case '{}'
        return { 11, tmplt };
    }

    auto numIt = std::next(it);
    if (!std::all_of(numIt, ite, [](unsigned char c){ return std::isdigit(c); }))
    {
        return { ret, tmplt };
    }

    auto num = std::string(numIt, ite);
    ret = std::stoi(num);

    // remove number, leaving only '{}'
    tmplt.erase(numIt, ite);

    return { ret, tmplt };
}


std::error_category& maketemp_category()
{
    static maketemp_error_category c;
    return c;
}


fs::path temp_filename(string_view tmplt, fs::path dir, error_code& ec)
{
    ec.clear();
    
    if (!dir.is_absolute())
    {
        dir = fs::absolute(dir);
    }

    auto[charCount, normTempl] = parse_template(string(tmplt));

    if (charCount == -1) {
        ec = make_error_code(maketemp_error::invalid_template);
        return dir;
    }
    
    if (charCount < 3 || charCount > 255)
    {
        ec = make_error_code(maketemp_error::bad_template_lenght);
        return dir;
    }

    auto name = random_name(charCount);

    try
    {
        auto fn = fmt::format(normTempl, name);
        dir /= fn;
    }
    catch(const fmt::format_error&)
    {
        ec = make_error_code(maketemp_error::invalid_template); 
    }
    
    return dir;
}

error_code create_temp(const fs::path& p, bool isDir)
{
    error_code ec;

    if (fs::exists(p, ec))
    {
        return make_error_code(maketemp_error::file_dir_exists);
    }

    if (ec) return ec;

    if (!fs::exists(p.parent_path(), ec))
    {
        return make_error_code(maketemp_error::base_dir_not_found);
    }

    if (!isDir) {
        std::fstream f;
        f.open(p, std::ios::out);
        if (f.bad()) { 
            ec = make_error_code(std::errc::io_error); 
        } 
    } 
    else {
        fs::create_directory(p, ec);
    }
    
    return ec;
}
