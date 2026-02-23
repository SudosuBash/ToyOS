#include <kloader.h>
#include <pgtable.h>

static inline void memcpy(uint8_t* src,uint8_t* dst,uint32_t size) {
    for(int i=0;i<size;i++) {
        dst[i] = src[i];
    }
}

static void proc_program_header(Elf32_Phdr* phdr,void* src_base, void* dst_base,int cnt) {
    uint8_t* st_addr = dst_base; //目标地址
    for(Elf32_Phdr* p = phdr;p < phdr + cnt;p++) {
        if(p->p_type == PT_LOAD) { //展开段
            uint64_t vaddr = p->p_vaddr & PAGE_MASK;    
            uint32_t offset = (p->p_vaddr - vaddr);

            uint8_t* dat = src_base+p->p_offset; //段数据
            memcpy(dat,st_addr+offset,p->p_filesz);
            
            int pg_sz = (p->p_memsz + offset + PAGE_SZ-1 ) & PAGE_MASK;
            //总共的页数
            int pg_cnt = pg_sz >> PAGE_OFFSET;
            
            for(int i=0;i<pg_cnt;i++) { //页表映射
                link_new_pte_addr((uint32_t)st_addr,vaddr);
                vaddr += PAGE_SZ;
                st_addr += PAGE_SZ;
            }
        }
    }
}

Elf32_Addr load_elf(void* src,void* dst) {
    Elf32_Ehdr* ehdr = (Elf32_Ehdr*)src;

    Elf32_Phdr* phdr = (Elf32_Phdr*)(src + ehdr->e_phoff);
    proc_program_header(phdr, src, dst, ehdr->e_phnum);
    return ehdr->e_entry;
}