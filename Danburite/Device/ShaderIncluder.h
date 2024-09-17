#pragma once

#include <shaderc/shaderc.hpp>
#include <filesystem>

namespace Dev
{
    class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface
    {
    public:
        ShaderIncluder() = default;

        virtual shaderc_include_result *GetInclude(
            char const *requested_source,
            shaderc_include_type includeType,
            char const *requesting_source,
            size_t include_depth) override;

        virtual void ReleaseInclude(
            shaderc_include_result *data) override;

    private:
        struct __IncludeResultPlaceholder
        {
        public:
            std::string sourceName;
            std::string content;
        };

        [[nodiscard]]
        static std::filesystem::path __resolveTargetAbsolutePath(
            shaderc_include_type includeType,
            std::string_view const &targetSource,
            std::string_view const &requestingSource) noexcept;
    };
}