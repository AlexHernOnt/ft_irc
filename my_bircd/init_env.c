/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/04 17:21:49 by ahernand          #+#    #+#             */
/*   Updated: 2023/03/11 20:03:21 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <sys/resource.h>
#include "bircd.h"

void		init_env(t_env *e)
{
	int		i;
	struct	rlimit rlp;

	i = 0;
	X(-1, getrlimit(RLIMIT_NOFILE, &rlp), "getlimit");
	e->maxfd = rlp.rlim_cur;
	e->fds = (t_fd*)Xv(NULL, malloc(sizeof(*e->fds) * e->maxfd), "malloc");
	while (i < e->maxfd)
	{
		clean_fd(&e->fds[i])
		i++;
	}
}
