namespace ssf
{
// I had a dream:
// Each command is composed of several CommandObjects.
// For example: "vim test.txt":
// vim - VimCommand
// test.txt - file
// Both should autocomplete or list options when the user hits TAB
// Command Objects can (or should) be daisy-chained together: file is child object of vimCommand  
class CommandObject
{
public:
	
	//Each CommandObject implements:
	//set_text THIS IS THE COMMAND TEXT (COMPLETE OR NOT)	
	//get_autocompletion()
	//is_valid()
	//callback_t(vector)
	//evaluate()	

private:
	//List of child CommandObjects	




}



}
