#include <random>
#include <fstream>
#include <string>

#include "fmt/format.h"
#include "makeTemp.h"

using std::string;
using std::string_view;
using std::error_code;
namespace fs = std::filesystem;

// helper methods

static auto random_name(const int size) -> string
{
    static const char data[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
        ;

    auto eng = std::minstd_rand(std::random_device{}());
    auto dist = std::uniform_int_distribution<>(0, std::size(data) - 2);
    
    auto fn = string(size, '*');
    for (auto& c : fn)
    {
        c = data[dist(eng)];
    }

    return fn;
}

// convert to uchar
// https://en.cppreference.com/w/cpp/string/byte/isdigit
static bool is_digit(unsigned char c) {
    return std::isdigit(c);
}


std::pair<int, string> parse_template(string_view tmplt)
{
    int ret = -1;
    auto t = std::string(tmplt);

    // find default case
    if (t.find("{}") != string::npos) {
        return { 11, t };
    }

    auto it = std::adjacent_find(begin(t), end(t), 
    [](const unsigned char a, const unsigned char b) {
        return a == '{' && std::isdigit(b);
    });

    auto ite = std::find(it, t.end(), '}');
    auto numIt = std::next(it);

    if (!std::all_of(numIt, ite, is_digit)) {
        return { ret, t };
    }

    auto num = std::string(numIt, ite);
    ret = std::stoi(num);

    // remove number, leaving only '{}'
    t.erase(numIt, ite);

    return { ret, t };
}


extern
makeTempErr_category& MakeTempErr_category()
{
    static makeTempErr_category c;
    return c;
}


fs::path temp_filename(string_view tmplt, fs::path dir, error_code& ec)
{
    ec.clear();
    
    if (!dir.is_absolute())
    {
        dir = fs::absolute(dir);
    }

    auto[charCount, normTempl] = parse_template(tmplt);

    if (charCount == -1) {
        ec = make_error_code(makeTempErr::invalid_template);
        return dir;
    }
    
    if (charCount < 3 || charCount > 255)
    {
        ec = make_error_code(makeTempErr::bad_template_lenght);
        return dir;
    }

    auto name = random_name(charCount);

    try
    {
        auto fn = fmt::format(normTempl, name);
        dir /= fn;
    }
    catch(const fmt::format_error& e)
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
        return ec ? ec : make_error_code(makeTempErr::base_dir_not_found);
    }

    if (!isDir) {
        std::fstream f;
        f.open(p, std::ios::out);
        if (f.bad()) { 
            ec = make_error_code(std::errc::io_error); 
        } 
        f.close(); 
    } 
    else {
        fs::create_directory(p, ec);
    }
    
    return ec;
}
