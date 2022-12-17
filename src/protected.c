/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multipipe.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvidon <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/14 08:00:00 by cvidon            #+#    #+#             */
/*   Updated: 2022/12/14 18:00:00 by cvidon           888   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

#include <stdlib.h>
#include <sys/stat.h>

/*
 ** Tools:
 **
 ** #include <sys/stat.h> // tmp
 ** struct stat information; (void)information; // tmp
 ** dprintf(2, "> %i\n", fstat (*prevpipe, &information)); // tmp
 **
 ** valgrind -q --trace-children=yes --track-fds=yes
 **
 ** dprintf (2, "%s: %i\n", __func__, getpid()); // tmp
 */

int	ft_len(char	**cmd)
{
	int	len;

	len = 0;
	while (cmd[len] && *cmd[len] != '|')
		len++;
	return (len);
}

void	ft_perror(char *s1, char *s2)
{
	while (s1 && *s1)
		write (STDERR_FILENO, s1++, 1);
	while (s2 && *s2)
		write (STDERR_FILENO, s2++, 1);
	write (STDERR_FILENO, "\n", 1);
}

void	ft_fatal(int line)
{
	dprintf (2, "@@@@@@@@@@@@@@@@@@@>> l.%i PID_%i\n", line, getpid());
	exit (1);
}

void	ft_last(char **cmd, int len, char **env, int prevpipe)
{
	pid_t	cpid;

	cpid = fork ();
	if (cpid == -1)
		ft_fatal (__LINE__);
	else if (cpid == 0)
	{

		dprintf (2, ">> %i ft_last/child %s\n", getpid(), cmd[0]);

		if (dup2 (prevpipe, STDIN_FILENO) == -1)
			ft_fatal (__LINE__);

		if (close (prevpipe) == -1)
			ft_fatal (__LINE__);

		cmd[len] = NULL;
		if (execve (cmd[0], cmd, env) == -1)
			ft_perror ("error: cannot execute ", cmd[0]);
	}
	else
	{
		dprintf (2, ">> %i ft_last/parent %s\n", getpid(), cmd[0]);
		if (close (prevpipe) == -1)
			ft_fatal (__LINE__);
		while (wait (NULL) != -1)
			;
	}
}

void	ft_pipe(char **cmd, int len, char **env, int *prevpipe)
{
	int		pipefd[2];
	pid_t	cpid;

	if (pipe (pipefd) == -1)
		ft_fatal (__LINE__);
	cpid = fork ();
	if (cpid == -1)
		ft_fatal (__LINE__);
	else if (cpid == 0)
	{
		dprintf (2, ">> %i ft_pipe/child %s\n", getpid(), cmd[0]);
		if (close (pipefd[0]) == -1)
			ft_fatal (__LINE__);
		if (dup2 (*prevpipe, STDIN_FILENO) == -1)
			ft_fatal (__LINE__);

		if (close (*prevpipe) == -1)
			ft_fatal (__LINE__);

		if (dup2 (pipefd[1], STDOUT_FILENO) == -1)
			ft_fatal (__LINE__);
		if (close (pipefd[1]) == -1)
			ft_fatal (__LINE__);
		cmd[len] = NULL;
		if (execve (cmd[0], cmd, env) == -1)
			ft_perror ("error: cannot execute ", cmd[0]);
	}
	else
	{
		dprintf (2, ">> %i ft_pipe/parent %s\n", getpid(), cmd[0]);
		if (close (pipefd[1]) == -1)
			ft_fatal (__LINE__);

		if (close (*prevpipe) == -1)
			ft_fatal (__LINE__);

		*prevpipe = pipefd[0];
	}
}

int	main(int ac, char **cmd, char **env)
{
	int	prevpipe;
	int	len;

	(void)ac;
	len = 0;
	prevpipe = dup (0);
	if (prevpipe == -1)
		ft_fatal (__LINE__);
	while (cmd[len] && cmd[len + 1])
	{
		cmd += len + 1;
		len = ft_len (cmd);
		if (cmd[len] != NULL && *cmd[len] == '|')
			ft_pipe (cmd, len, env, &prevpipe);
		else if (cmd[len] == NULL)
			ft_last (cmd, len, env, prevpipe);
	}
	dprintf (2, "<< %i finished %s\n", getpid(), cmd[0]);
	return (0);
}
