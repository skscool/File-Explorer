
//**********************************************************************
// All Header file included/imported here
//**********************************************************************
#include<bits/stdc++.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include <grp.h>


using namespace std;

//**********************************************************************
// Declaring extern global variables
//**********************************************************************
char *root;
char* curPath;
int searchflag=0;
vector< string > dirList;
stack <string> back_stack;
stack <string> forw_stack;
unsigned int rowsize,colsize;
unsigned int totalFiles;
int wintrack;
int isDIR(char *);

//**********************************************************************
// Declaring global method											   *
//**********************************************************************
void gotoDir(const char *);
void printDIR(const char *, const char *);
int getScreenCapacity();
void navigate();


int startCommandMode();
void clearCommand();
string pathProcessing(string str);
void createNewFiles(vector<string> list);
void createSingleFile(char *path);
void makeDirectories(vector<string> list);
void removeDirectories(vector<string> list);
void removeSingleDirectory(char *path);
void removeFiles(vector<string> list);
void removeSingleFile(char *path);
void renameFiles(vector<string> list);
void copycommand(vector<string> list);
void copydirectory(char *path,char *des);
void copyfile(char *path,char *des);
void movecommand(vector<string> list);
int searchcommand(vector<string> list);
void searchanything(char *path,string filename);
void printERROR(string str);


int countTotalFiles(const char *);
///////////////////////////////////////////////////////MAIN//////////////////////////////////////////////////////////////


//**********************************************************************
// Main Method
//**********************************************************************
int main(int argc, char *argv[])
{
	unsigned int printingLimit;
	char c[] = {'.', '\0'};
	root = c;
	
	totalFiles = countTotalFiles(root);
	printingLimit = getScreenCapacity();
	sort(dirList.begin(), dirList.end());
	wintrack = 0;
	printf("\033[H\033[J");
	printf("%c[%d;%dH", 27, 1, 1);
	
	unsigned int i, j=1;
	for (i = 0; i < totalFiles && j <= printingLimit; i++)
	{
		char currFile[dirList[i].length() + 1];
		strcpy(currFile, dirList[i].c_str());
		printDIR(currFile, root);
		j++;
	}

	printf("%c[%d;%dH", 27, 1, 80);

	//Start Navigating through Command prompt
	navigate();
	
	return 0;
}







/////////////////////////////////////////////////////////MY DIRECTORY FUNCTION/////////////////////////////////////////////







//**********************************************************************
// Method returns total number of files + Directory withing given path
//**********************************************************************
int countTotalFiles(const char *currPath)
{
	dirList.clear();
	int files = 0;
	DIR *dr = opendir(currPath);;
	struct dirent *dir;

	while ((dir = readdir(dr)) != NULL)
	{
		//changed logic implementation of while content
		
		string s(dir->d_name);
		if(s == ".."){
			if(strcmp(currPath, root) != 0){
				dirList.push_back(string(dir->d_name));
				files++;
			}
		}else{
			dirList.push_back(string(dir->d_name));
			files++;
		}
	}
	closedir(dr);

	return files;
}

//************************************************************************
// function that prints directory/file within given terminal size
//************************************************************************
void gotoDir(const char *currPath)
{
	//DIR opening error handling removed	
	dirList.clear();
	totalFiles = countTotalFiles(currPath);
	unsigned int printingLimit = getScreenCapacity();
	sort(dirList.begin(), dirList.end());
	wintrack = 0;
	printf("\033[H\033[J");
	printf("%c[%d;%dH", 27, 1, 1);
	
	unsigned int i, j=1;
	for (i = 0; i < totalFiles ; i++){
		if(j > printingLimit){
			break;
		}
		
		char currFile[dirList[i].length() + 1];
		strcpy(currFile, dirList[i].c_str());
		printDIR(currFile, currPath);
		j++;
	}

	printf("%c[%d;%dH", 27, 1, 80);
}

//************************************************************************
// function to display file/Directory's MetaDta
//************************************************************************
void printDIR(const char *fileName, const char *filePath){
	
	string location = searchflag ? string(fileName) : string(filePath) + "/" + string(fileName);
	char path[location.length() + 1];
	strcpy(path, location.c_str());
	//cout<<"Display path : "<<path<<"     ";
	
	struct stat proprt;
	int status = stat(path, &proprt);
	if (status == -1){
		perror("lstat");
		return;
	}
	
	string op = "";
	if(S_ISDIR(proprt.st_mode)){
		op += "d";
	}else{
		op += "-";
	}
	if(proprt.st_mode & S_IRUSR){
		op += "r";
	}else{
		op += "-";
	}
	if(proprt.st_mode & S_IWUSR){
		op += "w";
	}else{
		op += "-";
	}
	if(proprt.st_mode & S_IXUSR){
		op += "x";
	}else{
		op += "-";
	}
	if(proprt.st_mode & S_IRGRP){
		op += "r";
	}else{
		op += "-";
	}
	if(proprt.st_mode & S_IWGRP){
		op += "w";
	}else{
		op += "-";
	}
	if(proprt.st_mode & S_IXGRP){
		op += "x";
	}else{
		op += "-";
	}
	if(proprt.st_mode & S_IROTH){
		op += "r";
	}else{
		op += "-";
	}
	if(proprt.st_mode & S_IWOTH){
		op += "w";
	}else{
		op += "-";
	}
	if(proprt.st_mode & S_IXOTH){
		op += "x";
	}else{
		op += "-";
	}
	
	cout<<op;
	
	struct passwd *pwdid = getpwuid(proprt.st_uid);
	struct group *grpid = getgrgid(proprt.st_gid);
	if (pwdid != 0)	printf("\t%-8s", pwdid->pw_name);
	if (grpid != 0)	printf(" %-8s", grpid->gr_name);

	printf("%10.2fK", ((double)proprt.st_size) / 1024);
	char *tt = (ctime(&proprt.st_mtime));
	tt[strlen(tt) - 1] = '\0';
	printf("%30s", tt);
	if(S_ISDIR(proprt.st_mode)){
		printf("\033[1;32m");
		printf("\t%-20s\n", fileName);
		printf("\033[0m");
	}else{
		printf("\t%-20s\n", fileName);
	}
}



//////////////////////////////////////////////////////////SCROLLING//////////////////////////////////////////////////






//**********************************************************************
// Method returns count of maximun num of file to be printed on terminal
//**********************************************************************
int getScreenCapacity(){
	struct winsize ws;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	rowsize = ws.ws_row - 1;
	return min(totalFiles, rowsize);
}







/////////////////////////////////////////////////////////////NAVIGATE///////////////////////////////////////////






unsigned int xcor = 1, ycor = 80;


//**********************************************************************
// Method that update current path when backspace key pressed
//**********************************************************************
void setPathToUpper(char *presDir){
	string presDirStr = string(presDir);
	size_t pos = presDirStr.find_last_of("/\\");
	string backPath = presDirStr.substr(0, pos);
	strcpy(curPath, backPath.c_str());				//update backpath in global curPath
}


//**********************************************************************
// Method for navigation on key press in a Normal mode
//**********************************************************************
void navigate(){

	curPath = root;
	xcor = 1, ycor = 80;
	printf("%c[%d;%dH", 27, xcor, ycor);    //goto x,y cordinate
	char ch;

	struct termios backupCANON, newCANON;
	tcgetattr(fileno(stdin), &backupCANON);
	//switch to canonical mode and echo mode
	newCANON = backupCANON;
	newCANON.c_lflag &= ~ICANON;
	newCANON.c_lflag &= ~ECHO;

	if (tcsetattr(fileno(stdin), TCSAFLUSH, &newCANON) != 0){
		fprintf(stderr, "CANONICAL SETTINGS ERROR\n");
	}else{
		while (1){
			
			int endOfDisplay = rowsize + 1;
			printf("%c[%d;%dH", 27, endOfDisplay, 1);	//goto endOfDisplay
			cout << "-----NORMAL MODE----- (Press : to switch Command Mode)";
			printf("%c[%d;%dH", 27, xcor, ycor);    //goto x,y cordinate

			ch = cin.get();
			if (ch == 27){
				
				ch = cin.get();
				ch = cin.get();

				//If UP-arrow Key press
				if (ch == 'A'){
					if (xcor + wintrack > 1){
						xcor--;
						if (xcor > 0)	printf("%c[%d;%dH", 27, xcor, ycor);    //goto x,y cordinate
						else if (xcor + wintrack >= 1){
							printf("%c[2J", 27);    //clear screen;
							
							wintrack = wintrack>0 ? wintrack-1 : wintrack;
							
							//cout<<"wintrack : "<<wintrack<<"***********";
							printf("%c[%d;%dH", 27, 1, 1);	//goto begining of screen
							
							unsigned int i = wintrack;
							while(i <= rowsize + wintrack - 1){
								char currFileName[dirList[i].length() + 1];
								strcpy(currFileName, dirList[i].c_str());
								printDIR(currFileName, curPath);
								i++;
							}
							xcor++;
							printf("%c[%d;%dH", 27, xcor, ycor);    //goto x,y cordinate
						}
					}
				}
				//If DOWN-arrow Key press
				else if (ch == 'B')
				{
					if (xcor + wintrack < (totalFiles))
					{
						xcor++;
						if (xcor <= rowsize)	printf("%c[%d;%dH", 27, xcor, ycor);    //goto x,y cordinate
						else if (xcor + wintrack <= totalFiles)
						{
							printf("%c[2J", 27);    //clear screen;
							int screenCapacity = getScreenCapacity() - 1;
							
							wintrack = totalFiles>rowsize ? wintrack+1: wintrack;
							
							printf("%c[%d;%dH", 27, 1, 1);	//goto begining of screen
							int i = wintrack;
							while (i <= screenCapacity + wintrack)
							{
								char currFileName[dirList[i].length() + 1];
								strcpy(currFileName, dirList[i].c_str());
								printDIR(currFileName, curPath);
								i++;
							}
							xcor--;
						}
						printf("%c[%d;%dH", 27, xcor, ycor);    //goto x,y cordinate
					}
				}
				//If RIGHT-arrow Key press
				else if (ch == 'C')
				{
					//cout<<"RIGHT"<<endl;
					if (forw_stack.empty() == false)
					{
						if(searchflag!=1)	back_stack.push(string(curPath));
						string forwFile = forw_stack.top();
						forw_stack.pop();
						strcpy(curPath, forwFile.c_str());
						
						searchflag=0;
						gotoDir(curPath);
						xcor = 1, ycor = 80;
						printf("%c[%d;%dH", 27, xcor, ycor);    //goto x,y cordinate
					}
				}
				//If LEFT-arrow Key press
				else if (ch == 'D')
				{
					if (back_stack.empty() == false)
					{
						if(searchflag!=1)	forw_stack.push(string(curPath));
						string backFile = back_stack.top();
						back_stack.pop();
						strcpy(curPath, backFile.c_str());
						searchflag=0;
						
						gotoDir(curPath);
						xcor = 1, ycor = 80;
						printf("%c[%d;%dH", 27, xcor, ycor);    //goto x,y cordinate
					}
				}
			}
			//If HOME key pressed
			else if (ch == 104 || ch == 72)
			{
				if(strcmp(curPath, root) != 0)
				{
					if(searchflag!=1)
						back_stack.push(string(curPath));
					
					//empty the forward stack------------------------
					while (!forw_stack.empty())
						forw_stack.pop();
					
					strcpy(curPath, root);
					searchflag=0;
					gotoDir(curPath);
					xcor = 1, ycor = 80;
					printf("%c[%d;%dH", 27, xcor, ycor);    //goto x,y cordinate
				}
				
			}
			//If Back-Space key pressed
			else if (ch == 127)
			{
				//cout<<"*************curPathr"<<curPath<<"***********";
				string cpath = string(curPath);
				if ((strcmp(curPath, root) != 0) && searchflag!=1)
				{
					//cout<<"**************Root : "<<root<<"***********";
					back_stack.push(curPath);
					
					//empty the forward stack------------------------
					while (!forw_stack.empty())
						forw_stack.pop();
					
					setPathToUpper(curPath);
					gotoDir(curPath);
					xcor = 1, ycor = 80;
					printf("%c[%d;%dH", 27, xcor, ycor);    //goto x,y cordinate
				}
			}
			//If Enter key pressed
			else if (ch == 10)
			{
				//cout<<"********prev curPath : "<<curPath<<endl;
				string curDir = dirList[xcor + wintrack - 1];
				//cout<<"********CurDir/file  : "<<curDir<<endl;
				string fullPath;
				fullPath = searchflag==1 ? curDir : string(curPath) + "/" + curDir;

				char *path = new char[fullPath.length() + 1];
				strcpy(path, fullPath.c_str());
				//cout<<"**************"<<path<<"************";

				struct stat sb;
				stat(path, &sb);

				//If file type is Directory
				if ((sb.st_mode & S_IFMT) == S_IFDIR)
				{
					//cout<<"DIR"<<endl;
					xcor = 1;
					searchflag=0;
					if (curDir != string("."))
					{						
						if (curDir == string(".."))
						{
							back_stack.push(string(curPath));
							
							//empty the forward stack------------------------
							while (forw_stack.empty()==false)
								forw_stack.pop();
						
							setPathToUpper(curPath);
						}
						else
						{
							if (curPath != NULL)
							{
								back_stack.push(string(curPath));
								
								//empty the forward stack------------------------
								while (forw_stack.empty() == false)
									forw_stack.pop();
							
							}
							curPath = path;
						}
					}
					
					gotoDir(curPath);
				}
				//If file type is Regular File
				else if ((sb.st_mode & S_IFMT) == S_IFREG)
				{
					//cout<<"**************File Path : "<<filepath<<"***************"<<endl;
					int fileOpen=open("/dev/null",O_WRONLY);
					dup2(fileOpen,2);
					close(fileOpen);
					pid_t processID = fork();
					if(processID == 0)
					{
						execlp("xdg-open","xdg-open",path,NULL);
						exit(0);
					} 
				}
				else
				{
					printERROR("Unknown File !!! :::::"+string(curDir));
				}
			}
			//If : pressed , go into command mode
			else if (ch == 58)
			{
				printf("%c[%d;%dH", 27, rowsize + 1, 1);	//goto last line of screen
				printf("%c[2K", 27);
				cout << ":";
				//cout<<"going into command mode :"<<endl;
				int result = startCommandMode();
				xcor = 1;
				printf("%c[%d;%dH", 27, xcor, ycor);    //goto x,y cordinate
				if (result == 1)
				{
					gotoDir(curPath);
					//cout<<"goto out : ";
				}
				else if (result == 2)
				{
					//cout<<"search out : ";
				}
				else
				{
					//cout<<"Normal out : ";
					searchflag=0;
					gotoDir(curPath);
				}
			}
		}
	}

	tcsetattr(fileno(stdin), TCSANOW, &backupCANON);
}









/////////////////////////////////////////////////////EXTRA UTILITY//////////////////////////////////////////////////







//**********************************************************************
// This Function used to display error in Red Formate
//**********************************************************************
void printERROR(string str){
	cout<<endl<<"\033[0;31m"<<str<<"\033[0m"<<":"<<endl;
}


















////////////////////////////////////////////////////////COMMAND MODE/////////////////////////////////////////////
vector<string> tokens;

//**********************************************************************
// It handles absolute and relative paths in cmd mode
//**********************************************************************
string pathProcessing(string str)
{
	char firstchar = str[0];
	string absolutePath="";
	string basepath = string(root);
	if(firstchar =='/')
	{
		absolutePath = basepath + str;
	}
	else if(firstchar=='~')
	{
		absolutePath = basepath + str.substr(1,str.length());
	}
	else if(firstchar=='.')
	{
		absolutePath = string(curPath) + str.substr(1,str.length());	
	}
	else
	{
		absolutePath = string(curPath)+ "/" + str;
	}

	return absolutePath;
}

//**********************************************************************
// It splits input string into tokens separeted by space
//**********************************************************************
void inputProcessing(string str)
{
	
	unsigned int i=0;
	tokens.clear();
	int flag=0;
	while(i<str.length())
	{
		string sub="";
		while(str[i]!=' ' && i<str.length())
		{
			if(str[i]=='\\')
			{
				sub += str[i+1];
				i=i+2;
			}
			else{

				sub += str[i];
				i++;
			}
			
		}
		if(flag==1)
		{
			string abspath=pathProcessing(sub);
			tokens.push_back(abspath);
		}
		else{

			tokens.push_back(sub);
			if(tokens[0]!="create_file" && tokens[0]!="create_dir" && tokens[0]!="search")
			{
				flag=1;	
			}
			
		}
		i++;	
	}

}

//**********************************************************************
// After each command runs, it clear command line area
//**********************************************************************
void clearCommand()
{
	int lastLine = rowsize + 1;
	printf("%c[%d;%dH",27,lastLine,1);
	printf("%c[2K", 27);
	cout<<":";
}

//**********************************************************************
// Method that starts the CommandMode
//**********************************************************************
int startCommandMode()
{
	char ch;
	do{
		string input;
		while(((ch = getchar())!= 10) && ch!=27)
		{
			if(ch==127)
			{
				clearCommand();
				if(input.length()<=1)
				{
					input="";
				}
				else{
					input = input.substr(0,input.length()-1);
				}
				cout<<input;
				
			}
			else{
				input = input + ch;
				cout<<ch;	
			}
			
		}
		inputProcessing(input);
		if(ch==10)
		{
			string command= tokens[0];
			if(command == "copy")
			{
				//cout<<"Copy command  : "<<endl;
				copycommand(tokens);
				clearCommand();
			}
			else if(command == "move")
			{
				//cout<<"Move command  : "<<endl;
				movecommand(tokens);
				clearCommand();
			}
			else if(command == "rename")
			{
				//cout<<"rename command  : "<<endl;
				renameFiles(tokens);
				clearCommand();
			}
			else if(command == "create_file")
			{
				//cout<<"create_file command  : "<<endl;
				createNewFiles(tokens);
				clearCommand();
			}
			else if(command == "create_dir")
			{
				//cout<<"create_dir command  : "<<endl;
				makeDirectories(tokens);
				clearCommand();
			}
			else if(command == "delete_file")
			{
				//cout<<"delete_file command  : "<<endl;
				removeFiles(tokens);
				clearCommand();
			}
			else if(command == "delete_dir")
			{
				//cout<<"delete_dir command  : "<<endl;
				removeDirectories(tokens);
				clearCommand();
			}
			else if(command == "goto")
			{
				//cout<<"goto command  : "<<endl;
				string gpath = "";
				
				if(tokens.size()!=2)				{
					printERROR("Invalid Argument in Goto");		//handle error case to continue program
				}else{
					gpath= tokens[1];
				}
				
				
				
				char *path = new char[gpath.length() + 1];
		 		strcpy(path, gpath.c_str());
		 		back_stack.push(string(curPath));

				//empty the forward stack------------------------
					while (!forw_stack.empty())
						forw_stack.pop();

		 		curPath = path;
		 		//cout<<"\ngoto path : "<<path<<endl;
		 		return 1;
			}
			else if(command == "search")
			{
				//cout<<"search command  : "<<endl;
				int status=searchcommand(tokens);
				if(status!=0)
					return 2;
				clearCommand();
				
			}
			else{
				printERROR("Invalid Command");
				clearCommand();
			}
		}	

	}while(ch != 27);

   return 0;
}







/////////////////////////////////////////////////////COPY COMMAND///////////////////////////////////////////////////






//**********************************************************************
// This function recursively copy all files/dir into destination path
//**********************************************************************
void copydirectory(char *path, char *des)
{
	int status= mkdir(des,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	 if(-1 == status)
	 {
	 	printERROR("Error in creating the Directory in path :::::  "+string(path));
	 }

	DIR *d;
	struct dirent *dir;
	d = opendir(path);
	if (d) 
	{
	    while ((dir = readdir(d)) != NULL) 
	    {	 
		      if( (string(dir->d_name) == "..") || (string(dir->d_name) == ".") )	
		      {   } 
		  	  else
		  	  {		
		  	  		string finalpath=string(path) + "/" +string(dir->d_name);
					char* newpath = new char[finalpath.length() + 1];
					strcpy(newpath, finalpath.c_str());

					string finaldestpath=string(des) + "/" +string(dir->d_name);
					char* newdestpath = new char[finaldestpath.length() + 1];
					strcpy(newdestpath, finaldestpath.c_str());

		  			struct stat sb;
					if (stat(newpath,&sb) == -1) {
				        perror("lstat");
				    }
				    else{

				    	if((S_ISDIR(sb.st_mode)))
					    {
					    	copydirectory(newpath,newdestpath);
					    }
					    else
					    {
					    	 copyfile(newpath,newdestpath);
					    }
				    }
		  	  		
		  	  }
	    }
	}
	else{
		printERROR("No such Directory found while copying with path :::::"+ string(path));
	}
}

//**********************************************************************
// This function used to copy files at specify Destination
//**********************************************************************
void copyfile(char *path, char *des)
{
	//cout<<"\nsource path : "<<path<<endl;
	//cout<<"\ndestination path :"<<des<<endl;

	char block[1024];
	int in, out;
	int nread;
	
	in = open(path, O_RDONLY);
	out = open(des, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
	while((nread = read(in,block,sizeof(block))) > 0)
		write(out,block,nread);

	struct stat sourcestat,deststat;
	if (stat(path,&sourcestat) != -1) {  
    }
    if (stat(des,&deststat) != -1) {  
    }
    
    int status1=chown(des,sourcestat.st_uid, sourcestat.st_gid);
	if(status1!=0)
		printERROR("Error in setting ownership of file using chown");

    int status2=chmod(des,sourcestat.st_mode);
	if(status2!=0)
		printERROR("Error in setting permission of file using chmod");

}

//**********************************************************************
// This Function handle copy cmd argument specified by user
//**********************************************************************
void copycommand(vector<string> list)
{
	unsigned int len = list.size();
	if(len < 3)
	{
		printERROR("Less number of Argument in copy command !!!");
	}
	else{
			
		for(unsigned int i=1;i<len-1;i++)
		{
			//getFileNameFromPath
			string newData = list[i];
			size_t pos = newData.find_last_of("/\\");
			string name = newData.substr(pos+1, newData.length());
			
				
			//cout<<"\nfilename : "<<name;

			string destpath= list[len-1];
			destpath =destpath + "/" + name;
			char *des = new char[destpath.length() + 1];
			strcpy(des, destpath.c_str());
			//cout<<"\ndespath in copy : "<<des<<endl;
	
			char *path = new char[newData.length() + 1];
			strcpy(path, newData.c_str());
			if(isDIR(path))
			{
				copydirectory(path,des);
			}
			else
			{
				copyfile(path,des);
			}
		}
	}
}







/////////////////////////////////////////CREATE DELETE FILES///////////////////////////////////////////////////







//**********************************************************************
// This function is to remove single file 
//**********************************************************************
void removeSingleFile(char *path)
{
	//cout<<"path for deleting file : "<<path<<endl;
	int status= remove(path);
	 if(status != 0)
	 {
	 	printERROR("Error in removing the File with path ::::: "+string(path));
	 }
	
}

//**********************************************************************
// It removes multiple files that passed by User in argument
//**********************************************************************
void removeFiles(vector<string> list)
{	
	if(list.size()<2)
	{
		 printERROR("Less number of Argument in delete_file command");
	}
	for(unsigned int i=1;i<list.size();i++)
	{
		 char *path = new char[list[i].length() + 1];
		 strcpy(path, list[i].c_str());
		 removeSingleFile(path);
	}	 
}

//**********************************************************************
// This Function used to create file in given path
//**********************************************************************
void createSingleFile(char *path)
{
		//cout<<"\ncreateSingleFile path : "<<path<<endl;
		int status=open(path,O_RDONLY | O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ); 	
		if (status == -1)
	    {
			 printERROR("Error in creating new file path :::::  " + string(path));	       
	    }
}

//**********************************************************************
// This function is used to create multiple files as given by user
//**********************************************************************
void createNewFiles(vector<string> list)
{

	if(list.size() < 3)
	{
		printERROR("Less Number of Argument in Renaming !!!");
		return ;
	}
	unsigned int len=list.size();
	string destpath= pathProcessing(list[len-1]);
	//cout<<"\ndestpath : "<<destpath<<endl;
	for(unsigned int i=1;i<len-1;i++)
	{
		 string fileName = destpath + "/" + list[i];
		 char *path = new char[fileName.length() + 1];
		 strcpy(path, fileName.c_str());
		 createSingleFile(path);
		 
	}
		
}

//**********************************************************************
// This Function rename the file/Dir
//**********************************************************************
void renameFiles(vector<string> list)
{
	if(list.size()!=3)
	{
		printERROR("Invalid Argument in Renaming !!!");
	}
	else{
		string initName = list[1];
		string finalName = list[2];
		rename(initName.c_str(),finalName.c_str());
	}
	
}







/////////////////////////////////////////////////CREATE DIRECTORY////////////////////////////////////////////////////////






//**********************************************************************
// This Function used to create directory(s) 
//**********************************************************************
void makeDirectories(vector<string> list)
{
	
	unsigned int len=list.size();
	if(list.size() <= 2)
	{
		printERROR("Lesser Number of Argument in create_dir!!!");
	}
	else{
		string destpath= pathProcessing(list[len-1]);
		//cout<<"\ndestpath : "<<destpath<<endl;
		for(unsigned int i=1;i<len-1;i++)
		{
			 string dir = destpath + "/" + list[i];
			 char *path = new char[dir.length() + 1];
			 strcpy(path, dir.c_str());
			 //cout<<"\nmkdir name :"<<dir<<endl;
			 int status= mkdir(path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			 if(-1 == status)
			 {
			 	printERROR("Error in creating the Directory in path :::::  "+string(path));
			 }
		}	
	}
	
}








////////////////////////////////////////////////DELETE DIRECTORY////////////////////////////////////////////////////////







//**********************************************************************
// This function recursively remove all file and dir inside it.
// and finally delete dir itself
//**********************************************************************
void removeSingleDirectory(char *path)
{
	DIR *d;
	struct dirent *dir;
	d = opendir(path);
	if (d) 
	{
	    while ((dir = readdir(d)) != NULL) 
	    {
		      if( (string(dir->d_name) == "..") || (string(dir->d_name) == ".") )	
		      {   } 
		  	  else
		  	  {		
		  	  		string finalpath=string(path) + "/" +string(dir->d_name);
					char* newpath = new char[finalpath.length() + 1];
					strcpy(newpath, finalpath.c_str());

		  			struct stat sb;
					if (stat(newpath,&sb) == -1) {
				        perror("lstat");
				    }
				    else{

				    	if((S_ISDIR(sb.st_mode)))
					    {
					    	removeSingleDirectory(newpath);
					    }
					    else
					    {
					    	removeSingleFile(newpath);
					    }
				    }		
		  	  }
	    }
	    closedir(d);
	    int status= rmdir(path);
		if(-1 == status)
		{
		 	printERROR("Error in removing the Directory with path ::::: "+string(path));
		}
	}
	else{
		printERROR("No such Directory Exist !!!");
	}
}

//**********************************************************************
// It removes multiple directory that passed by User in argument
//**********************************************************************
void removeDirectories(vector<string> list)
{
	if(list.size() < 2)
	{
		printERROR("Less number of argument in delete_dir !!!");
	}
	for(unsigned int i=1;i<list.size();i++)
	{
		 char *path = new char[list[i].length() + 1];
		 strcpy(path, list[i].c_str());
		 //cout<<"\nrmdir path :"<<path<<endl;
		 removeSingleDirectory(path);
	}	 
}







////////////////////////////////////////////////////MOVE COMMAND///////////////////////////////////////////////////





//**********************************************************************
// This Function moves file/folder to destination path
//**********************************************************************
void movecommand(vector<string> list)
{
	unsigned int len = list.size();
	if(len < 3)
	{
		printERROR("Less number of Argument in move command !!!");
	}
	else{

		for(unsigned int i=1;i<len-1;i++)
		{				
			//getFileNameFromPath
			string newData = list[i];
			size_t pos = newData.find_last_of("/\\");
			string name = newData.substr(pos+1, newData.length());
			
			
			
			//cout<<"\nfilename : "<<name;

			string destpath= list[len-1];
			destpath =destpath + "/" + name;
			char *des = new char[destpath.length() + 1];
			strcpy(des, destpath.c_str());
			//cout<<"\ndespath in copy : "<<des<<endl;
	
			char *path = new char[newData.length() + 1];
			strcpy(path, newData.c_str());
			if(isDIR(path))
			{
				copydirectory(path,des);
				removeSingleDirectory(path);
			}
			else
			{
				copyfile(path,des);
				removeSingleFile(path);
			}
		}
	}
}







////////////////////////////////////////////////////////SEARCH COMMAND///////////////////////////////////////////////////







vector<string> searchResult;

//**********************************************************************
// It match two file name are same or not
//**********************************************************************
int isNameMatch(string tobesearch, string name)
{
	if (tobesearch == name)
		return 1;
	else
		return 0;
}

//**********************************************************************
// This function used to print the search Data
//**********************************************************************
void printsearchData()
{
	totalFiles = dirList.size();
	unsigned int len = getScreenCapacity();
	sort(dirList.begin(), dirList.end());
	wintrack = 0;
	printf("\033[H\033[J");
	printf("%c[%d;%dH", 27, 1, 1);

	for (unsigned int i = 0, itr = 1; i < totalFiles && itr <= len; i++, itr++)
	{
		 char *searchfullpath = new char[dirList[i].length() + 1];
         strcpy(searchfullpath,dirList[i].c_str());
         printDIR(searchfullpath,root);

	}

	printf("%c[%d;%dH", 27, 1, 1);
}

//**********************************************************************
// It searches given filename into its recursive subfolder 
// if filename found push its absolute path into searchResult vector
//**********************************************************************
void searchanything(char *path, string filename)
{
	DIR *d;
	struct dirent *dir;
	d = opendir(path);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if ((string(dir->d_name) == "..") || (string(dir->d_name) == "."))
			{
			}
			else
			{
				string name = string(dir->d_name);
				string finalpath = string(path) + "/" + name;
				char *newpath = new char[finalpath.length() + 1];
				strcpy(newpath, finalpath.c_str());

				struct stat sb;
				if (stat(newpath, &sb) == -1)
				{
					perror("lstat");
				}
				else
				{

					if ((S_ISDIR(sb.st_mode)))
					{
						if (isNameMatch(filename, name))
						{
							searchResult.push_back(string(newpath));
						}
						searchanything(newpath, filename);
					}
					else
					{
						if (isNameMatch(filename, name))
						{
							searchResult.push_back(string(newpath));
						}
					}
				}
			}
		}
	}
	else
	{
		//printERROR("No such Directory path Exist while searching !!!");
	}
}

//**********************************************************************
//This function process search argument given by user & display results
//**********************************************************************
int searchcommand(vector<string> list)
{
	searchResult.clear();
	unsigned int len = list.size();
	if (len != 2)
	{
		printERROR("Less number of Argument in search command !!!");
		return 0;
	}
	else
	{
		string filename = list[1];
		char *path = new char[strlen(curPath) + 1];
		strcpy(path, curPath);
		//cout<<"\npath : "<<path<<endl;
		//cout<<"\nfilename : "<<filename<<endl;
		searchanything(path, filename);

		if (searchResult.size() < 1)
		{
			printERROR("No search result found  for file/Dir ::::: " + filename);
			return 0;
		}
		else
		{
			back_stack.push(string(curPath));
			
			//empty the forward stack------------------------
			while (!forw_stack.empty())
				forw_stack.pop();
					
			searchflag=1;
			dirList.clear();
			dirList = searchResult;
			printsearchData();
		}
	}

	return 1;
}







//**********************************************************************
// This Function returns whether given path is of directory or file
//**********************************************************************
int isDIR(char *filepath)	//changed implementation
{
	struct stat sb;
	if (stat(filepath,&sb) == -1) {
        perror("lstat");
		return -1;
    }
	if(S_ISDIR(sb.st_mode))
		return 1;

	return 0;
}