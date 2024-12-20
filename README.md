**Warning:** Use of the *Bulk User Builder* program is at your own risk. The author is not responsible for any potential damage, malfunctions, or unintended consequences. **This program can significantly alter the user accounts on your system and may lead to system instability.** 

- **Do not run this program on your primary or production computer unless you fully understand the risks.**
- The program is intended for testing purposes only and should be used in a controlled environment (e.g., virtual machines or test systems).  
- **All responsibility for any consequences lies solely with you.**

---

# Bulk User Builder for Windows

Bulk User Builder is a testing tool that enables you to create more than 1,000 users (up to a maximum of 10,000) in a Windows operating system. This program is useful for performance testing, configuration, or other purposes related to bulk user account creation.

## Important Note:
This program is designed for **English versions of Windows**. In other language versions, the names of user groups may differ. To make the program work with non-English versions of Windows, you need to modify the group names in the `createnewuser` function.

### Code Section to Modify:
The part of the code that determines the group names is shown below:

```cpp
if (isUserAdmin) {
    groupName = L"Administrators";
} else {
    groupName = L"Users";
}
```

### Steps to Fix for Other Languages:
1. Open the program's source code and locate the above section in the `createnewuser` function.
2. Replace the group names (`"Administrators"` and `"Users"`) with the exact names used in your Windows system for these groups.  
   To find the correct group names:
   - Press `Win + R`, type `lusrmgr.msc`, and press Enter.
   - Navigate to **Groups** and locate the **Administrators** and **Users** groups.
   - Copy the exact names of these groups as they appear in your system and paste them into the program's code.
3. After making these changes, recompile the program.

### Example for Non-English Versions:
For example, if you're using a French version of Windows:
```cpp
if (isUserAdmin) {
    groupName = L"Administrateurs"; // French for "Administrators"
} else {
    groupName = L"Utilisateurs"; // French for "Users"
}
```

## Features:
- Create a large number of users on Windows.
- Easy to use and configure.
- Flexible for modifications to meet your specific requirements.

## License:
Bulk User Builder is distributed under the [MIT License](https://opensource.org/licenses/MIT), allowing you to freely use, modify, and distribute the program. The only requirement is to credit the original author.

## Installation and Usage:
1. Clone the repository:  
   ```bash
   git clone https://github.com/Pilm404/Bulk-user-builder-for-windows.git
   ```
2. Follow the instructions in the `README.md` file to configure and run the program.

## Author:
Developer: [Pilm404](https://github.com/Pilm404)  
Please ensure to credit the original author when using or modifying the program.

For more information, visit the repository: [Bulk User Builder for Windows](https://github.com/Pilm404/Bulk-user-builder-for-windows).
