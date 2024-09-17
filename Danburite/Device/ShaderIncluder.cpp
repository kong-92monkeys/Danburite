#include "ShaderIncluder.h"
#include "../System/Env.h"

namespace Dev
{
	shaderc_include_result *ShaderIncluder::GetInclude(
        char const *const requested_source,
        shaderc_include_type const includeType,
        char const *const requesting_source,
        size_t const include_depth)
    {
        auto &assetManager{ Sys::Env::getInstance().getAssetManager() };

        auto *const pPlaceholder    { new __IncludeResultPlaceholder };
        auto &sourceName            { pPlaceholder->sourceName };
        auto &content               { pPlaceholder->content };

        auto const absolutePath
        {
            __resolveTargetAbsolutePath(
                includeType, requested_source, requesting_source)
        };

        try
        {
            sourceName  = absolutePath.string();
            content     = assetManager.readString(sourceName);
        }
        catch (...)
        {
            content = std::format("Cannot open file: {}", requested_source);
        }

        auto *const pRetVal             { new shaderc_include_result };
        pRetVal->source_name            = pPlaceholder->sourceName.c_str();
        pRetVal->source_name_length     = pPlaceholder->sourceName.size();
        pRetVal->content                = pPlaceholder->content.c_str();
        pRetVal->content_length         = pPlaceholder->content.size();
        pRetVal->user_data              = pPlaceholder;

        return pRetVal;
    }

    void ShaderIncluder::ReleaseInclude(
        shaderc_include_result *const data)
    {
        delete data->user_data;
        delete data;
    }

    std::filesystem::path ShaderIncluder::__resolveTargetAbsolutePath(
        shaderc_include_type const includeType,
        std::string_view const &targetSource,
        std::string_view const &requestingSource) noexcept
    {
        if (includeType == shaderc_include_type::shaderc_include_type_standard)
            return targetSource;

        auto const parentPath{ std::filesystem::path{ requestingSource }.parent_path() };
        return (parentPath / targetSource);
    }
}