//
// 
//
//
#include <vector>
#include<filesystem>

#include"log.hpp"

namespace ssf{
namespace filesys{

using entry_vector_t = std::vector<std::filesystem::directory_entry>;

class Navigator{
	public:
		void move_up(){
			//update selections
			//update _fs_snapshot
		}
		void move_down(){
			//update selections
			//update _fs_snapshot
		}
		void move_left(){
			//update selections
			//update _fs_snapshot
		}
		void move_right(){
		
		}
		void get_left(){
		}
		void get_right(){
		}
		void get_active(){
		}
	private:
		FilesystemSnapshot _fs_snapshot;
};
struct emptySelection: public std::exception
{
	const char * what () const throw ()
    {
    	return "selection is empty!";
    }
}
class Selection{
	public:
		Selection();
		bool valid(){
			return _entries.empty();	
		}
		auto get_path(){
			if(valid()){
				return _entries.at(0);
			}else{
				throw emptySelection();
			}
		}
		auto update(std::filesystem::directory_entry de){
			_entries.resize(0);
			_entries.emplace_back(de);
		}

	private:
		entry_vector_t _entries;
};

class FilesystemSnapshot{
	public:
		FilesystemSnapshot(){};
		auto get_left(){
			return _left;
		}

		auto get_active(){
			return _active;
		}

		auto get_right(){
			return _right;
		}
		void update(Selection s){
			_s = s;
		}
		void snapshot(){
			//update active
			try{
				_active.clear();
				auto di = std::filesystem::directory_iterator(s.get_path());
				for( auto e : di){
					_active.push_back(e);	
				}

			}
			catch (emptySelection& e){
				//Something went wrong :(
				log << e.what() << "\n";  	
			}
			catch (std::filesystem:filesystem_error){
				//catch filesystem exceptions as well
				log << e.what() << "\n";  	
			}
			//update left
						
			//update right


		}
		template<typename T>
		void sort(T comp){
		}

	private:
		entry_vector_t _left = {}
		entry_vector_t _active =  {};
		entry_vector_t _right = {};
		Selection _s;

};

}
}
