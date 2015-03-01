#include "ioaccess/IOAccess.h"
#include "ioaccess/StdioInterface.h"
#include "ioaccess/StdioInterfacePrivate.h"

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>

namespace IOAccess
{

	File *StdioInterface::openFile(const std::string &path, const std::string &mode)
	{
		StdioFile *fh = new StdioFile();
		if(!fh->open(path, mode))
		{
			delete fh;
			return nullptr;
		}

		return fh;
	}

	Directory *StdioInterface::openDir(const std::string &path)
	{
		StdioDirectory *fh = new StdioDirectory();
		if(!fh->open(path))
		{
			delete fh;
			return nullptr;
		}

		return fh;
	}

	bool StdioInterface::stat(const std::string &path, StatInfo *si)
	{
		struct stat sbuff;
		int32_t ret = ::stat(path.c_str(), &sbuff);
		if(checkError(ret != 0))
			return false;

		si->device = sbuff.st_dev;
		si->mode = sbuff.st_mode;
		si->nlink = sbuff.st_nlink;
		si->uid = sbuff.st_uid;
		si->gid = sbuff.st_gid;
		si->rdev = sbuff.st_rdev;
		si->size = sbuff.st_size;
		#if defined(__unix)
		si->atime.sec = sbuff.st_atim.tv_sec;
		si->atime.nsec = sbuff.st_atim.tv_nsec;

		si->ctime.sec = sbuff.st_ctim.tv_sec;
		si->ctime.nsec = sbuff.st_ctim.tv_nsec;

		si->mtime.sec = sbuff.st_mtim.tv_sec;
		si->mtime.nsec = sbuff.st_mtim.tv_nsec;
		#elif defined(_WIN64) || defined(_WIN32)
		si->atime.sec = sbuff.st_atime;
		si->atime.nsec = 0;

		si->ctime.sec = sbuff.st_ctime;
		si->ctime.nsec = 0;

		si->mtime.sec = sbuff.st_mtime;
		si->mtime.nsec = 0;
		#endif

		return true;
	}

	bool StdioInterface::exists(const std::string &path)
	{
		struct stat sbuff;
		int32_t ret = ::stat(path.c_str(), &sbuff);

		// if any error occurs, the file isn't accessible, so it might as well not exist.
		if(checkError(ret != 0))
			return false;

		return true;
	}

	int32_t StdioInterface::getErrno()
	{
		return errno;
	}

	bool StdioInterface::checkError(bool err)
	{
		if(err)
		{
			errno_ = errno;
			return true;
		}

		errno_ = 0;
		return false;
	}

	StdioFile::~StdioFile()
	{
		if(fh_)
			close();

		errno_ = 0;
	}

	bool StdioFile::checkError(bool err)
	{
		if(err)
		{
			errno_ = errno;
			return true;
		}

		errno_ = 0;
		return false;
	}

	bool StdioFile::open(const std::string &path, const std::string &mode)
	{
		fh_ = fopen(path.c_str(), mode.c_str());
		if(checkError(fh_ == nullptr))
			return false;

		return true;
	}

	size_t StdioFile::read(void *ptr, size_t len)
	{
		size_t ret = fread(ptr, 1, len, fh_);
		checkError(ret != len);

		return ret;
	}

	size_t StdioFile::write(void *ptr, size_t len)
	{
		size_t ret = fwrite(ptr, 1, len, fh_);
		checkError(ret != len);

		return ret;
	}

	void StdioFile::close()
	{
		int ret = fclose(fh_);
		if(checkError(ret != 0))
			return;

		fh_ = nullptr;
		errno_ = 0;
	}

	bool StdioFile::eof()
	{
		return feof(fh_) != 0;
	}

	bool StdioFile::error()
	{
		return ferror(fh_) != 0;
	}

	int32_t StdioFile::getErrno()
	{
		return errno_;
	}

	bool StdioDirectory::checkError(bool err)
	{
		if(err)
		{
			errno_ = errno;
			return true;
		}

		errno_ = 0;
		return false;
	}

	StdioDirectory::~StdioDirectory()
	{
		if(dir_)
			close();

		errno_ = 0;
	}

	bool StdioDirectory::open(const std::string &path)
	{
		dir_ = opendir(path.c_str());
		if(checkError(dir_ == nullptr))
			return false;

		path_ = path;

		return true;
	}

	bool StdioDirectory::read(std::string *ent, bool fullpath)
	{
		std::string entname;
		std::string path;

		do {
			struct dirent *dent = readdir(dir_);
			if(checkError(dent == nullptr))
				break;

			// skip . and .. entries
			if(strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
				continue;

			entname = dent->d_name;

			printf("StdioDirectory::read(fp:%i): %s\n", fullpath, entname.c_str());

			if(fullpath)
				ent->assign(path_ + std::string("/") + entname);
			else
				ent->assign(entname);

			return true;

		} while(1);

		return false;
	}

	void StdioDirectory::close()
	{
		int32_t ret = closedir(dir_);
		checkError(ret != 0);
		dir_ = nullptr;
	}

	int32_t StdioDirectory::getErrno()
	{
		return errno_;
	}

}
