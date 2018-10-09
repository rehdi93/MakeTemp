#pragma once

#include <system_error>
#include <filesystem>
#include <string_view>

enum class makeTempErr
{
    invalid_template = 1,
    bad_template_lenght,
    file_dir_exists,
    base_dir_not_found,
    unexpected = 99
};

struct makeTempErr_category : public std::error_category
{
    const char* name() const noexcept override { return "makeTemp error"; }

    std::string message(int c) const override
    {
        switch (static_cast<makeTempErr>(c))
        {
            case makeTempErr::invalid_template:
                return "Template is not valid";
            case makeTempErr::bad_template_lenght:
                return "Invalid template lenght";
            case makeTempErr::file_dir_exists:
                return "A file / directory of that name already exists";
            case makeTempErr::base_dir_not_found:
                return "Base directory does not exist";
            case makeTempErr::unexpected:
            default:
                return "unexpected error";
        }
    }

    std::error_condition default_error_condition(int c) const noexcept override
    {
        switch (static_cast<makeTempErr>(c))
        {
            case makeTempErr::invalid_template:
                return std::make_error_condition(std::errc::invalid_argument);
            case makeTempErr::bad_template_lenght:
                return std::make_error_condition(std::errc::argument_out_of_domain);
            case makeTempErr::file_dir_exists:
                return std::make_error_condition(std::errc::file_exists);
            case makeTempErr::base_dir_not_found:
                return std::make_error_condition(std::errc::no_such_file_or_directory);
            default:
                return std::error_condition(c, *this);
        }
    }
};

extern makeTempErr_category& MakeTempErr_category();

namespace std {
    template <> struct is_error_code_enum<makeTempErr> : true_type {};
} 

inline std::error_code make_error_code(makeTempErr e)
{
    return {static_cast<int>(e), MakeTempErr_category()};
}


auto temp_filename(std::string_view template_, std::filesystem::path baseDir, std::error_code& ec)
 -> std::filesystem::path;

std::error_code create_temp(const std::filesystem::path& p, bool isDir);