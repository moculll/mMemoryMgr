# mMemoryMgr
## 1. Memory Map
| mMemoryMgr | part0->part1->NULL |
| ---------- | ---------- |
| **part0(mode: 0)** | **part1(mode: 1)** |
| namespace(1byte) | header(mMemoryNs_t) |
| namespace(2byte) | namespace(7byte) |
| namespace(3byte) | align size(1byte) |
| namespace(4byte) | header(mMemoryNs_t) |
| namespace(5byte) | namespace(12byte) |
| namespace(6byte) | align size(4byte) |
| 3byte reserved for align | header(mMemoryNs_t) |
| header(mMemoryNs_t)(6byte) | namespace(17byte) |
| header(mMemoryNs_t)(5byte) | align size(7byte) |
| header(mMemoryNs_t)(4byte) | header(mMemoryNs_t) |
| header(mMemoryNs_t)(3byte) | namespace(61byte) |
| header(mMemoryNs_t)(2byte) | align size(3byte) |
| header(mMemoryNs_t)(1byte) | **unused space** |

## 2. clone repo and Compile example
```
git clone https://github.com/moculll/mMemoryMgr.git
git checkout debug
make
./build/test
```
## 3. Debug via Vscode
```
git clone https://github.com/moculll/mMemoryMgr.git
git checkout debug
```
```
press debug C/C++ file button in vscode
```


## 4. use as submodule

1. add branch debug's rbtree.c rbtree.h into your project

2. execute follow commands
```
git clone https://github.com/moculll/mMemoryMgr.git
git checkout main
```

3. add mMemoryMgr.c mMemoryMgr.h to your project
4. follow example usage in testcase1.c

