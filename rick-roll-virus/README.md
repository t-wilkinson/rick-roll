# rick-roll-virus

## Goals

Create an obnoxious program which will return any http request with one 'never gonna give you up' youtube video. To make this more obnoxious, overtime the program should further embed itself in the host system. To counteract this, create a second program which can systematically remove all traces of the adversary program.
Makes getting rid of this program game-like. Progressively increase the change that infected computer gets rick rolled over time. Also progressively embed code into more files and create more ways that program will update iptables. For "ultimate" fun, also make it rewrite files with rick-roll lyrics.

- RickRollMania: malware which infects computer and progressively gets more destructive
  - Infection: infect computer very covertly. Make it hard to detect and redundant.
    - Intercept linking process to insert code that writes the actual virus. Or... add a binary onto path that hides gcc, calls gcc with arguments, gets the output binary, and rewrite it there.
    - In terms of places to insert virus code, we can insert it into gcc, which would require sudo privaleges, or we can listen to calls.
  - Intercept:
    - make some http requests rick roll the user, getting progressively more common over time
    - write "never gonna give you up" lyrics to random terminals, sockets, etc.
  - Destruction: begin destroying files (rewriting them with "never gonna give you up lyrics")
  - Ransom: demand money to remove malware
  - Purger: program which removes all traces of the malware

## Notes

- Malware assumes that it is initially run with sudo privaleges. It is possible for at least portions of the malware to work with user privaleges if the system is inproperly configured.

## netedit

Rust progam which iptables will forward new connections to which can modify connections.

## Design decisions

### What is the best way to actually intercept web traffic?

1. Custom interface
   - Forward all traffic to custom interface.
   - Would require a lot of work
2. Iptables (I choose this option)
   - Fairly easy to do, and portable across linux systems.
3. DNS
   - Create custom dns resolver that resolves to a controlled computer. Provides easier configurability as you have control over the computer doing a lot of the "mal intent", but probably easy for infected computer to fix.
4. nfqueues
   - Probably not. Seems to mostly be installed by default on RedHat distributions. Others one must install it first. Issues here.

### Steps to create self-replicating code that is difficult to remove

The first goal is to find the code that is writing the file, so we can insert virus code in there.

1.

```bash
$ strace -f gcc main.c -o build/m-static 2>&1 > /dev/null | grep m-static -C 5
[pid 1379643] readlink("/usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1", 0x7ffc54f65390, 1023) = -1 EINVAL (Invalid argument)
[pid 1379643] faccessat2(AT_FDCWD, "/usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/", F_OK, AT_EACCESS) = 0
[pid 1379643] readlink("/usr/lib", 0x7ffc54f65390, 1023) = -1 EINVAL (Invalid argument)
[pid 1379643] readlink("/usr/lib/crtn.o", 0x7ffc54f65390, 1023) = -1 EINVAL (Invalid argument)
[pid 1379643] prlimit64(0, RLIMIT_NOFILE, NULL, {rlim_cur=1024, rlim_max=512*1024}) = 0
[pid 1379643] newfstatat(AT_FDCWD, "build/m-static", {st_mode=S_IFREG|0755, st_size=15232, ...}, 0) = 0
[pid 1379643] newfstatat(AT_FDCWD, "build/m-static", {st_mode=S_IFREG|0755, st_size=15232, ...}, AT_SYMLINK_NOFOLLOW) = 0
[pid 1379643] unlink("build/m-static")  = 0
[pid 1379643] openat(AT_FDCWD, "build/m-static", O_RDWR|O_CREAT|O_TRUNC, 0666) = 3
[pid 1379643] fcntl(3, F_GETFD)         = 0
[pid 1379643] fcntl(3, F_SETFD, FD_CLOEXEC) = 0
[pid 1379643] openat(AT_FDCWD, "/usr/lib/gcc/x86_64-pc-linux-gnu/12.2.1/../../../../lib/Scrt1.o", O_RDONLY) = 4
[pid 1379643] fcntl(4, F_GETFD)         = 0
[pid 1379643] fcntl(4, F_SETFD, FD_CLOEXEC) = 0
--
[pid 1379643] read(3, "\177ELF\2\1\1\0\0\0\0\0\0\0\0\0\3\0>\0\1\0\0\0 \20\0\0\0\0\0\0"..., 4096) = 4096
[pid 1379643] munmap(0x7f06cb5e3000, 135168) = 0
[pid 1379643] lseek(3, -3208, SEEK_CUR) = 888
[pid 1379643] write(3, "\4\0\0\0\24\0\0\0\3\0\0\0GNU\0lI.\345`q\203\360Z\322F\326J\257D\370"..., 36) = 36
[pid 1379643] close(3)                  = 0
[pid 1379643] newfstatat(AT_FDCWD, "build/m-static", {st_mode=S_IFREG|0644, st_size=15232, ...}, 0) = 0
[pid 1379643] umask(000)                = 022
[pid 1379643] umask(022)                = 000
[pid 1379643] chmod("build/m-static", 0755) = 0
[pid 1379643] close(17)                 = 0
[pid 1379643] close(7)                  = 0
[pid 1379643] close(6)                  = 0
[pid 1379643] close(18)                 = 0
[pid 1379643] close(5)                  = 0
```

Piping this command to `grep -E 'write.*ELF` gives a single line, which

I use `-f` because I notice there must be more syscalls than I'm getting without the flag, and looking the help for strace, `-f` includes forked threads.
I want to follow system calls to see where `m-static` is being written. I saw a `execve()` syscall which looks like it is running a command, confirmed by its corresponding man page. Passing the command output to `grep write` gives no output.

2. `gdb`

Now that I know which syscall and its arguments is probably writing the file, I can use gdb to step through to see what exactly happening.

## TODO

- Move scripts to manage iptables to netedit?
- Make NetEdit more general. Extract the rick roll intercept to a separate program which uses NetEdit.
- Test in docker container in order to isolate iptables?
- Limit the redirects to something like 30% of the time on youtube videos
- Right now only works on the host system. What about infecting the router (which are usually linux based), allows messing with a lot more people more efficiently.
