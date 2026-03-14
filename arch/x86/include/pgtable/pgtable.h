#ifndef _TOYOS_PGTABLE
#define _TOYOS_PGTABLE

#include <kloader/pgtable.h>

/**
 * 这套页表采取以下策略控制:
 *  pml4 作为闸门, 它控制这整套页表是内核还是用户
 *  pdpt pde(如果不是大页) 设置为 us=1, nx=0, rw=1
 *  最底层pte/pde(如果是大页) 做详细us nx rw控制
*/
void set_pml4_us(uint64_t vaddr, uint8_t us);

void set_pde_us_bigpage(uint64_t vaddr, uint8_t us);
void set_pte_us(uint64_t vaddr, uint8_t us);
void set_pde_nx_bigpage(uint64_t vaddr, uint8_t nx);
void set_pte_nx(uint64_t vaddr, uint8_t nx);
void set_pde_rw_bigpage(uint64_t vaddr, uint8_t rw);
void set_pte_rw(uint64_t vaddr, uint8_t rw);
void set_pde_pcd_bigpage(uint64_t vaddr, uint8_t pcd);
void set_pte_pcd(uint64_t vaddr, uint8_t pcd);
void set_pde_pwt_bigpage(uint64_t vaddr, uint8_t pwt);
void set_pte_pwt(uint64_t vaddr, uint8_t pwt);

void delete_link(uint64_t vaddr);
#endif