# Java Version Switcher

A quick way to change between java versions

### Installation:
Put jvs.exe in a folder, and add that folder to your system PATH environnement variable.
You can also put jvs.exe in C:\Windows, but you will have to create the config file with admin rights, or launch jvs in admin rights the first time.

### Config:
```
{
    "17": "C:/Program Files/Zulu/zulu-17",
    "8": "C:/Program Files/Zulu/zulu-8"
}
```
In this example config, the command `jvs 17` will delete existing java dirs in the `PATH` env variable, and add the directory `C:/Program Files/Zulu/zulu-17/bin` to `PATH`.
<br/>It will also set `JAVA_HOME` to `C:/Program Files/Zulu/zulu-17`

### Usage:
`jvs <javaVersion>`

### Info:
This does not change the system wide PATH env variable, it calls _putenv in the parent process (so cmd.exe), so that only that process java path will be changed.
It uses game hacking techniques with VirtualAllocEx, WriteProcessMemory and CreateRemoteThread in order to achieve that.

#### Why a c++ program instead of a simple batch file ?
Because c++ is fun, and it allows to have a cool json config file, as well as the ability to remove existing java paths to avoid conflicts.
Overall it's fast, and it will allow more features in the future, such as a jdk downloader.