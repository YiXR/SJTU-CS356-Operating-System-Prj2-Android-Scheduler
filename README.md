
# Author:
-Xingrui Yi

# Introduction:

In this project, we are asked to add a new scheduling policy:

Weighted Round Robin:
Round-robin scheduling treats all tasks equally, but there are
times when it is desirable to give some tasks preference over
others.

Android Tasks can be classified into foreground groups and
background groups.

WRR assigned more milliseconds as a time slice for foreground
groups. (In our problem, 100ms for fore and 10ms for back)

Foreground and Background Groups:
From user level, you can run ps -P on the device or emulator to check
the assigned groups for each task.

At the kernel level, a task's group information (*) can be found using a
task pointer. Refer to the line 96 in kernel/sched/debug.c and use that function appropriately. The return value will be "/" for a foreground (and system group in
earlier versions) group, "/bg_non_interactive" for a background group.

# Files:

Goldfish: 

It includes all the .c .h I've changed in the kernel. In order to distingush between two sched.h, I put them into different subfolder.

Test:

It includes test.c and Android.mk, which is used to test the performace of wrr.

Compare:

It includes compare.c and Android.mk, which is used to compare the performance of different scheduling.

Test Compare:

It includes test_compare.c and Android.mk, which is used run compare program by groups.