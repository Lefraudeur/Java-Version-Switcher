# Java Version Switcher

A quick way to change between java versions in windows cmd

### <u>Installation:</u>
Put jvs.exe in a folder, and add that folder to your system PATH environnement variable.
Make sure jvs.exe is in a folder where it has read/write permissions.

### <u>Config:</u>
```
{
    "17": "C:/Program Files/Zulu/zulu-17",
    "8": "C:/Program Files/Zulu/zulu-8",
    "install": {
        "zulu17": "https://cdn.azul.com/zulu/bin/zulu17.44.53-ca-jdk17.0.8.1-win_x64.zip",
        "zulu20": "https://cdn.azul.com/zulu/bin/zulu20.32.11-ca-jdk20.0.2-win_x64.zip",
        "zulu8": "https://cdn.azul.com/zulu/bin/zulu8.72.0.17-ca-jdk8.0.382-win_x64.zip"
    }
}
```
The config `jvs.json` needs to be in the same folder as `jvs.exe`.
<br/>In this example config, the command `jvs 17` will delete existing java dirs in the `PATH` env variable, and add the directory `C:/Program Files/Zulu/zulu-17/bin` to `PATH`.
<br/>It will also set `JAVA_HOME` to `C:/Program Files/Zulu/zulu-17`.
<br/>The command `jvs install zulu17` will download the zip file at `https://cdn.azul.com/zulu/bin/zulu17.44.53-ca-jdk17.0.8.1-win_x64.zip` and extract it to a folder `zulu17` in the same place as `jvs.exe`.
<br/>It will then add that folder to `jvs.json` so that you can `jvs zulu17`.

### <u>Usage:</u>
`jvs <javaVersion>`
`jvs install <javaVersion>`

### <u>Info:</u>
This does not change the system wide PATH env variable, it calls _putenv in the parent process (so cmd.exe), so that only that process java path will be changed.
It uses game hacking techniques with VirtualAllocEx, WriteProcessMemory and CreateRemoteThread in order to achieve that.

#### Why a c++ program instead of a simple batch/powershell script ?
Because c++ is fun, and it allows to have a cool json config file, as well as the ability to remove existing java paths to avoid conflicts.
Overall it's fast, and it will allow more features in the future.