# PREEMPT RT Patching

Image not tested for now (WIP). Patch for kernel `5.10.4`:

```
https://cdn.kernel.org/pub/linux/kernel/projects/rt/5.10/older/patch-5.10.4-rt22.patch.gz
```

Apply rt22 patch (put it into `linux_5.10` dir inside Image Source root):

```bash
patch -p1 < patch-5.10.4-rt22.patch
```

You can find kernel params file in this dir with CONFIG_PREEMPT_RT enabled and some other params.


