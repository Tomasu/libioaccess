#ifndef IOACCESS_STDIOINTERFACE_H_GUARD
#define IOACCESS_STDIOINTERFACE_H_GUARD

#include <string>

namespace IOAccess
{
	class File;
	class Directory;
	struct StatInfo;
	
	class StdioInterface : public FileSystemInterface
	{
		public:
			File *openFile(const std::string &path, const std::string &mode);
			Directory *openDir(const std::string &path);
			bool stat(const std::string &path, StatInfo *si);
			bool exists(const std::string &path);
			int32_t getErrno();
		
		private:
			int32_t errno_;
			
			bool checkError(bool e);
	};
}

#endif /* IOACCESS_STDIOINTERFACE_H_GUARD */
