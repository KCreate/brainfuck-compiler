.def bf_ptr r0
.def bf_calc1 r1b
.def bf_static_one r2b
.def bf_static_zero r3b
.label bf_cells
.org 30000
.label entry_addr
loadi bf_ptr, 0
loadi bf_static_one, 1
loadi bf_static_zero, 0
read bf_calc1, bf_ptr
add bf_calc1, bf_static_one
write bf_ptr, bf_calc1
read bf_calc1, bf_ptr
add bf_calc1, bf_static_one
write bf_ptr, bf_calc1
read bf_calc1, bf_ptr
add bf_calc1, bf_static_one
write bf_ptr, bf_calc1
read bf_calc1, bf_ptr
cmp bf_calc1, bf_static_zero
jz bf_block_0
.label bf_block_0
read bf_calc1, bf_ptr
add bf_calc1, bf_static_one
write bf_ptr, bf_calc1
read bf_calc1, bf_ptr
add bf_calc1, bf_static_one
write bf_ptr, bf_calc1
read bf_calc1, bf_ptr
add bf_calc1, bf_static_one
write bf_ptr, bf_calc1
read bf_calc1, bf_ptr
cmp bf_calc1, bf_static_zero
jz bf_block_2
.label bf_block_2
reads 1, bf_ptr
rpush sp
push t_size, 1
push t_syscall, sys_write
syscall
read bf_calc1, bf_ptr
sub bf_calc1, bf_static_one
write bf_ptr, bf_calc1
read bf_calc1, bf_ptr
cmp bf_calc1, bf_static_zero
jz bf_block_3
jmp bf_block_2
.label bf_block_3

reads 1, bf_ptr
rpush sp
push t_size, 1
push t_syscall, sys_write
syscall
read bf_calc1, bf_ptr
sub bf_calc1, bf_static_one
write bf_ptr, bf_calc1
read bf_calc1, bf_ptr
cmp bf_calc1, bf_static_zero
jz bf_block_1
jmp bf_block_0
.label bf_block_1

reads 1, bf_ptr
push t_syscall, sys_exit
syscall
