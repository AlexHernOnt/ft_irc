/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   birdc.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/25 16:49:50 by ahernand          #+#    #+#             */
/*   Updated: 2023/03/11 18:15:14 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BIRCD_H
# define BIRCD_H

# include <sys/select.h>
# include <stdio.h>

# define FD_FREE    0
# define FD_SERV    1
# define FD_CLIENT  2

# define BUF_SIZE   4096

# define X(err,res,str)		(x_int(err, res, str, __FILE, __LINE__))
# define Xv(err,res,str)    (x_void(err,res,str,__FILE__,__LINE__))

typedef struct	s_fd
{
	int			type;
	void		(*fct_read)();
	void		(*fct_write)();
	char		buf_read[BUF_SIZE + 1];
	char		buf_write[BUF_SIZE + 1];
}				t_fd;

typedef struct	s_env
{
	t_fd		*fds;
	int			port;
	int			maxfd;
	int			max;
	int			r;
	fd_set		ft_read;
	fd_set		ft_write;
}				t_env;

/*
**	Preparing FDs
*/

void init_env(t_env *e);

/*
**	Errors
*/

int x_int(int err, int res, char *str, char *file, int line);


#endif
