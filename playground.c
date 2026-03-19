
///////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

int	main(int ac, char *av[]){
	int arr[5];
	srand(time(NULL));

	int i;
	for (i = 0; i < 5; i++){
		arr[i] = rand() % 100;
	}

	return 0;
}

// int	main(int ac, char **av)
// {
// 	(void)ac;
// 	(void)av;

// 	if (mkfifo("myfifo1", 0777) == -1){
// 		if (errno != EEXIST){
// 			printf("Could not creat fifo file\n");
// 			return 1;
// 		}
// 	}

// 	printf("Opening...\n");
// 	int	fd = open("myfifo1", O_WRONLY);
// 	if (fd == -1){
// 		return 3;
// 	}
// 	printf("Opened\n");
// 	printf("%d\n", fd);
// 	int	x = 97;
// 	if (write(fd, &x, sizeof(x)) == -1){
// 		return 2;
// 	}
// 	printf("Written\n");
// 	close(fd);
// 	printf("Closed\n");
// 	return 0;
// }


// int	main(int ac, char *av[]){
// 	(void)ac;
// 	(void)av;
	
// 	int arr[] = {1, 2, 4, 5, -9, 0, 4, 3, 10};
// 	int arr_size = sizeof(arr) / sizeof(int);
// 	int start = 0, end = 0;

// 	int	fd[2];
// 	if (pipe(fd) == -1)
// 		return 1;

// 	int id = fork();
// 	if (id == -1)
// 		return 2;

// 	int id_child;

// 	if (id == 0){
// 		int	fd_child[2];

// 		if (pipe(fd_child) == -1)
// 			return 3;

// 		id_child = fork();
// 		if (id_child == -1)
// 			return 4;
// 		if (id_child == 0)
// 		{
// 			start = 0;			// for grandchild process
// 			end = arr_size / 3;
// 			int sum = 0;
// 			int n;
// 			for (n = start; n < end; n++){
// 				sum += arr[n];
// 			}
// 			printf("Sum for the grandchild: %d\n", sum);
// 			close(fd_child[0]);
// 			write(fd_child[1], &sum, sizeof(sum));
// 			close(fd_child[1]);

// 			wait(NULL);
// 		}
// 		else{
// 			start = ( arr_size / 3 ); // for child process
// 			end = ( arr_size / 3 ) * 2; 
// 			int mid = 0;
// 			read(fd_child[0], &mid, sizeof(int));
// 			close(fd_child[0]);

// 			int j;
// 			for (j = start; j < end; j++){
// 				mid += arr[j];
// 			}
// 			printf("Sum for the child: %d\n", mid);
// 			write(fd[1], &mid, sizeof(int));
// 			close(fd[1]);

// 			wait(NULL);
// 		}

// 	}
// 	else{
// 		start = (arr_size / 3) * 2; // for parent process
// 		end = arr_size;
// 		int total;
// 		read(fd[0], &total, sizeof(int));
// 		close(fd[0]);
// 		int i;
// 		for (i = start; i < end; i++){
// 			total += arr[i];
// 		}
// 		printf("Total sum: %d\n", total);

// 		wait(NULL);
// 	}

// 	return 0;
// }


// int	main(int ac, char *av[]){
// 	(void)ac;
// 	(void)av;
// 	int	fd[2];
// 	if (pipe(fd) == -1)
// 		printf("Error occured while creating a pipe\n");
	
// 	int id  = fork();
// 	(void)id;
// 	if (id == 0)
//     	printf("Child: fd[0] = %d, fd[1] = %d\n", fd[0], fd[1]);
// 	else
//     	printf("Parent: fd[0] = %d, fd[1] = %d\n", fd[0], fd[1]);
// 	if (id == 0){
// 		close(fd[0]);
// 		int x;
// 		printf("Input a number: ");
// 		scanf("%d", &x);
// 		write(fd[1], &x, sizeof(int));
// 		close(fd[1]);
// 		printf("x = %d\n", x);
// 	}
// 	else {
// 		close(fd[1]);
// 		int	y;
// 		read(fd[0], &y, sizeof(int));
// 		printf("y = %d\n", y);
// 		close(fd[0]);
// 	}
// 	while (1){
// 		if (wait(NULL) > 0)
// 			continue;
// 		if (wait(NULL) == -1 && errno == EINTR)
// 			continue;
// 		break;
// 	}
// 	return (0);
// }

// int	main(int ac, char **av)
// {
// 	(void)ac;
// 	(void)av;

// 	int id1 = fork();
// 	int id2 = fork();

// 	if (id1 == 0){ // id1 == b
// 		if (id2 == 0){
// 			printf("This is the bottom process d\n");
// 		}
// 		else{ // id2 == d
// 			printf("This is the middle process b\n");
// 		}
// 	}
// 	else{ // id1 == a
// 		if (id2 == 0){
// 			printf("This is the right process c\n");
// 		}
// 		else{ // id2 == c
// 			printf("This is the parent process a\n");
// 		}
// 	}

// 	while (1){
// 		int id = wait(NULL);
// 		if (id > 0){
// 			printf("Child process %d finished\n", id);
// 			continue;
// 		}
// 		if (id == -1 && errno == EINTR){
// 			printf("Error: %s\n", strerror(errno));
// 			continue;
// 		}
// 		break;
// 	}
// 	if (id1 != 0 && id2 != 0){
// 		printf("Parent process finished\n");
// 	}
// 	//wait(NULL);
// 	return (0);
// }
