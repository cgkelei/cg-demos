#include <IO/FileSystem.h>
#include <IO/FileStream.h>
#include <IO/PathUtil.h>
#include <Core/Exception.h>
#include <sys/stat.h>
#include <direct.h>

namespace RcEngine {

SINGLETON_DECL(FileSystem)

FileSystem::FileSystem()
{

}

FileSystem::~FileSystem()
{

}

bool FileSystem::FileExits( const String& fileName )
{
	if (!CheckAccess(PathUtil::GetPath(fileName)))
		return false;

	String fixedName = PathUtil::GetInternalPath(fileName);

	struct stat st;
	if (stat(fixedName.c_str(), &st) || st.st_mode & S_IFDIR)
		return false;

	return true;
}

bool FileSystem::DirExits( const String& pathName )
{
	if (!CheckAccess(PathUtil::GetPath(pathName)))
		return false;

	String fixedName = PathUtil::RemoveTrailingSlash(pathName);

	struct stat st;
	if (stat(fixedName.c_str(), &st) || !(st.st_mode & S_IFDIR))
		return false;

	return true;
}


void FileSystem::RegisterPath( const String& pathName, const String& group )
{
	if (pathName.empty())
	{
		return;
	}

	String fixedPath = PathUtil::RemoveTrailingSlash(pathName);

	mResouceGroups[group].push_back(fixedPath);
}


bool FileSystem::CheckAccess( const String& pathName )
{
	if (mAllowedPaths.empty())
	{
		return true;
	}

	String fixedPath = PathUtil::AddTrailingSlash(pathName);

	// If there is any attempt to go to a parent directory, disallow
	//if (fixedPath.find_first_of("..") != String::npos)
	//{
	//	return false;
	//}

	// Check if the path is a partial match of any of the allowed directories

	/*for (auto iter = mAllowedPaths.begin(); iter != mAllowedPaths.end(); ++iter)
	{
		if (fixedPath.find(*iter) == 0) 
			return true;
	}*/


	// Not found, so disallow
	return true;
}

String FileSystem::GetCurrentDir() const
{
	char path[MAX_PATH];
	path[0] = 0;
	getcwd(path, MAX_PATH);
	return PathUtil::AddTrailingSlash(String(path));
}

bool FileSystem::SetCurrentDir( const String& pathName )
{
	if (!CheckAccess(pathName))
	{
		return false;
	}

	if (chdir(PathUtil::RemoveTrailingSlash(pathName).c_str()) != 0)
	{
		return false;
	}

	return true;
}

void FileSystem::ScanDirInternal( vector<String>& result, String path, const String& startPath, const String& filter, unsigned flags, bool recursive )
{

}

String FileSystem::Locate( const String& file, const String& group )
{
	auto groupIter = mResouceGroups.find(group);
	if (groupIter == mResouceGroups.end())
	{
		std::cout << "Group: " << group << " doesn't exits" << std::endl;
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, "Group: " + group + " doesn't exits", "FileSystem::Locate");
	}

	vector<String>& paths = groupIter->second;
	for (auto iter = paths.begin(); iter != paths.end(); ++iter)
	{
		String fileName = PathUtil::GetFileNameAndExtension(file);
		String fullPath = *iter + "/" + fileName;

		if (FileExits(fullPath))
		{
			return fullPath;
		}
	}
	// Log
	std::cout << "File: " << file << " doesn't exit!" << std::endl;
	ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, "File: " + file + " doesn't exit!", "FileSystem::Locate");
}

shared_ptr<Stream> FileSystem::OpenStream( const String& file, const String& group/*="General"*/ )
{
	auto groupIter = mResouceGroups.find(group);
	if (groupIter == mResouceGroups.end())
	{
		std::cout << "Group: " << group << " doesn't exits" << std::endl;
		ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, "Group: " + group + " doesn't exits", "FileSystem::OpenStream");
	}

	vector<String>& paths = groupIter->second;
	for (auto iter = paths.begin(); iter != paths.end(); ++iter)
	{
		String fileName = PathUtil::GetFileNameAndExtension(file);
		String fullPath = *iter + "/" + fileName;

		if (FileExits(fullPath))
		{
			shared_ptr<FileStream> stream ( new FileStream );
			stream->Open(fullPath);
			return stream;
		}
	}


	ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, "File: " + file + "in " + group + " doesn't exits", "FileSystem::OpenStream");
}

bool FileSystem::Exits( const String& name, const String& group/*="General"*/ )
{
	auto groupIter = mResouceGroups.find(group);
	if (groupIter == mResouceGroups.end())
	{
		return false;
	}

	vector<String>& paths = groupIter->second;
	for (auto iter = paths.begin(); iter != paths.end(); ++iter)
	{
		String fileName = PathUtil::GetFileNameAndExtension(name);
		String fullPath = *iter + "/" + fileName;

		if (FileExits(fullPath))
		{
			return true;
		}
	}

	return false;
}


} //Namespace RcEngine