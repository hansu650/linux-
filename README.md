<div align="center">

# Linux System and Analysis Course Portfolio

**Qin Tian · 202431123002054**

Course notes and portfolio documentation for practical Linux topics covering services, multithreading, network programming, system calls, kernel modules and operating-system analysis.

</div>

## Overview

This repository supports my **Linux System and Analysis** course portfolio. It currently keeps dated class notes and README documentation rather than the full Word report, screenshots or compiled task outputs. The associated course work covers five practical tasks across user space, kernel space, Linux services, processes, threads, sockets, system calls and device drivers. Some kernel-related work depends on a specific Ubuntu environment and cannot be reproduced directly on every Linux system.

> Kernel-related tasks are environment-specific. A custom system call requires booting into the rebuilt kernel, and a kernel module must be built against the running kernel.

## Tasks

The table below summarises the five assessment topics and the tracked files that currently support them. When the current repository does not contain the complete source code or screenshot evidence, the status is marked clearly instead of being invented.

| Task | Topic | Main Technologies | Tracked Evidence |
|---|---|---|---|
| Task 1 | Hadoop Installation and Configuration | Java, SSH, HDFS/YARN concepts, XML configuration, process checking | `4-20-hadoop.md` |
| Task 2 | Multi-threaded CPU Utilisation Control | C concepts, process/thread theory, pthread, CPU utilisation | `5-11.md`, `5-18.md`, `5-25.md` |
| Task 3 | TCP / Network Programming | TCP, UDP, sockets, ports, blocking behaviour, C/S and P2P concepts | `6-8.md`, `6-15.md` |
| Task 4 | Custom System Call and Kernel Work | system calls, interrupts, kernel/user mode, boot files | `6-1.md`, `3-16.md` |
| Task 5 | Character Device Driver | kernel modules, device-file idea, user/kernel data flow | TODO: complete driver source and screenshot evidence are not tracked in this repository |

## Repository Structure

The repository root is:

```text
D:\daima\cursor\linux系统与分析
```

Current tracked files are organised as dated notes:

```text
.
├── 3-2.md              # Course stages, Ubuntu stage, programming and kernel topics
├── 3-9.md              # GNU/GPL, Ubuntu installation, distributions and package management
├── 3-9-2.md            # Partitioning, UEFI/GPT and boot-mode notes
├── 3-16.md             # TTY, permissions, /boot, GRUB, vmlinuz and initrd.img notes
├── 4-13-apache.md      # Apache, HTTPS, modules and file-system support notes
├── 4-20-hadoop.md      # Hadoop, network checking and jps-related notes
├── 4-27-内核000.md      # Empty placeholder note file
├── 5-11.md             # Linux programming stage, gcc, GDB, Makefile and CPU-curve task notes
├── 5-18.md             # Process, thread and kernel-stack notes
├── 5-25.md             # fork, vfork, pthread and CPU-utilisation notes
├── 6-1.md              # Interrupts, system calls and kernel/user mode notes
├── 6-8.md              # Network layering, TCP/UDP and socket overview notes
├── 6-15.md             # Socket programming, C/S, P2P and blocking/I/O notes
└── 6-22.md             # Raw sockets, ICMP, ping and network-security awareness notes
```

`dos.c` and `myping.c` were removed from Git tracking by request, but the local files were left on disk.

## Environment

The notes mention the following environment or tools:

- Ubuntu 24.04 desktop is mentioned in the installation notes.
- GCC, GDB and Makefile are mentioned in the Linux programming notes.
- Linux boot files such as `vmlinuz`, `System.map`, `initrd.img` and GRUB are discussed in the boot and kernel notes.
- Hadoop process checking with `jps` is mentioned in the Hadoop notes.

Other versions, paths and screenshots are environment-specific and should be checked on the actual machine before reproduction.

## Task Highlights

### Task 1 — Hadoop

The Hadoop-related notes connect service configuration with network checking, user/environment setup and process verification. The most important verification idea is that a startup command is not enough by itself; the final process state should be checked, for example with `jps`.

### Task 2 — CPU Heart Curve

The process and thread notes explain why threads are lighter than processes, how pthread programming relates to CPU scheduling, and how CPU utilisation can be controlled by busy and idle time. The course task used these ideas to produce a visible CPU-usage curve.

### Task 3 — TCP Chat / Network Programming

The network notes cover IP addresses, ports, TCP, UDP, socket structures and the difference between C/S and P2P thinking. They also discuss blocking behaviour and why a program may need processes, threads or I/O multiplexing when it waits for multiple events.

### Task 4 — Custom System Call

The kernel notes explain interrupts, user mode, kernel mode and the system-call boundary. The boot notes also connect kernel work with `/boot`, GRUB, `vmlinuz`, `System.map` and `initrd.img`. A system call is part of the rebuilt kernel, not a loadable module.

### Task 5 — Character Device Driver

The repository does not currently track the complete character-driver source. The related notes still prepare the necessary concepts: kernel modules, device files, file operations and the relationship between user-space file access and kernel-space code.

## Verification Summary

| Task | Most Useful Verification Idea | Current Repository Evidence |
|---|---|---|
| Task 1 | Check Hadoop processes after startup | `4-20-hadoop.md` mentions `jps` |
| Task 2 | Compare expected CPU-control behaviour with visible CPU usage | `5-11.md`, `5-25.md` |
| Task 3 | Confirm that two endpoints communicate without blocking the interface | `6-8.md`, `6-15.md` |
| Task 4 | Confirm the running kernel and related boot files | `3-16.md`, `6-1.md` |
| Task 5 | Confirm module/device behaviour in the final report environment | TODO: complete tracked evidence is not present here |

## Representative Problems and Lessons

- **Permissions and users:** The early notes emphasise `sudo`, `su`, normal users and root privileges. This supports the later habit of checking user identity, ownership and permissions before changing a system.
- **Service verification:** Hadoop notes mention process checking with `jps`, which is more reliable than trusting only a startup message.
- **Build and programming tools:** The programming notes connect `gcc`, GDB and Makefile with multi-file or multi-threaded C work.
- **Boot-file matching:** The boot notes discuss GRUB, `vmlinuz`, `System.map` and `initrd.img`, which helps explain why kernel-related files must match the running kernel.
- **Blocking and concurrency:** The socket notes discuss blocking behaviour, threads and I/O models, which explains why a network program may freeze if it waits in the wrong execution flow.
- **Raw packet awareness:** The raw-socket and ICMP notes show that lower-level network programming should be studied carefully and responsibly.

## Innovation Thinking

**A useful way to find an innovation point is to trace the history of a technology and identify which limitation of the earlier method it solved.**

Examples from the course notes and discussions:

- polling -> interrupts;
- one blocking flow -> threads and I/O multiplexing;
- `select` -> more scalable event mechanisms such as `epoll`;
- compiling everything into the kernel -> loadable modules;
- high-level sockets -> raw packets for understanding lower protocol layers.

The goal is not to use a newer API only because it is newer, but to understand the problem that made the newer method necessary.

## Screenshots

No screenshot image files are currently tracked in this repository. The final course report contains environment-specific screenshots, but they are not stored here.

<details>
<summary>Screenshot slots for future documentation</summary>

- TODO: Hadoop `jps` result screenshot.
- TODO: CPU heart-curve screenshot.
- TODO: TCP chat GUI screenshot.
- TODO: custom system-call test and kernel-log screenshot.
- TODO: character device read/write and kernel-log screenshot.

</details>

## Class Notes and References

- Teacher Xu's PPT, classroom explanations and official requirement documents were the primary references for the course work.
- LinuxIDC and Runoob were used for quick command and concept review.
- Selected sections of 《鸟哥的Linux私房菜-基础篇》第四版 were used for deeper reading about boot files, kernel compilation and module management.
- AI and class discussions were used to compare possible causes and organise next checks.
- Final conclusions should be verified on the local system instead of accepted only from a reference.

## Safety and Ethics

The tracked notes include raw socket, ICMP, ping and denial-of-service awareness topics.

> Low-level packet and denial-of-service examples related to this course are included only for classroom explanation, protocol study and defensive awareness. Do not use them against systems or networks without explicit authorisation.

This README does not provide attack commands, real targets, bypass methods or direct misuse steps.

## Learning Outcomes

- Linux practical operation
- root-cause analysis
- C programming and debugging
- process and thread understanding
- network programming
- user-space and kernel-space reasoning
- technical communication
- source evaluation and responsible AI use
- report and README writing
- open-source and innovation awareness

## Notes

- Commands may depend on system paths and kernel versions.
- Kernel modules must match the running kernel.
- Screenshots and reports contain environment-specific results.
- Readers should check their own paths, users and versions before reproducing commands.
- This repository currently stores notes and documentation; full task source files and report screenshots should be added only if they are intended to be tracked.
