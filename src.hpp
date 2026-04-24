// Copyright (c) 2024 ACM Class, SJTU

namespace sjtu {

class BuddyAllocator {
public:
  BuddyAllocator(int ram_size, int min_block_size) {
    this->ram_size = ram_size;
    this->min_block_size = min_block_size;
    
    // Calculate number of levels
    num_levels = 0;
    int size = min_block_size;
    while (size <= ram_size) {
      num_levels++;
      size *= 2;
    }
    
    // Allocate memory for tracking
    int total_size = 0;
    for (int i = 0; i < num_levels; i++) {
      int level_size = min_block_size * (1 << i);
      int num_blocks = ram_size / level_size;
      total_size += num_blocks;
    }
    
    allocated = new char[total_size];
    for (int i = 0; i < total_size; i++) {
      allocated[i] = 0;
    }
    
    // Calculate offsets for each level
    level_offsets = new int[num_levels];
    level_offsets[0] = 0;
    for (int i = 1; i < num_levels; i++) {
      int prev_level_size = min_block_size * (1 << (i-1));
      int prev_num_blocks = ram_size / prev_level_size;
      level_offsets[i] = level_offsets[i-1] + prev_num_blocks;
    }
  }
  
  ~BuddyAllocator() {
    delete[] allocated;
    delete[] level_offsets;
  }

  int malloc(int size) {
    for (int addr = 0; addr < ram_size; addr += size) {
      if (try_allocate(addr, size)) {
        return addr;
      }
    }
    return -1;
  }

  int malloc_at(int addr, int size) {
    if (try_allocate(addr, size)) {
      return addr;
    }
    return -1;
  }

  void free_at(int addr, int size) {
    int level = get_level(size);
    if (level < 0) return;
    
    // Mark as free
    mark_range(addr, size, false);
    
    // Try to merge buddies recursively
    try_merge(addr, level);
  }

private:
  int ram_size;
  int min_block_size;
  int num_levels;
  char* allocated;
  int* level_offsets;
  
  int get_level(int size) {
    int level = 0;
    int block_size = min_block_size;
    while (block_size < size) {
      block_size *= 2;
      level++;
    }
    if (block_size > ram_size) return -1;
    return level;
  }
  
  bool is_range_free(int addr, int size) {
    // Check if the entire range [addr, addr+size) is free
    for (int a = addr; a < addr + size; a++) {
      if (allocated[a / min_block_size] != 0) {
        return false;
      }
    }
    return true;
  }
  
  void mark_range(int addr, int size, bool alloc) {
    // Mark the range [addr, addr+size) as allocated or free
    for (int a = addr; a < addr + size; a++) {
      allocated[a / min_block_size] = alloc ? 1 : 0;
    }
  }
  
  bool try_allocate(int addr, int size) {
    if (addr < 0 || addr + size > ram_size) return false;
    
    if (is_range_free(addr, size)) {
      mark_range(addr, size, true);
      return true;
    }
    return false;
  }
  
  void try_merge(int addr, int level) {
    // Merging is not strictly necessary for correctness
    // but helps with memory fragmentation
    // For now, we'll keep it simple
  }
};

} // namespace sjtu
