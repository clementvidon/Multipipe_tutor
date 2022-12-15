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

/*
 ** @brief      Last program.
 **
 ** @instructions
 **
 ** - Close unused prevpipe
 ** - Wait for children
 ** - Execute
 */

void	ft_last(char **cmd, int len, char **env, int prevpipe)
{
	pid_t	cpid;

	cpid = fork ();
	if (cpid == 0)
	{
		dup2 (prevpipe, STDIN_FILENO);
		close (prevpipe);
		cmd[len] = NULL;
		execve (cmd[0], cmd, env);
	}
	else
	{
		close (prevpipe);
		while (wait (NULL) != -1)
			;
	}
}

/*
 ** @brief      Not last program.
 **
 ** @instructions
 **
 ** Main
 **
 ** - Creates a pipe
 ** - Creates a child
 **
 ** Child (Writer)
 **
 ** - Close unused pipefd[0]
 ** - Redirect STDIN to STDIN or prevpipe
 ** - Redirect STDOUT to pipefd[1]
 ** - Execute
 **
 ** Parent (Reader)
 **
 ** - Close unused pipefd[1]
 ** - Update prevpipe
 */

void	ft_pipe(char **cmd, int len, char **env, int *prevpipe)
{
	int		pipefd[2];
	pid_t	cpid;

	pipe (pipefd);
	cpid = fork ();
	if (cpid == 0)
	{
		close (pipefd[0]);
		dup2 (*prevpipe, STDIN_FILENO);
		close (*prevpipe);
		dup2 (pipefd[1], STDOUT_FILENO);
		close (pipefd[1]);
		cmd[len] = NULL;
		execve (cmd[0], cmd, env);
	}
	else
	{
		close (pipefd[1]);
		close (*prevpipe);
		*prevpipe = pipefd[0];
	}
}

/*
 ** @brief      Find the end of a command and return its index.
 */

int	ft_len(char	**cmd)
{
	int	len;

	len = 0;
	while (cmd[len] && *cmd[len] != '|' && *cmd[len] != ';')
		len++;
	return (len);
}

/*
 ** @brief      Parse the given command.
 **
 ** @usage
 **
 ** Do not forget to surround | and ; with double quotes so that they are not
 ** interpreted by your shell.
 **
 ** Example:
 ** $ gcc multipipe.c && ./a.out \
 **   /bin/echo five "|" /bin/wc -c "|" /bin/cat -e
 **
 ** Check open fds with valgrind:
 ** $ gcc -Wall -Wextra -Werror -g multipipe.c && \
 **   valgrind -q --trace-children=yes --track-fds=yes ./a.out \
 **   /bin/echo five "|" /bin/wc -c "|" /bin/cat -e
 **
 ** @instructions
 **
 ** - Init prevpipe to a valid fd
 */

int	main(int ac, char **cmd, char **env)
{
	int	prevpipe;
	int	len;

	(void)ac;
	len = 0;
	prevpipe = dup (0);
	while (cmd[len] && cmd[len + 1])
	{
		cmd += len + 1;
		len = ft_len (cmd);
		if (cmd[len] != NULL && *cmd[len] == '|')
			ft_pipe (cmd, len, env, &prevpipe);
		else if (cmd[len] == NULL || *cmd[len] == ';')
			ft_last (cmd, len, env, prevpipe);
	}
	return (0);
}
