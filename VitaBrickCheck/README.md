# VitaBrickCheck
Little script that checks for potential bricking code in Vita apps.
Works with any Linux, easily portable to FreeBSD and OSX.

The tool looks for anomalous mountpoints in certain executable extensions.
It is not advanced (the payload could be obfuscated), so it is NOT and CANNOT be 100% effective.

## Usage
```
./VitaBrickCheck.sh /path/of/file.vpk
```

## Example of malware detection
In case of an actual infected app, it returns:
```
WARNING: '/tmp/tmp.BqxPPOHZtI/mai_moe/mai.suprx' is suspicious

Execution finished for /home/user/infected_package.vpk.
1 suspicious file(s) found.

```
(yes, it was tested on the real thing).

This script is released as public domain.
