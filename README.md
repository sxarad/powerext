# PowerExt - Developer Tools
PowerExt can display assembly version, assembly name, public key, strong name etc in the File Properties dialog. It's a Windows File Explorer add-in that is written in C++. For .NET Assemblies (.dll and .exe files) it adds an additional .NET tab to the Windows Explorer's File Properties dialog.


## v1.0 (Alpha 5) released on 14th May 2017
[Download PowerExt v1.0.5.1 (Alpha 5)](https://github.com/ssidpat/powerext/releases/download/v1.0.5.1/PowerExt-1.0.5.1.Alpha.5.zip)

Release notes:
1. Project and source code permanently moved from CodePlex to GitHub
2. Code updated to work with either .NET 2.x or .NET 4.x frameworks
3. Both 32bit (x86) and 64bit (x64) installer packages are included in the latest download

PowerExt is packaged in a simple MSI installer. There are builds for both x86 and x64 editions of Windows in the zip file available for download. 

Please send your feedback, bug reports and comments to: ssidpat at gmail dot com


## Tested on:
* Windows 7 Pro SP1 (x86 and x64) with .NET 3.5 
* Windows 10 (x64) with .NET 4.7


## Requirements
1. [Microsoft Visual C++ 2015 Redistributable](https://www.microsoft.com/en-us/download/details.aspx?id=53587 "Visual C++ 2015 Redistributable") is required. Chances are you already have this on you machine.
2. .NET Framework - Although PowerExt itself doesn't use any managed code the unmanaged APIs that it uses are installed by .NET Framework so you will need a version of .NET Framework. (The extension has been tested with .NET 3.5 and 4.7) 


## Screenshot
![](img/PowerExt-Screenshot-01.png?raw=true)


## Troubleshooting
* Module C:\Program Files\PowerExt\PowerExt.dll failed to register. HRESULT -2147024770. Contact your support personnel. - If you get this error then you need to install Microsoft Visual C++ 2015 Redistributable.
* If you are using 64-bit version of Windows 7 and after installing PowerExt you don't see the .NET tab in the File Properties dialog then you might have installed the 32-bit (x86) build. Uninstall the 32-bit build and install the 64-bit build from the releases page. 


## Files
1. PowerExt.dll - The main add-in or extension dll
2. DnaInfo.exe - .NET Assembly Info (Diagnostics tool)


## How does it work?
PowerExt is a ATL COM component. It registers itself as property sheet handlers for .DLL and .EXE files with Windows Explorer. When a user right-clicks on a DLL or EXE file within Windows Explorer window and selects Properties, the extension will try to determine if the selected file is a .NET assembly or not. If the file is a .NET assembly then you will see a new .NET tab next to the General tab. The .NET tab displays information such as assembly name, assembly version, assembly strong name, public key token and the public key. This information is particularly useful for programmers working with different versions of the same assembly. Without PowerExt, this information isn't displayed by Windows Explorer in Vista or Windows 7.


## License
PowerExt is licensed under the BSD 3-clause "New" or "Revised" License. 


## Limitations
* Being a new project this extension hasn't been tested thoroughly yet
* Might not be able to identify all .NET assemblies
* On some machines the extension isn't able to register the property sheet handlers and as a result you might not see the .NET tab


## Tips
* In Windows Explorer select a file and hit Alt+Enter to bring up the File Properties dialog

