#include "AssimpAssetIOSystem.h"
#include "AssimpAssetBinaryStream.h"
#include "AssimpAssetStringStream.h"
#include "../System/Env.h"

#ifdef CreateDirectory
#undef CreateDirectory
#endif

#ifdef DeleteFile
#undef DeleteFile
#endif

namespace Frx
{
    bool AssimpAssetIOSystem::Exists(
        char const *const pFile) const
    {
        auto &assetManager{ Sys::Env::getInstance().getAssetManager() };
        return assetManager.exists(pFile);
    }

    char AssimpAssetIOSystem::getOsSeparator() const
    {
        return '\\';
    }

    Assimp::IOStream *AssimpAssetIOSystem::Open(
        char const *const pFile,
        char const *const pMode)
    {
        if (std::strcmp(pMode, "rb") == 0)
            return new AssimpAssetBinaryStream{ pFile };

        if ((std::strcmp(pMode, "r") == 0) || (std::strcmp(pMode, "rt") == 0))
            return new AssimpAssetStringStream{ pFile };

        return nullptr;
    }

    void AssimpAssetIOSystem::Close(
        Assimp::IOStream *const pFile)
    {
        delete pFile;
    }

    bool AssimpAssetIOSystem::PushDirectory(
        std::string const &path)
    {
        if (path.empty())
            return false;

        __pwd = path;
        __pathStack.push(path);

        return true;
    }

    std::string const &AssimpAssetIOSystem::CurrentDirectory() const
    {
        return __pwd;
    }

    size_t AssimpAssetIOSystem::StackSize() const
    {
        return __pathStack.size();
    }

    bool AssimpAssetIOSystem::PopDirectory()
    {
        if (__pathStack.empty())
            return false;

        __pathStack.pop();

        if (__pathStack.empty())
            __pwd = "";
        else
            __pwd = __pathStack.top();

        return true;
    }

    bool AssimpAssetIOSystem::CreateDirectory(
        std::string const &path)
    {
        return false;
    }

    bool AssimpAssetIOSystem::ChangeDirectory(
        std::string const &path)
    {
        __pwd = path;
        __pathStack.top() = path;

        return true;
    }

    bool AssimpAssetIOSystem::DeleteFile(
        std::string const &file)
    {
        return false;
    }
}