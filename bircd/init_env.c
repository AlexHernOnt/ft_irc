#include <stdlib.h>
#include <sys/resource.h>
#include "bircd.h"

void		init_env(t_env *e)
{
	int		i;
	struct	rlimit	rlp;

	i = 0;
	X(-1, getrlimit(RLIMIT_NOFILE, &rlp), "getrlimit");												// X is an error function, will send error when there is no more space for more fd for ... (???)
	e->maxfd = rlp.rlim_cur;
	e->fds = (t_fd*)Xv(NULL, malloc(sizeof(*e->fds) * e->maxfd), "malloc");
	while (i < e->maxfd)
	{
		clean_fd(&e->fds[i]);
		i++;
	}
}
