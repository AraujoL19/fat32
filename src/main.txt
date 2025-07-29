/*INCLUDE*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*DEFINE*/
#define SECTOR_SIZE	512
#define CLUSTER_SIZE	2*SECTOR_SIZE
#define ENTRY_BY_CLUSTER CLUSTER_SIZE /sizeof(dir_entry_t)
#define NUM_CLUSTER	4096
#define fat_name	"fat.part"

// FAT32-like filesystem structure
// This is a simplified version and does not implement all FAT32 features
struct _dir_entry_t
{
	unsigned char filename[18];
	unsigned char attributes;
	unsigned char reserved[7];
	unsigned short first_block;
	unsigned int size;
};

// This structure represents a directory entry in the filesystem.
// It contains the filename, attributes (0 for free, 1 for directory, 2 for file),
// reserved bytes, the first block where the file or directory starts, and the size of the file.
typedef struct _dir_entry_t  dir_entry_t;

// This structure represents a data cluster in the filesystem.
// It contains an array of directory entries and an array of data bytes.
// The directory entries are used to store information about files and directories,
// while the data bytes are used to store the actual content of files.
// The size of the cluster is defined by CLUSTER_SIZE, which is set to 1024 bytes (2 sectors of 512 bytes each).
// The number of clusters is defined by NUM_CLUSTER, which is set to 4096.
// The total size of the filesystem is 4MB (4096 clusters * 1024 bytes per cluster).
// The filesystem uses a FAT (File Allocation Table) to keep track of which clusters are free and which are used.
// The FAT is an array of unsigned short integers, where each entry corresponds to a cluster.
// An entry value of 0x0000 indicates that the cluster is free,
// an entry value of 0xfffd indicates the end of the filesystem,
// an entry value of 0xfffe indicates a reserved cluster, and
// an entry value of 0xffff indicates a bad cluster.
// The root directory is represented by an array of 32 directory entries, which can hold up to 32 files or directories.
// The filesystem supports creating directories, creating files, reading files, writing to files, appending to files, and unlinking (deleting) files or directories.
// The filesystem is initialized with a boot block, a FAT, and an empty root directory.
// The boot block is a fixed-size block that contains metadata about the filesystem, such as the cluster size and number of
union _data_cluster
{
	dir_entry_t dir[CLUSTER_SIZE / sizeof(dir_entry_t)];
	unsigned char data[CLUSTER_SIZE];
};

typedef union _data_cluster data_cluster;

/*DATA DECLARATION*/
unsigned short fat[NUM_CLUSTER];
unsigned char boot_block[CLUSTER_SIZE];
dir_entry_t root_dir[32];
data_cluster clusters[4086];

/*Function declaration*/
void append(char* path, char* content);
void create(char* path);
void mkdir (char* path);
void read  (char* path);
void unlink(char* path);
int  find_free_space(dir_entry_t* dir);

void init(void)
{
	FILE* ptr_file; // Pointer to the file where the filesystem will be stored
	int i;
	ptr_file = fopen(fat_name,"wb"); // Open the file in binary write mode
	for (i = 0; i < CLUSTER_SIZE; ++i) // Fill the boot block with a fixed pattern
		boot_block[i] = 0xbb;

	fwrite(&boot_block, sizeof(boot_block), 1,ptr_file); // Write the boot block to the file

	fat[0] = 0xfffd; // Set the first entry of the FAT to indicate the end of the filesystem
	for (i = 1; i < 9; ++i) // Set the next 8 entries to indicate reserved clusters
		fat[i] = 0xfffe;

	fat[9] = 0xffff; // Set the 10th entry to indicate a bad cluster
	for (i = 10; i < NUM_CLUSTER; ++i) // Set the remaining entries to indicate free clusters
		fat[i] = 0x0000;

	fwrite(&fat, sizeof(fat), 1, ptr_file); // Write the FAT to the file
	fwrite(&root_dir, sizeof(root_dir), 1,ptr_file); // Write the root directory to the file

	for (i = 0; i < 4086; ++i) // Initialize the clusters to zero
		fwrite(&clusters, sizeof(data_cluster), 1, ptr_file);

	fclose(ptr_file); // Close the file after writing the filesystem structure
}

void load(void)
{
	FILE* ptr_file; // Pointer to the file where the filesystem is stored
	int i; // Load the boot block from the file
	ptr_file = fopen(fat_name, "rb"); // Open the file in binary read mode
	fseek(ptr_file, sizeof(boot_block), SEEK_SET); // Seek to the position after the boot block
	fread(fat, sizeof(fat), 1, ptr_file); // Read the FAT from the file
	fclose(ptr_file); // Close the file after reading the FAT
}

// This function saves the FAT to the filesystem by writing it to the file specified by fat_name.
void save_fat(void)
{
	FILE* ptr_file; // Pointer to the file where the FAT will be saved
	int i;
	ptr_file = fopen(fat_name, "r+b"); // Open the file in binary read-write mode
	fseek(ptr_file, sizeof(boot_block), SEEK_SET); // Seek to the position after the boot block
	fwrite(fat, sizeof(fat), 1, ptr_file); // Write the FAT to the file
	fclose(ptr_file); // Close the file after writing the FAT
}

// This function loads a data cluster from the filesystem by reading it from the file specified by fat_name.
data_cluster* load_cluster(int block)
{
	data_cluster* cluster; // Pointer to a data_cluster structure that will hold the loaded cluster
	cluster = calloc(1, sizeof(data_cluster)); // Allocate memory for the data_cluster structure
	FILE* ptr_file; // Pointer to the file where the data cluster will be loaded from
	ptr_file = fopen(fat_name, "rb"); // Open the file in binary read mode
	fseek(ptr_file, block*sizeof(data_cluster), SEEK_SET); // Seek to the position corresponding to the specified block
	fread(cluster, sizeof(data_cluster), 1, ptr_file); // Read the data cluster from the file
	fclose(ptr_file); // Close the file after reading the data cluster
	return cluster; // Return the pointer to the loaded data_cluster structure
}

// This function writes a data cluster to the filesystem by writing it to the file specified by fat_name.
data_cluster* write_cluster(int block, data_cluster* cluster)
{
	FILE* ptr_file; // Pointer to the file where the data cluster will be written
	ptr_file = fopen(fat_name, "r+b"); // Open the file in binary read-write mode
	fseek(ptr_file, block*sizeof(data_cluster), SEEK_SET); // Seek to the position corresponding to the specified block
	fwrite(cluster, sizeof(data_cluster), 1, ptr_file); // Write the data cluster to the file
	fclose(ptr_file); // Close the file after writing the data cluster
}

// This function wipes a data cluster by writing an empty data_cluster structure to the specified block.
void wipe_cluster(int block)
{
	data_cluster* cluster;
	cluster = calloc(1, sizeof(data_cluster));
	write_cluster(block, cluster);
}

// This function finds the parent directory of a given path in the filesystem.
data_cluster* find_parent(data_cluster* current_cluster, char* path, int* addr)
{
	char path_aux[strlen(path)]; // Auxiliary string to hold the path
	strcpy(path_aux, path); // Copy the path to the auxiliary string
	char* dir_name = strtok(path_aux, "/"); // Tokenize the path to get the first directory name
	char* rest     = strtok(NULL, "\0"); // Get the rest of the path after the first directory name

	dir_entry_t* current_dir = current_cluster->dir; // Pointer to the directory entries in the current cluster

	int i=0; // Iterate through the directory entries to find the specified directory
	while (i < 32) { // Check each directory entry in the current cluster
		dir_entry_t child = current_dir[i];
		if (strcmp(child.filename, dir_name) == 0 && rest){
			data_cluster* cluster = load_cluster(child.first_block);
			*addr = child.first_block;
			return find_parent(cluster, rest, addr);
		}
		else if (strcmp(child.filename, dir_name) == 0 && rest){ // If the directory is found and there is no rest of the path
			return NULL;
		}
		i++;
	}

	if (!rest) // If there is no rest of the path, return the current cluster
		return current_cluster;

	return NULL;
}

// This function finds a data cluster in the filesystem based on a given path.
data_cluster* find(data_cluster* current_cluster, char* path, int* addr)
{
	if (!path || strcmp(path, "/") == 0) // If the path is empty or just the root directory, return the current cluster
		return current_cluster;

	char path_aux[strlen(path)]; // Auxiliary string to hold the path
	strcpy(path_aux, path); // Copy the path to the auxiliary string
	char* dir_name = strtok(path_aux, "/"); // Tokenize the path to get the first directory or file name
	char* rest     = strtok(NULL, "\0"); // Get the rest of the path after the first directory or file name

	dir_entry_t* current_dir = current_cluster->dir; // Pointer to the directory entries in the current cluster

	int i=0;
	while (i < 32) { // Check each directory entry in the current cluster
		dir_entry_t child = current_dir[i]; // If the directory entry is found and there is a rest of the path
		if (strcmp(child.filename, dir_name) == 0){ // If the directory or file name matches
			data_cluster* cluster = load_cluster(child.first_block); // Load the data cluster corresponding to the directory or file
			*addr = child.first_block; // Update the address to the first block of the found directory or file
			return find(cluster, rest, addr); // Recursively search in the found cluster for the rest of the path
		}
		i++;
	}
	return NULL;
}

// This function extracts the name of a file or directory from a given path.
char* get_name(char* path)
{

	char name_aux[strlen(path)]; // Auxiliary string to hold the path
	strcpy(name_aux, path); // Copy the path to the auxiliary string

	char* name = strtok(name_aux, "/"); // Tokenize the path to get the first directory or file name
	char* rest = strtok(NULL, "\0"); // Get the rest of the path after the first directory or file name
	if (rest != NULL) // If there is a rest of the path, recursively call get_name to get the name from the rest
		return get_name(rest); // Otherwise, return the name found

	return (char*) name; // Return the name of the file or directory
}

// This function finds a free space in a directory by checking each entry in the directory's dir array.
int find_free_space(dir_entry_t* dir)
{
	int i;
	for (i = 0; i < 32; ++i){ // Iterate through the directory entries
		if (!dir->attributes) // If the attributes of the directory entry are 0 (indicating free space)
			return i; // Return the index of the free space found
		dir++; // Move to the next directory entry
	}
	return -1; // If no free space is found, return -1
}

// This function copies a string from the source to the target character array.
void copy_str(char* target, char* src)
{
	int len = strlen(src); // Get the length of the source string
	int i; // Iterate through the characters of the source string
	for (i = 0; i < len; ++i) { // Copy each character from the source to the target
		target[i] = src[i]; // Copy the character from the source to the target
	}
}

// This function searches for a free block in the FAT (File Allocation Table).
int search_fat_free_block(void)
{
	load(); // Load the FAT from the filesystem
	int i; // Iterate through the FAT entries to find a free block
	for (i = 10; i < 4096; ++i) // Start from the 10th entry in the FAT
		if(!fat[i]){ // If the entry is 0 (indicating a free block)
			fat[i] = -1; // Mark the block as used by setting it to -1
			save_fat(); // Save the updated FAT to the filesystem
			return i; // Return the index of the free block found
		}
	return 0;
}

// This function creates a new directory at the specified path.
void mkdir(char* path)
{
	if(path == "/") // If the path is the root directory, do nothing
		return;

	int root_addr = 9; // The address of the root directory in the filesystem
	data_cluster* root_cluster = load_cluster(9); // Load the root directory cluster from the filesystem
	data_cluster* cluster_parent = find_parent(root_cluster, path, &root_addr); // Find the parent directory of the specified path

	if (cluster_parent){ // If the parent directory is found
		int free_position = find_free_space(cluster_parent->dir); // Find a free space in the parent directory
		int fat_block = search_fat_free_block(); // Search for a free block in the FAT
		if (fat_block && free_position != -1) { // If a free block is found and there is free space in the parent directory
			char* dir_name = get_name(path); // Get the name of the directory to be created from the path
			copy_str(cluster_parent->dir[free_position].filename, dir_name); // Copy the directory name to the directory entry
			cluster_parent->dir[free_position].attributes = 1; // Set the attributes of the directory entry to 1 (indicating a directory)
			cluster_parent->dir[free_position].first_block = fat_block; // Set the first block of the directory entry to the free block found
			write_cluster(root_addr, cluster_parent); // Write the updated parent directory cluster back to the filesystem
		}
	}
	else
		printf("PATH NOT FOUND\n"); // If the parent directory is not found, print an error message
}

// This function lists the contents of a directory at the specified path.
void ls(char* path)
{
	int root_addr = 9; // The address of the root directory in the filesystem
	data_cluster* root_cluster = load_cluster(9); // Load the root directory cluster from the filesystem
	data_cluster* cluster = find(root_cluster, path, &root_addr); // Find the directory cluster corresponding to the specified path
	int i; // Iterate through the directory entries in the found cluster
	if (cluster){ // If the directory cluster is found
		for (i = 0; i < 32; ++i){ // Check each directory entry in the cluster
			if (cluster->dir[i].attributes == 1 || cluster->dir[i].attributes == 2) // If the attributes indicate a directory (1) or a file (2)
				printf("%s\n", cluster->dir[i].filename); // Print the filename of the directory or file
		}
	}
	else
		printf("PATH NOT FOUND\n"); // If the directory cluster is not found, print an error message
}

// This function creates a new file at the specified path.
void create(char* path)
{
	if(path == "/") // If the path is the root directory, do nothing
		return;

	int root_addr = 9; // The address of the root directory in the filesystem
	data_cluster* root_cluster = load_cluster(9); // Load the root directory cluster from the filesystem
	data_cluster* cluster_parent = find_parent(root_cluster, path, &root_addr); // Find the parent directory of the specified path

	if (cluster_parent){ // If the parent directory is found
		int free_position = find_free_space(cluster_parent->dir); // Find a free space in the parent directory
		int fat_block = search_fat_free_block(); // Search for a free block in the FAT
		if (fat_block && free_position != -1) { // If a free block is found and there is free space in the parent directory
			char* dir_name = get_name(path); // Get the name of the file to be created from the path
			copy_str(cluster_parent->dir[free_position].filename, dir_name); // Copy the file name to the directory entry
			cluster_parent->dir[free_position].attributes = 2; // Set the attributes of the directory entry to 2 (indicating a file)
			cluster_parent->dir[free_position].first_block = fat_block; // Set the first block of the directory entry to the free block found
			write_cluster(root_addr, cluster_parent); // Write the updated parent directory cluster back to the filesystem
		}
	}
	else
		printf("PATH NOT FOUND\n");
}

// This function writes content to a file at the specified path.
void write(char* path, char* content)
{
	int root_addr = 9; // The address of the root directory in the filesystem
	data_cluster* root_cluster = load_cluster(9); // Load the root directory cluster from the filesystem
	data_cluster* cluster = find(root_cluster, path, &root_addr); // Find the file cluster corresponding to the specified path
	if (cluster){ // If the file cluster is found
		copy_str(cluster->data, content); // Copy the content to the data field of the file cluster
		write_cluster(root_addr, cluster); // Write the updated file cluster back to the filesystem
	}
	else
		printf("FILE NOT FOUND\n");

}

// This function checks if a data cluster is empty by iterating through its directory entries.
int empty(int block)
{
	data_cluster* cluster = load_cluster(block); // Load the data cluster from the filesystem
	int i; // Iterate through the directory entries in the cluster
	for (i = 0; i < 32; ++i) // Check each directory entry in the cluster
		if(cluster->dir[i].attributes != 0) // If the attributes of the directory entry are not 0 (indicating free space)
			return 0;

	return 1;
}

// This function unlinks (deletes) a file or directory at the specified path.
void unlink(char* path)
{
	load(); // Load the filesystem from the file
	int i; // Iterate through the directory entries to find the specified file or directory
	int root_addr = 9; // The address of the root directory in the filesystem
	data_cluster* root_cluster = load_cluster(root_addr); // Load the root directory cluster from the filesystem
	find_parent(root_cluster, path, &root_addr); // Find the parent directory of the specified path
	data_cluster* cluster = load_cluster(root_addr); // Load the data cluster corresponding to the parent directory
	if (cluster) { // If the parent directory cluster is found
		char* name = get_name(path); // Get the name of the file or directory to be unlinked from the path
		for (i = 0; i < 32; ++i) { // Check each directory entry in the cluster
			if (strcmp(cluster->dir[i].filename, name)==0) { // If the directory entry matches the name of the file or directory to be unlinked
				cluster->dir[i].attributes = -1; // Set the attributes of the directory entry to -1 (indicating it is deleted)
				int first = cluster->dir[i].first_block; // Get the first block of the file or directory to be unlinked
				fat[first] = 0x0000; // Mark the first block in the FAT as free
				save_fat(); // Save the updated FAT to the filesystem
				write_cluster(root_addr, cluster); // Write the updated parent directory cluster back to the filesystem
			}
		}
	}
	else
		printf("FILE NOT FOUND UNLINK\n");

}

// This function reads the content of a file at the specified path and prints it to the console.
void read(char* path)
{
	load(); // Load the filesystem from the file
	int root_addr = 9; // The address of the root directory in the filesystem
	data_cluster* root_cluster = load_cluster(9); // Load the root directory cluster from the filesystem
	data_cluster* cluster = find(root_cluster, path, &root_addr); // Find the file cluster corresponding to the specified path
	if (cluster) // If the file cluster is found
		printf("%s\n", cluster->data); // Print the content of the file to the console

	else
		printf("FILE NOT FOUND\n");
}

// This function appends content to a file at the specified path.
void append(char* path, char* content)
{
	load(); // Load the filesystem from the file
	int root_addr = 9; // The address of the root directory in the filesystem
	data_cluster* root_cluster = load_cluster(9); // Load the root directory cluster from the filesystem
	data_cluster* cluster = find(root_cluster, path, &root_addr); // Find the file cluster corresponding to the specified path
	if (cluster){ // If the file cluster is found
		char* data = cluster->data; // Get the current content of the file
		strcat(data, content); // Append the new content to the current content
		copy_str(cluster->data, data); // Copy the updated content back to the data field of the file cluster
		write_cluster(root_addr, cluster); // Write the updated file cluster back to the filesystem
	}

	else
		printf("FILE NOT FOUND\n");

}

// This function processes user commands by reading input from stdin and executing the corresponding filesystem operations.
void command(void)
{
	char name[4096] = { 0 };
    char name2[4096] = { 0 };
	char nameCopy[4096] = { 0 };
	const char aux[2] = "/";
	char aux2[4096] = { 0 };

	char *token;
	int i;

	fgets(name,4096,stdin);

	strcpy(nameCopy,name);

	token = strtok(name,aux);

	if ( strcmp(token, "append ") == 0 && nameCopy[7] == '/')
	{
		for(i = 7; i < strlen(nameCopy)-1; ++i)
		{
			aux2[i-7] = nameCopy[i];
		}
		printf("Digite o texto");
		fgets(name2,4096,stdin);
		append(aux2,name2);
	}
	else if ( strcmp(token, "create ") == 0 && nameCopy[7] == '/')
	{
		for(i = 7; i < strlen(nameCopy)-1; ++i)
		{
			aux2[i-7] = nameCopy[i];
		}
		create(aux2);
	}
	else if ( strcmp(token, "init\n") == 0)
	{
		init();
	}
	else if ( strcmp(token, "load\n") == 0)
	{
		load();
	}
	else if ( strcmp(token, "ls ") == 0 && nameCopy[3] == '/')
	{
		for(i = 3; i < strlen(nameCopy)-1; ++i)
		{
			aux2[i-3] = nameCopy[i];
		}
		ls(aux2);
	}
	else if ( strcmp(token, "mkdir ") == 0 && nameCopy[6] == '/')
	{
		for(i = 6; i < strlen(nameCopy)-1; ++i)
		{
			aux2[i-6] = nameCopy[i];
		}
		mkdir(aux2);
	}
	else if ( strcmp(token, "read ") == 0 && nameCopy[5] == '/')
	{
		for(i = 5; i < strlen(nameCopy)-1; ++i)
		{
			aux2[i-5] = nameCopy[i];
		}
		read(aux2);
	}
	else if ( strcmp(token, "unlink ") == 0 && nameCopy[7] == '/')
	{
		for(i = 7; i < strlen(nameCopy)-1; ++i)
		{
			aux2[i-7] = nameCopy[i];
		}
		unlink(aux2);
	}
	else if ( strcmp(token, "write ") == 0 && nameCopy[6] == '/')
	{
		for(i = 6; i < strlen(nameCopy)-1; ++i)
		{
			aux2[i-6] = nameCopy[i];
		}
		printf("Digite o texto");
		fgets(name2,4096,stdin);
		write(aux2,name2);
	}
	else printf("nao foi possivel encontrar o comando digitado");
}

int main(void)
{
	while(1)
	{
		command();
	}

	return 0;
}