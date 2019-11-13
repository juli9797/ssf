//
// 
//
//
#include <vector>
#include<filesystem>

namespace ssf{
namespace filesys{

using entry_vector_t = std::vector<std::filesystem::directory_entry>;

class Navigator{
	private:
		Selection _selection;
		FilesystemSnapshot _fs_snapshot;
		
};

class Selection{
	private:
		entry_vector_t _entries;
};

class FilesystemSnapshot{
	private:
		entry_vector_t left = {};
		entry_vector_t mid =  {};
		entry_vector_t right = {};

};

}
}
