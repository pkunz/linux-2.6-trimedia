#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* target;
static char* depfile;
static char line[512]={0};

/*
 *  fixdep-pre depfile target
 */
int
main(int argc,char* argv[])
{
	FILE* fd=NULL;
	char* m=NULL;
	char* end=NULL;
	int len=0;
	if(argc!=3){
		printf("Incorrect usage of fixdep-pre!\n");
		abort();
	}

	/*
	 * print the target name to stdout.
	 */
	depfile=argv[1];
	target=argv[2];
	
	fd=fopen(depfile,"r");
	if(fd==NULL){
		printf("Can not open dependence file!\n");
		abort();
	}

	printf("%s: ",target);

	/*
	 * Process the first line
	 */
	end=fgets(line,sizeof(line),fd);
	if(end==NULL){
		return 0;
	}
	len=strlen(line);
	line[len-1]=0;
	m=strchr(line,':');
	if(m==NULL){
		printf("Incorrect dependence file!\n");
		abort();
	}
	m=m+1;
	printf("  %s",m);

	while(!feof(fd)){
		end=fgets(line,sizeof(line),fd);
		if(end==NULL){
			break;
		}
		/*goto next line*/
		printf(" \\\n");
		
		/*process the next line*/
		len=strlen(line);
		line[len-1]=0;
		m=strchr(line,':');
		if(m==NULL){
			printf("Incorrect dependence file!\n");
			abort();			
		}
		m=m+1;
		printf("  %s",m);
	}
	return 0;	
}
