struct alloc_info{
	unsigned long vaddr;
	int 		  num_pages;
	bool		  write;		// permission-related
};

struct free_info{
	unsigned long vaddr;
};

struct sharepage_info{
	unsigned long vaddr;
	int 		  client_pid;
	bool		  write;		// permission-related
};

struct accesspage_info{
	unsigned long vaddr;
};

#define ALLOCATE		_IOW('a', 'a', struct alloc_info *)
#define FREE			_IOW('a', 'b', struct free_info *)
#define SHAREPAGE		_IOW('a', 'c', struct sharepage_info *)
#define ACCESSPAGE		_IOW('a', 'd', struct accesspage_info *)