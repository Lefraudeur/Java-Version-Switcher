# Java Version Switcher

A quick way to change between java versions

### Installation:
Put jvs.exe in a folder, and add that folder to your system PATH environnement variable.

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
