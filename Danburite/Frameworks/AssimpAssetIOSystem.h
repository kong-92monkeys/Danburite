#pragma once

#include <assimp/IOSystem.hpp>
#include <filesystem>
#include <stack>

namespace Frx
{
    class AssimpAssetIOSystem : public Assimp::IOSystem
    {
    public:
        AssimpAssetIOSystem() = default;
        virtual ~AssimpAssetIOSystem() noexcept override = default;

        [[nodiscard]]
        virtual bool Exists(
            char const *pFile) const override;

        [[nodiscard]]
        virtual char getOsSeparator() const override;

        [[nodiscard]]
        virtual Assimp::IOStream *Open(
            char const *pFile,
            char const *pMode) override;

        virtual void Close(
            Assimp::IOStream *pFile) override;

        virtual bool PushDirectory(
            std::string const &path) override;

        [[nodiscard]]
        virtual std::string const &CurrentDirectory() const override;

        [[nodiscard]]
        virtual size_t StackSize() const override;

        virtual bool PopDirectory() override;

        virtual bool CreateDirectory(
            std::string const &path) override;

        virtual bool ChangeDirectory(
            std::string const &path) override;

        virtual bool DeleteFile(
            std::string const &file) override;

    private:
        std::string __pwd;
        std::stack<std::string> __pathStack;
    };
}