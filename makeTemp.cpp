#include "makeTemp.h"
#include <random>
#include <fstream>
#include "fmt/format.h"

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

static bool validateTemplate(string_view templ)
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


extern
makeTempErr_category& MakeTempErr_category()
{
    static makeTempErr_category c;
    return c;
}

fs::path make_temp_name(string_view template_, fs::path dir, const int rndSize, error_code& ec)
{
    ec.clear();

    if (!dir.is_absolute())
    {
        dir = fs::absolute(dir);
    }

    if (!validateTemplate(template_))
    {
        ec = make_error_code(makeTempErr::invalid_template);
        return dir;
    }

    if (rndSize < 3 || rndSize > 255)
    {
        ec = make_error_code(makeTempErr::rand_chars_out_of_range);
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
        return make_error_code(makeTempErr::file_dir_exists);
    }

    if (ec) return ec;

    if (!isDir) {
        fstream f;
        f.open(p, ios::out);
        if (f.good()) { f.close(); }
        else { ec = make_error_code(makeTempErr::failed_to_create); }
    }
    else {
        fs::create_directory(p, ec);
    }
    
    return ec;
}