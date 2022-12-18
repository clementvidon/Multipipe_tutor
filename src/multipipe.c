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

/*
 ** @brief      Last program.
 **
 ** @Instructions
 **
 ** 	Parent
 **
 ** 	- Redirect Stdin to prevpipe
 ** 	- Close prevpipe
 ** 	- Execute
 **
 ** 	Child
 **
 ** 	- Close unused prevpipe
 ** 	- Wait for children
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
 **		- create a pipe
 **		- fork itself
 **
 ** 	Child (Writer)
 **
 ** 	- Close unused pipefd[0]
 ** 	- Redirect Stdin to prevpipe
 ** 	- Redirect Stdout to pipefd[1]
 ** 	- Execute
 **
 ** 	Parent (Reader)
 **
 ** 	- Close unused pipefd[1]
 ** 	- Update prevpipe
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
		dup2 (pipefd[1], STDOUT_FILENO);
		close (pipefd[1]);
		dup2 (*prevpipe, STDIN_FILENO);
		close (*prevpipe);
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
 ** @brief      Return command length.
 */

int	ft_len(char	**cmd)
{
	int	len;

	len = 0;
	while (cmd[len] && *cmd[len] != '|')
		len++;
	return (len);
}

/*
 ** @brief      Iterate over piped commands.
 **
 ** @usage
 **
 ** Do not forget to surround "|" with double quotes
 ** so that it is not interpreted by the shell.
 **
 ** Example:
 **
 ** 	./multipipe /bin/echo five "|" /bin/wc -c "|" /bin/cat -e
 **
 ** File descriptors debugging tools:
 **
 ** 	- Open at exit:
 **       valgrind --trace-children=yes --track-fds=yes
 **
 ** 	- Print fds status:
 **
 ** 	  #include <stdio.h>
 ** 	  #include <sys/stat.h>
 **
 ** 	  struct stat information;
 **
 ** 	  dprintf (2, "> %s: %i\n", __func__, fstat (*prevpipe, &information));
 ** 	  dprintf (2, "> %s: %i\n", __func__, getpid());
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
		else if (cmd[len] == NULL)
			ft_last (cmd, len, env, prevpipe);
	}
	return (0);
}
