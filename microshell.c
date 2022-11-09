/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amarzana <amarzana@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/18 13:44:41 by dilopez-          #+#    #+#             */
/*   Updated: 2022/11/09 19:47:22 by amarzana         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int	g_fd;

static int	print(char *str)
{
	int	len = 0;

	while (str[len])
		len++;
	write(2, str, len);
	return (1);
}

static int	executor(char **av, int i, char **ev)
{
	int	status;
	int	fd[2];
	int	pid;
	int	next = 0;

	if (av[i] && strcmp(av[i], "|") == 0)
		next = 1;
	if (av[i] == *av)
		return (0);
	if (pipe(fd) == -1)
		return (print("error: fatal\n"));
	pid = fork();
	if (pid == -1)
		return (print("error: fatal\n"));
	else if (pid == 0)
	{
		close(fd[0]);
		dup2(g_fd, 0);
		av[i] = 0;
		if (next)
			dup2(fd[1], 1);
		close(g_fd);
		close(fd[1]);
		if (execve(*av, av, ev) == -1)
		{
			print("error: cannot execute ");
			print(*av);
			print("\n");
			exit(0);
		}
	}
	else
	{
		close(fd[1]);
		waitpid(pid, &status, 0);
		if (next)
			g_fd = dup(fd[0]);
		else
			close(g_fd);
		close(fd[0]);
	}
	return (0);
}

static int	builtin_cd(char **av)
{
	if (av[2] && strcmp(av[2], "|") != 0 && strcmp(av[2], ";") != 0)
		return (print("error: cd: bad arguments\n"));
	if (chdir(av[1]) == -1)
		return (print("error: cannot execute cd\n"));
	return (0);
}

int	main(int an, char **av, char **ev)
{
	int	i = 1;

	if (an == 1)
		return (0);
	av[an] = 0;
	while (av[i - 1] && av[i])
	{
		av = av + i;
		i = 0;
		while (av[i] && strcmp(av[i], "|") != 0 && strcmp(av[i], ";") != 0)
			i++;
		if (!strcmp(*av, "cd"))
			builtin_cd(av);
		else
			executor(av, i, ev);
		i++;
	}
}
