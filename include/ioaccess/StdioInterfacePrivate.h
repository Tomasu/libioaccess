#ifndef IOACCESS_STDIOINTERFACE_PRIVATE_H_GUARD
#define IOACCESS_STDIOINTERFACE_PRIVATE_H_GUARD

#include <cstdio>

#include <dirent.h>

namespace IOAccess
{
	class StdioFile : public File
	{
		public:
			StdioFile() : fh_(nullptr), errno_(0) { };
			virtual ~StdioFile();
			
			bool open(const std::string &path, const std::string &mode);
			size_t read(void *ptr, size_t len);
			size_t write(void *ptr, size_t len);
			
			void close();
			
			bool eof();
			bool error();
			int32_t getErrno();
		
		private:
			FILE *fh_;
			int32_t errno_;
			
			bool checkError(bool e);
	};
	
	class StdioDirectory : public Directory
	{
		public:
			StdioDirectory() : dir_(nullptr), errno_(0) {}
			virtual ~StdioDirectory();
			
			bool open(const std::string &path);
			std::string read();
			void close();
			int32_t getErrno();
			
		private:
			DIR *dir_;
			int32_t errno_;
			
			bool checkError(bool e);
	};
}

#endif /* IOACCESS_STDIOINTERFACE_PRIVATE_H_GUARD */