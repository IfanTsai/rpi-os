# rpi-os

A simple os in Raspberry Pi 3B.

- process management
    - privilege isolation and **system call**
    - **fork** system call like unix fork
    - static priority time slic task **O(n) scheduleer**
- memory management
    - **virtual memory** mapping base on MMU
    - full process memory isolation
    - handle page fault interrupt
- exception handler
- simple printf API
- framebuffer



<img src="https://img.caiyifan.cn/typora_picgo/image-20211118223421247.png" alt="image-20211118223421247" width="20%" height="20%" /> 
