| **Field**       | **File (Regular)**                            | **Directory**                               | **Explanation**                                                  |
|-----------------|-----------------------------------------------|---------------------------------------------|------------------------------------------------------------------|
| **`st_mode`**    | `S_IFREG | 0644` (regular file, permissions)  | `S_IFDIR | 0755` (directory, permissions)   | Specifies the file type (regular or directory) and access mode. |
| **`st_nlink`**   | `1` (one hard link)                           | `>= 2` (including `.` and `..` links)        | Number of hard links. For directories, `st_nlink` includes subdirs. |
| **`st_size`**    | Actual size of the file (e.g., 1024 for 1KB)  | `0` (size of the directory structure)        | File size in bytes. For directories, it's often `0`.             |
| **`st_uid`**     | Current user ID (`getuid()`)                  | Current user ID (`getuid()`)                 | User ID of the file or directory owner.                         |
| **`st_gid`**     | Current group ID (`getgid()`)                 | Current group ID (`getgid()`)                | Group ID of the file or directory owner.                        |
| **`st_atime`**   | Last access time (`time_t`)                   | Last access time (`time_t`)                  | Time when the file or directory was last accessed.               |
| **`st_mtime`**   | Last modification time (`time_t`)             | Last modification time (`time_t`)            | Time when the file or directory was last modified.               |
| **`st_ctime`**   | Metadata change time (`time_t`)               | Metadata change time (`time_t`)              | Time when file or directory metadata (permissions, ownership) was last changed. |






# st_mode **File Mode Creation with Permissions (C Program)**

## **Introduction**
This program allows users to create a file mode for a file in Linux, specifying the file type, permissions for the owner, group, and others, as well as setting special bits such as `setuid`, `setgid`, and `sticky bit`. It provides a simple and user-friendly interface to help users understand file permissions in a clear and straightforward way.

## **Features**
- **File Type Selection**: Users can choose from several file types such as regular files, directories, symbolic links, and more.
- **Numeric Permission Input**: Users enter numeric values (0-7) for owner, group, and others, which correspond to specific permission sets.
- **Special Bit Options**: Users can set special bits (`setuid`, `setgid`, and `sticky`) for the file.
- **User-Friendly CLI**: The program provides an intuitive guide explaining how to input permissions and special bits, ensuring the user can easily understand how to configure the file mode.

## **File Permissions Explained**
In Linux, file permissions are represented by three categories: **Owner**, **Group**, and **Others**. Each of these categories can have three types of permissions: **read**, **write**, and **execute**. These permissions are represented numerically using the octal system.

- **Read (r)**: Value 4
- **Write (w)**: Value 2
- **Execute (x)**: Value 1

The possible permission combinations for each category (owner, group, others) are represented by a number from 0 to 7, which is the sum of the values for read, write, and execute. For example:
- `7` = **rwx** (read, write, and execute)
- `6` = **rw-** (read and write)
- `5` = **r-x** (read and execute)
- `4` = **r--** (read only)

### **Special Bits**:
- **Set UID (setuid)**: When set, the file is executed with the privileges of the file's owner.
- **Set GID (setgid)**: When set, the file is executed with the privileges of the group associated with the file.
- **Sticky Bit**: When set, only the file's owner or root can delete the file.

## **Usage**

### **Step 1: File Type Selection**
The program starts by asking you to select the type of file you're configuring. The available types are:
- **regular**: A standard file.
- **directory**: A directory.
- **link**: A symbolic link.
- **char**: A character device.
- **block**: A block device.
- **socket**: A socket file.
- **fifo**: A FIFO (named pipe).

### **Step 2: Permission Input**
After selecting the file type, the program asks you to enter numeric permissions for **Owner**, **Group**, and **Others**. Each input should be a number between 0 and 7 that represents the permissions for each category.

The available values for each category are:
- `0` = `---` (no permissions)
- `1` = `--x` (execute only)
- `2` = `-w-` (write only)
- `3` = `-wx` (write and execute)
- `4` = `r--` (read only)
- `5` = `r-x` (read and execute)
- `6` = `rw-` (read and write)
- `7` = `rwx` (read, write, and execute)

### **Step 3: Special Bit Selection**
Once the basic permissions are set, you can choose to set special bits. The available special bits are:
- **setuid**: Set the user ID on execution.
- **setgid**: Set the group ID on execution.
- **sticky**: Restrict file deletion to the file's owner or root.

### **Step 4: Display File Mode**
After all inputs are provided, the program calculates and displays the file mode in both octal and decimal formats.

## **Example Usage**

### **Sample Input**
```plaintext
Permission Guide:
You will enter a number for each of Owner, Group, and Others permissions.
Each number corresponds to a set of permissions:
0 = --- (no permissions)
1 = --x (execute only)
2 = -w- (write only)
3 = -wx (write and execute)
4 = r-- (read only)
5 = r-x (read and execute)
6 = rw- (read and write)
7 = rwx (read, write, and execute)

Enter file type (regular, directory, link, char, block, socket, fifo): regular
Enter numeric permission for Owner (0-7): 7
Enter numeric permission for Group (0-7): 5
Enter numeric permission for Others (0-7): 4
Enter special bits (none, setuid, setgid, sticky): setuid
```

### **Sample Output**
```plaintext
st_mode (Octal): 0104754
st_mode (Decimal): 33972
```

---

## **Code Structure**

### **`print_mode(mode_t mode)`**
This function takes the `mode_t` value, which represents the file mode, and prints it in both octal and decimal formats.

### **`parse_permissions(int owner, int group, int others)`**
This function computes the numeric value for the permissions based on the input for owner, group, and others.

### **`print_permission_guide()`**
This function prints a guide explaining how to input the permission values for each category (Owner, Group, Others), as well as the special bits options.

### **`main()`**
The main function drives the user interaction:
1. Requests the file type.
2. Prompts for permission inputs for Owner, Group, and Others.
3. Asks for special bits.
4. Computes and displays the file mode.

---

## **Conclusion**
This program offers a simple and intuitive way to create file modes using numeric permissions and special bits. It eliminates the need for users to understand complex permission syntax and provides an easy-to-use command-line interface. By following the prompts, users can quickly and accurately set up file modes for various types of files.

