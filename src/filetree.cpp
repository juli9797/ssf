//FILETREE

#include <iostream>
#include <filesystem>



#include "filetree.h"

/* 
 * This class is supposed to handle all the filesystem interactions using std::filesystem
 */ 
class filetree{
	public:
		filetree(){
			std::cout << "init path: " << std::filesystem::current_path() << std::endl;
			current_path = ""; //initialize current path
			current_path = std::filesystem::absolute(current_path);

			
		}
		~filetree(){
			std::cout << "filetree DESTROYED!!!11elf" << std::endl;
		}
		/*
		 * This function obtains the directory contents relative to the working directory
		 */
		auto get_folder_contents(int layer = 0){
			std::cout << "get folder Path: "<< std::endl;
			std::filesystem::path path = current_path;
			for(int i = 0; i <= layer; i++){
				std::cout << path <<std::endl;
				if(path.has_parent_path()){
					path = path.parent_path();
				}
				else{
					//idk what to do here
				}		
			}

			std::filesystem::directory_iterator di = std::filesystem::directory_iterator(path);
			//[DEBUG] list file names
			for(auto& p: di){
				std::cout << p << std::endl;
			}  
			return di;  
		}
		auto move_layer_up(){
			if(current_path.has_parent_path()){
				std::cout << "current Path :"<< current_path << std::endl;
				current_path = current_path.parent_path();
				std::cout << "parent Path : "<< current_path <<  std::endl;
			}
			
		}
		auto move_to_dir(std::filesystem::path dir){
			
		}
		
	private:
		std::filesystem::path current_path;


};
/* //THIS SHIT IS NOT NECESSARY AT ALL
class Filesystem_element{
	public:
		Filesystem_element(){
		
		}
		~Filesystem_element(){
		
		}
		auto get_Path(){
			return Path;
		}
		auto get_file_Type(){
			return file_type;
		}
		auto get_perms(){
			return perms;
		}
	private:
		std::filesystem::path path;
		std::filesystem::file_type file_type;
		std::filesystem::perms perms;
		

};i*/
int main () {
	std::cout<< "Hello World" <<std::endl;
	filetree ft;
	//ft.get_folder_contents();
	//ft.move_layer_up();
	//ft.get_folder_contents(0);
	//ft.get_folder_contents(1);
	//ft.get_folder_contents(2);
	//ft.get_folder_contents(3);
	ft.get_folder_contents(4);
	
	
	
}

