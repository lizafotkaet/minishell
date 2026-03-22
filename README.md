*This project has been created as part of the 42 curriculum by ebarbash, asrichar.*

Description:

    Minishell is a minimal Unix shell implemented in C. The goal is to reproduce the core behavior of bash: reading a command line, parsing it, and executing it — including pipes, redirections, environment variable expansion, heredocs, and a set of built-in commands.

    The project is split into two parts: a parser that converts raw input into a structured pipeline of commands, and an executor that forks processes, sets up pipes and redirections, and runs each command. Signal handling (Ctrl-C, Ctrl-\) and a readline-based prompt with history are also implemented.

    Parser:

        The parser transforms a raw input string into a t_pipeline — a structured representation of one or more commands connected by pipes.

        Pipeline: create_pipeline(input, env) is the entry point. It tokenizes the input, performs environment variable substitution and quote removal, then splits on | tokens to produce a list of t_command structs.

        Tokenizer: The input is scanned character by character into a t_token_vector. Tokens are classified as words, pipes, or redirections (<, >, >>, <<). Quote boundaries (single and double) are tracked to prevent misclassification of special characters.

        Environment expansion: $VAR references and $? are expanded at token level after tokenization. Inside single quotes, expansion is suppressed. The env is stored as a t_env — a thin wrapper around the environment array.

        Command building: Each segment between pipe tokens is assembled into a t_command containing an argv array, a list of t_redirect structs (type + target filename), and heredoc contents collected via read_heredocs.

        Memory: All structures expose a matching free_* function. A RESULT(T) macro wraps return values to propagate allocation errors without using global state.

Instructions:

    Compile with make in the root folder.

Resources:

    YouTube: Oceano's tutorial on building a simple shell: https://www.youtube.com/watch?v=yTR00r8vBH8
             UNIX signals tutorials: https://www.youtube.com/playlist?list=PLfqABt5AS4FkW5mOn2Tn9ZZLLDwA3kZUY
    Text resources on writing a shell: https://www.cs.purdue.edu/homes/grr/SystemsProgrammingBook/Book/Chapter5-WritingYourOwnShell.pdf
    
    AI (parser): help with outlining the plan of the project, figuring out string vectors and tokenization, help refactoring functions, help with test cases
    GitHub (parser): reading a lot of other people's code and trying to understand different approaches, also testers 

    Also Codam peers :)