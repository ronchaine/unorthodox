#ifndef UNORTHODOX_UNIX_PIPES_HPP
#define UNORTHODOX_UNIX_PIPES_HPP

#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>

#include <array>

namespace unorthodox::unix
{
    class process
    {
        public:
            template <typename... Args>
            process(const char* command, Args... args)
            {
                pipe(parent_write_pipe);
                pipe(parent_read_pipe);

                pid = fork();

                if (pid == -1)
                {
                    close(parent_write_pipe[0]);
                    close(parent_write_pipe[1]);
                    close(parent_read_pipe[0]);
                    close(parent_read_pipe[1]);
                    return;
                }
                if (pid == 0) // child
                {
                    dup2(parent_write_pipe[READ_FD], STDIN_FILENO);
                    dup2(parent_read_pipe[WRITE_FD], STDOUT_FILENO);

                    close(parent_write_pipe[READ_FD]);
                    close(parent_read_pipe[WRITE_FD]);

                    close(parent_read_pipe[READ_FD]);
                    close(parent_write_pipe[WRITE_FD]);

                    execlp(command, command, args..., nullptr);
                }

                // parent
                close(parent_write_pipe[READ_FD]);
                close(parent_read_pipe[WRITE_FD]);
            }

            ~process()
            {
                close(parent_write_pipe[WRITE_FD]);
                close(parent_write_pipe[READ_FD]);
                close(parent_read_pipe[WRITE_FD]);
                close(parent_read_pipe[READ_FD]);
            }

            operator bool()
            {
                if (pid < 0)
                    return false;

                int status = 0;
                if (waitpid(pid, &status, WNOHANG) != 0)
                    return false;

                pid = -1;
                return true;
            }

        private:
            constexpr static int READ_FD = 0;
            constexpr static int WRITE_FD = 1;

            int parent_write_pipe[2];
            int parent_read_pipe[2];

            pid_t pid;
    };
}

#endif
