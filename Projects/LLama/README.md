# LLama2 inference

`stories15M.bin` have inference performance about `~8.1 tok/s` on LicheeRV Nano RISC-V Linux core.

## Compile && Run

```bash
git clone https://github.com/karpathy/llama2.c
cd llama2.c
# apply patch
patch -p1 -i ../compilation_fixes.patch
export COMPILER=... # path to dir with risc-v cross-compiler
make
```
Then copy `run` to board.


