/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   x.c                                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahernand <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/04 17:26:49 by ahernand          #+#    #+#             */
/*   Updated: 2023/03/11 18:34:36 by ahernand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bircd.h"

int	x_int(int err, int res, char *str, char *file, int line)
{
	if (err == res)
	{
		fprintf(stderr, "%s error (%s, %d): %s\n",
			   	str, file, line, strerror(errno));
		exit(1);
	}
	return (res);
}

void	*x_void(void *err, void *res, char *file, int line)
{
	if (res == err)
	{
		fprintf(stderr, "%s error (%s, %d): %s\n",
				str, file, line, strerror(errno));
		exit(1);
	}
	return (red);
}
