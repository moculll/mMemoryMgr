# mMemoryMgr
## 1. Memory Map
| mMemoryMgr | part0->part1->NULL |
| ---------- | ---------- |
| **part0(mode: 1)** | **part1(mode: 0)** |
| ns0(1byte) | header(mMemoryNs_t)(ns0) |
| ns1(2byte) | ns0(7byte) |
| ns2(3byte) | align size(1byte) |
| ns3(4byte) | header(mMemoryNs_t)(ns1) |
| ns4(5byte) | ns1(12byte) |
| ns5(6byte) | align size(4byte) |
| 3byte reserved for align | header(mMemoryNs_t)(ns2) |
| header(mMemoryNs_t)(ns5) | ns2(17byte) |
| header(mMemoryNs_t)(ns4) | align size(7byte) |
| header(mMemoryNs_t)(ns3) | header(mMemoryNs_t)(ns3) |
| header(mMemoryNs_t)(ns2) | ns3(61byte) |
| header(mMemoryNs_t)(ns1) | align size(3byte) |
| header(mMemoryNs_t)(ns0) | **unused space** |

```
mode could be 0/1

[0]: aligned mode
[1]: reversed mode
```

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

