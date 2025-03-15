
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
			
char **dividir_entrada(char *entrada, const char *delim, int* salidalong) {
    
    	int longitud = 0;
    	char *temp = strdup(entrada);
       	if(!temp){
		char error_message[30] = "An error has occurred\n";
		fprintf(stderr, "%s", error_message);	

    		return NULL;
   	 }
	char *aux = temp;

    	char *tok = strtok(temp, delim);
    	while (tok) {
        	longitud++;
        	tok = strtok(NULL, delim);
    	}
    	free(aux);

    	char **tokens = malloc((longitud + 1) * sizeof(char *));
    	if (!tokens) {
		char error_message[30] = "An error has occurred\n";
		fprintf(stderr, "%s", error_message);	

        	return NULL;
    	}

    	int i = 0;
    	tok = strtok(entrada, delim);
    	while (tok) {
        	tokens[i++] = tok;  
		tok = strtok(NULL, delim);
    	}	
    	tokens[i] = NULL;  
	*salidalong = longitud;
    	return tokens;
}

// devuelve -1 si el redir se hace mal
int redir (char** entrada, int entradalong, int* hayredir, int* fout){
	int i;
	for (i=0; entrada[i]!=NULL ; i++){

		if (strcmp (entrada[i], ">")== 0){
			*hayredir=1;
			break;
		}
	}

	if (*hayredir== 0 ){
		return 0;

	}

	if (i +2 != entradalong || entradalong < 3){
		return -1;
	}

	*fout = open(entrada[entradalong - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

	if(*fout ==-1){
		return -1;				
	}
				
	dup2(*fout,1);
	dup2(*fout,2);
	entrada[entradalong-2] = NULL;
		
			

	return 0;




}

//int buscar_redireccion
int main (int argc, char* argv[]){
	if (argc > 2){
		char error_message[30] = "An error has occurred\n";
		fprintf(stderr, "%s", error_message);	
		exit(1);
	}
	
	FILE *fichero = stdin;

	if (argc==2){
		fichero = fopen(argv[1], "r");
		if(fichero == NULL){
			char error_message[30] = "An error has occurred\n";
			fprintf(stderr, "%s", error_message);
			exit(1)	;
		}
	
	}
	char* buff = NULL;
	size_t tam = 0;

	while(1){




		if (argc == 1){
			fprintf(stdout, "%s", "UVash> "); 
		}

		if (getline(&buff, &tam, fichero) == EOF){
			


			free(buff);
			fclose(fichero);
			exit(0);	
		}
		if (buff[strlen(buff)-1]== '\n'){

			buff[strlen(buff)-1]= '\0';	
		}
		int buffdivlong = 0;
		char* aux = buff;
		char** buffdiv = dividir_entrada(buff, " \t", &buffdivlong);

		// el comando es exit
		if(buffdiv[0] !=NULL && strcmp(buffdiv[0], "exit") == 0){
			if (!buffdiv[1]){
				free(aux);
				free(buffdiv);
				fclose(fichero);
				exit(0);	
			}else{

				char error_message[30] = "An error has occurred\n";
				fprintf(stderr, "%s", error_message);
			}
		// el comando es cd
		}else if(buffdiv[0] !=NULL && strcmp(buffdiv[0], "cd") == 0){
			
			if(buffdivlong == 2){
				
				if(chdir(buffdiv[1])){
					char error_message[30] = "An error has occurred\n";
					fprintf(stderr, "%s", error_message);
				}


			
			}else{
				char error_message[30] = "An error has occurred\n";
				fprintf(stderr, "%s", error_message);

		
			}

		// el comando es otro
		}else{
			// el redir y fork para el & se hace aqui
			int hayredir = 0;
			int fout;
			int ori_stdout = dup(STDOUT_FILENO);
			int ori_stderr = dup(STDERR_FILENO);

			if (redir (buffdiv, buffdivlong , &hayredir, &fout) == -1){

				char error_message[30] = "An error has occurred\n";
				fprintf(stderr, "%s", error_message);
			}else{
			
			
			
			

				pid_t pid= fork();
		
				if (pid == -1){

					char error_message[30] = "An error has occurred\n";
					fprintf(stderr, "%s", error_message);
					exit(1);
	
				}else if(pid == 0){
					
					if(buffdiv[0] !=NULL && execvp(buffdiv[0], buffdiv) == -1){
						char error_message[30] = "An error has occurred\n";
						fprintf(stderr, "%s", error_message);
						exit(1);
					}
				}else{
					int status;
					wait( &status);
				
				}

				if(hayredir){
					close(fout);
					dup2(ori_stdout,1);
					dup2(ori_stderr, 2);
				}
			}

		}
		
		/*		if (strncmp(buff, "exit", 4)==0){
		
			int flag=0;
			for (int i = 4; i< strlen(buff); i++ ){
				if (buff[i]!= ' ' || buff[i] != '\t' ){
					flag =1;
					break;

				}

			}
			if(flag){
				char error_message[30] = "An error has occurred\n";
				fprintf(stderr, "%s", error_message);

				exit(0)	;

			}

			free(buff);
			fclose(fichero);
			exit(0);	
		}
*/

		free(aux);
		free(buffdiv);
		buff =NULL;
		tam =0;
	
	}
	
}
