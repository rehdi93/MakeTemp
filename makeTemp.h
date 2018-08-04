#pragma once

#include <system_error>
#include <filesystem>
#include <string_view>

enum class makeTempErr
{
    invalid_template = 1,
    rand_chars_out_of_range,
    file_dir_exists,
    unexpected_fail,
    baseDir_not_found
};

struct makeTempErr_category : public std::error_category
{
    inline
    const char* name() const noexcept override { return "makeTemp error"; }

    inline
    std::string message(int c) const override
    {
        switch (static_cast<makeTempErr>(c))
        {
            case makeTempErr::invalid_template:
                return "Template is not valid";
            case makeTempErr::rand_chars_out_of_range:
                return "Out of range length";
            case makeTempErr::file_dir_exists:
                return "A file / directory of that name already exists, are you using a replacement field?";
            case makeTempErr::unexpected_fail:
                return "File / directory creation failed unexpectedly";
            case makeTempErr::baseDir_not_found:
                return "Base directory does not exist";
            default:
                return "unknown";
        }
    }

    inline
    std::error_condition default_error_condition(int c) const noexcept override
    {
        switch (static_cast<makeTempErr>(c))
        {
            case makeTempErr::invalid_template:
                return std::make_error_condition(std::errc::invalid_argument);
            case makeTempErr::rand_chars_out_of_range:
                return std::make_error_condition(std::errc::argument_out_of_domain);
            case makeTempErr::file_dir_exists:
                return std::make_error_condition(std::errc::file_exists);
            case makeTempErr::unexpected_fail:
                return std::make_error_condition(std::errc::io_error);
            case makeTempErr::baseDir_not_found:
                return std::make_error_condition(std::errc::no_such_file_or_directory);
            default:
                return std::error_condition(c, *this);
        }
    }
};

extern makeTempErr_category& MakeTempErr_category();

// error_code boilerplate
namespace std
{
    template <> struct is_error_code_enum<makeTempErr> : std::true_type {};
} 

inline std::error_code make_error_code(makeTempErr e)
{
    return {static_cast<int>(e), MakeTempErr_category()};
}


auto make_temp_name(std::string_view template_, std::filesystem::path baseDir, const int rndCharLen, std::error_code& ec)
 -> std::filesystem::path;

std::error_code create_temp(const std::filesystem::path& p, bool isDir);