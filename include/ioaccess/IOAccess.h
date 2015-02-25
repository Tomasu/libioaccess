#ifndef IOACCESS_H_GUARD
#define IOACCESS_H_GUARD

#include <sys/stat.h>
#include <cstdint>
#include <string>

namespace IOAccess
{
	enum FileMode
	{
		ModeUnknown      = 0,
		
		ModeSetUid       = S_ISUID,
		ModeSetGid       = S_ISGID,
		ModeSticky       = S_ISVTX,
		ModeOwnerRead    = S_IRUSR,
		ModeOwnerWrite   = S_IWUSR,
		ModeOwnerExecute = S_IXUSR,
		ModeGroupRead    = S_IRGRP,
		ModeGroupWrite   = S_IWGRP,
		ModeGroupExecute = S_IXGRP,
		ModeOtherRead    = S_IROTH,
		ModeOtherWrite   = S_IWOTH,
		ModeOtherExecute = S_IXOTH,
		
		ModeSocket  = S_IFSOCK, // socket
		ModeLink    = S_IFLNK,  // symbolic link
		ModeRegular = S_IFREG,  // regular file
		ModeBlock   = S_IFBLK,  // block device
		ModeDir     = S_IFDIR,  // directory
		ModeChar    = S_IFCHR,  // character device
		ModeFifo    = S_IFIFO   // FIFO
	};
	
	struct Timespec
	{
		int64_t sec;
		int64_t nsec;
	};
	
	struct StatInfo
	{
		uint64_t device;  /* ID of device containing file */
		uint64_t inode;   /* inode number */
		uint32_t mode;    /* protection */
		int64_t nlink;    /* number of hard links */
		
		uint64_t uid;     /* user ID of owner */
		uint64_t gid;     /* group ID of owner */
		uint64_t rdev;    /* device ID (if special file) */
		int64_t size;     /* total size, in bytes */
		
		Timespec atime;   /* time of last access */
		Timespec mtime;   /* time of last modification */
		Timespec ctime;   /* time of last status change */
		
		bool isDir() const { return mode & ModeDir; }
		bool isFile() const { return mode & ModeRegular; }
		bool isReadable() const { return mode & ModeOwnerRead || mode & ModeGroupRead || mode & ModeOtherRead; }
		bool isWritable() const { return mode & ModeOwnerWrite || mode & ModeGroupWrite || mode & ModeOtherWrite; }
		bool isExecutable() const { return mode & ModeOwnerExecute || mode & ModeGroupExecute || mode & ModeOtherExecute; }
	};
	
	struct FileInterface
	{
		virtual bool open(const std::string &path, const std::string &mode) = 0;
		
		virtual size_t read(void *ptr, size_t len) = 0;
		virtual size_t write(void *ptr, size_t len) = 0;
		
		virtual void close() = 0;
		
		virtual bool eof() = 0;
		virtual bool error() = 0;
		
		virtual int32_t getErrno() = 0;
	};
	
	class File : public FileInterface
	{ 
		public:
			virtual ~File() { }
	};
	
	struct DirectoryInterface
	{
		virtual bool open(const std::string &path) = 0;
		virtual bool read(std::string *path, bool fullpath = false) = 0;
		virtual void close() = 0;
		virtual int32_t getErrno() = 0;
	};
	
	class Directory : public DirectoryInterface 
	{ 
		public:
			virtual ~Directory() { }
	};
	
	struct FileSystemInterface
	{
		virtual File *openFile(const std::string &path, const std::string &mode) = 0;
		virtual Directory *openDir(const std::string &path) = 0;
		virtual bool stat(const std::string &path, StatInfo *si) = 0;
		virtual bool exists(const std::string &path) = 0;
		virtual int32_t getErrno() = 0;
	};

	void SetDefaultInteface(FileSystemInterface *fsi);
	
	bool Initialize(FileSystemInterface *default_fsi = nullptr);
	
	File *OpenFile(const std::string &path, const std::string &mode);
	Directory *OpenDirectory(const std::string &path);
	bool Stat(const std::string &path, StatInfo *si);
	bool Exists(const std::string &path);
	int32_t Errno();
}

#endif /* IOACCESS_H_GUARD */
