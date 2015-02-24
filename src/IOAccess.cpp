#include "ioaccess/IOAccess.h"
#include "ioaccess/StdioInterface.h"

#include "errno.h"
namespace IOAccess {

static bool isInitialized = false;
static FileSystemInterface *default_fs_interface = nullptr;
static FileSystemInterface *stdio_fs_interface = nullptr;

static FileSystemInterface *getStdioInterface()
{
	if(!stdio_fs_interface)
		stdio_fs_interface = new StdioInterface();
	
	return stdio_fs_interface;
}

void SetDefaultInterface(FileSystemInterface *fsi)
{
	if(!fsi)
		default_fs_interface = getStdioInterface();
	else
		default_fs_interface = fsi;
}

bool Initialize(FileSystemInterface *fsi)
{
	if(isInitialized)
		return true;
	
	SetDefaultInterface(fsi);
	
	if(!default_fs_interface)
		return false;
	
	isInitialized = true;
	
	return true;
}

File *OpenFile(const std::string &path, const std::string &mode)
{
	if(!isInitialized || !default_fs_interface)
		return nullptr;
	
	return default_fs_interface->openFile(path, mode);
}

Directory *OpenDirectory(const std::string &path)
{
	if(!isInitialized || !default_fs_interface)
		return nullptr;
	
	return default_fs_interface->openDir(path);
}

bool Stat(const std::string &path, StatInfo *si)
{
	if(!isInitialized || !default_fs_interface)
		return false;
	
	return default_fs_interface->stat(path, si);
}

int32_t Errno()
{
	if(!isInitialized || !default_fs_interface)
		return EINVAL;
	
	return default_fs_interface->getErrno();
}


}
